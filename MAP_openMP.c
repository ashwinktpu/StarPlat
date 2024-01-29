// False sharing

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define size 100000


//Structure of the Map Table
struct table{
	int key;
	int value;
	int fill;
	int delet;
};


//Structure of the Element 
struct element{
	int key;
	int value;
};


//Defining Lock for each index of Map Table 
omp_lock_t lock[size];


//Hash Function(Should not be linear)
int doubleDashingFunction(int num, int i){

	int f1 = (num);
	// int f2 = (num%5);
	return (f1+(i*i))%size;

}

void insertFun(int insertSize, int (*insertEl)[2], int *workDone, int *searchDone, struct table *mapTable){

	#pragma omp for
	for(int i=0 ; i<insertSize ; i++){

		//taking key and value from input
		int key, value, hashedVal;
		key = insertEl[i][0];
		value = insertEl[i][1];	

		//counter helps in probing
		int counter = 0;

		//Run the loop untill the insertion is not marked
		while(!workDone[i] && !searchDone[i]){

			// printf("tid = %d and key = %d\n", omp_get_thread_num(), key);
			hashedVal = doubleDashingFunction(key, counter);
			// printf(" hashedVal = %d \n", hashedVal);

			//Check the condition if the respective postion is occupied or not
			if(mapTable[hashedVal].fill==0 || (mapTable[hashedVal].fill==1 && mapTable[hashedVal].delet==1)){

				//lock the respective index of Map Table where we have to insert
				omp_set_lock(&lock[hashedVal]);

				//Again checking so that other thread should not fill at same location
				if(mapTable[hashedVal].fill==0 || (mapTable[hashedVal].fill==1 && mapTable[hashedVal].delet==1)){						

					mapTable[hashedVal].key = key;
					mapTable[hashedVal].value = value;

					if(mapTable[hashedVal].fill==0)
						mapTable[hashedVal].fill=1;
					else
						mapTable[hashedVal].delet=0;

					workDone[i] = 1;

					//Unset the lock
					omp_unset_lock(&lock[hashedVal]);		

				}
				else{

					counter++;
					//After one round of traversal of whole Map Table, break out loop
					if(counter>30){
						//Unset the lock
						omp_unset_lock(&lock[hashedVal]);	//I don't think it is required here					
						break;	
					}

				}

			}else{

				counter++;
				//After one round of traversal of whole Map Table, break out loop
				if(counter>30){
						break;	
				}

			}
			// printf("tid = %d and key = %d value = %d and work=%d and counter = %d\n", omp_get_thread_num(), key, value, workDone[i], counter);
		}

	}

}



void searchPairFun(int searchSize, int (*searchEl)[2], int *searchDone, struct table *mapTable){

	//Start of Directive to run all iteration in parallel 
	#pragma omp parallel for
	for (int i = 0; i < searchSize; ++i)
	{
		int key, value, hashedVal;

		key = searchEl[i][0];
		value = searchEl[i][1];


		int counter = 0;

		while(!searchDone[i]){

			hashedVal = doubleDashingFunction(key, counter);
			if(!mapTable[hashedVal].fill)
				break;
			else{
				if(mapTable[hashedVal].key == key && mapTable[hashedVal].value == value){
					searchDone[i]=1;
				}else{

					counter++;
					//After one round of traversal of whole Map Table, break out loop
					if(counter>30)
						break;
				}

			}
		}
		
	}
	//End of Directive to run all iteration in parallel 	

}

void searchKeyFun(int searchSize, int *ansCounter, int *searchEl, int *searchDone, struct table *mapTable, int *checkSearch){

	//Start of Directive to run all iteration in parallel 
	#pragma omp parallel for
	for(int i=0 ; i<searchSize ; i++){

		int key = searchEl[i];
		int hashedVal, counter=0;


		while(1){

			hashedVal = doubleDashingFunction(key, counter);

			if(mapTable[hashedVal].fill==0){
				break;
			}
			else{

				if(mapTable[hashedVal].key == key){

					//printf("\nmatch key = %d\n", key);
					checkSearch[hashedVal] = 1;

					//Directive to put atomic condition on ansCounter variable
					#pragma omp atomic
					(*ansCounter)++;
					counter++;	
					searchDone[i] = 1;
				}
				else{
					counter++;
					//After one round of traversal of whole Map Table, break out loop
					if(counter>30)
						break;
				}
			}
		}
	}
	//End of Directive to run all iteration in parallel 



}

void searchKeyAnsInsert(int searchSize, int *insertIndex, int *searchEl, int *searchDone, int *checkSearch, struct table *mapTable, struct element *ans){

	//Start of Directive to run all iteration in parallel 
	#pragma omp parallel for
	for(int i=0 ; i<searchSize ; i++){

		if(searchDone[i]){

			int key = searchEl[i];
			int counter = 0;
			int hashedVal;
			while(1){

				hashedVal = doubleDashingFunction(key, counter);
				if(hashedVal < 0){
					counter++;
					continue;
				}

				if(checkSearch[hashedVal]==1 || mapTable[hashedVal].fill == 0)
					break;

				if(mapTable[hashedVal].key == key){
					
					//lock the respective index of Map Table so that only 1 thread can fetch the value
					omp_set_lock(&lock[hashedVal]);
					if(checkSearch[hashedVal] == 0 ){
						//printf("\nkey = %d and value = %d\n",mapTable[hashedVal].key, mapTable[hashedVal].value);
						ans[(*insertIndex)].key = mapTable[hashedVal].key;
						ans[(*insertIndex)].value = mapTable[hashedVal].value;
						(*insertIndex)++;
						checkSearch[hashedVal] = 1;
					}
					omp_unset_lock(&lock[hashedVal]);
					counter++;							

				}
				else
				{
					counter++;
				}

			}

		}
	}
	
	//End of Directive to run all iteration in parallel 


}


void deletePairFun(int deletBatchSize, int (*deletBatchEl)[2], int *checkDel, struct table *mapTable){
	//Start of Directive to run all iteration in parallel 
	#pragma omp parallel for 
	for(int i=0 ; i<deletBatchSize ; i++){

		int key = deletBatchEl[i][0];
		int value = deletBatchEl[i][1];

		int counter = 0;

		int hashedVal;

		while(1){

			hashedVal = doubleDashingFunction(key, counter);
			
			if(mapTable[hashedVal].fill == 0)
				break;

			if(mapTable[hashedVal].key == key && mapTable[hashedVal].value == value){
				mapTable[hashedVal].key = 0;
				mapTable[hashedVal].value = 0;
				mapTable[hashedVal].delet = 1;
				break;
			}

			counter++;
			//After one round of traversal of whole Map Table, break out loop
			if(counter>30)
				break;

		}

	}
	//End of Directive to run all iteration in parallel 



}


void deletKeyFun(int deletBatchSize, int *deletBatchEl, int *checkDel, struct table *mapTable){

	//Start of Directive to run all iteration in parallel 
	#pragma omp parallel for 
	for(int i=0 ; i<deletBatchSize ; i++){

		int key = deletBatchEl[i];				
		int counter = 0;
		int hashedVal;

		while(1){

			hashedVal = doubleDashingFunction(key, counter);

			if(mapTable[hashedVal].fill == 0)
				break;

			if(mapTable[hashedVal].key == key && mapTable[hashedVal].delet != 1){
				mapTable[hashedVal].key = 0;
				mapTable[hashedVal].value = 0;
				mapTable[hashedVal].delet = 1;						
			}

			counter++;
			//After one round of traversal of whole Map Table, break out loop
			if(counter>30)
				break;
		}
	}
	//End of Directive to run all iteration in parallel 


}




int main(){

	//Time variable
	double t1,t2;
	int num = omp_get_num_threads();
        #pragma omp single
        printf("No of Threads: = %d\n",num);



	//Initialization of locks using parallel threads
	#pragma omp for
	for(int i = 0; i < size; ++i){
		omp_init_lock(&lock[i]);
	}
	

	//Handling the operation using condtional variable
	int insert=1, insertA=1, search=0, del=0,searchPair=0, searchKey=0, deletKey=0, deletPair=0;


	//Creating Map Table Dynamically
	struct table *mapTable;
	mapTable = (struct table*)malloc(size*sizeof(struct table));

	for(int i=0 ; i<size ; i++){
		mapTable[i].key = -3;
		mapTable[i].value = -3;
		mapTable[i].fill = 0;
		mapTable[i].delet = 0;
	}


	//Insertion in Map Data Structure to test insertion before next insertion
	if(insert){		

		//Insertion Batch Size
		int insertSize = 10000;
		int insertEl[insertSize][2];


		t1 = omp_get_wtime();

		//for Marking the index of Insertion Batch Element
		int *workDone;
		workDone = (int*)malloc(insertSize*sizeof(int));

		int *searchDone;
		searchDone = (int*)malloc(insertSize*sizeof(int));


		/*Start of Dummy Input*/
		insertEl[0][0] = 59;
		insertEl[0][1] = 6;
		workDone[0] = 0;
		searchDone[0] = 0;

		insertEl[1][0] = 59;
		insertEl[1][1] = 8;
		workDone[1] = 0;
		searchDone[1] = 0;

		insertEl[2][0] = 59;
		insertEl[2][1] = 9;
		workDone[2] = 0;
		searchDone[2] = 0;

		for(int i=0 ; i<insertSize ; i++){
			insertEl[i][0] = i;
			insertEl[i][1] = i+7	;
			workDone[i] = 0;	//This is important
			searchDone[i] = 0;
		}
		/*End of Dummy Input*/


		// First need to search that all {key, value} pair present or not
		
		

		//Start of Directive to run all iteration in parallel 
			#pragma omp parallel for
			for (int i = 0; i < insertSize; ++i)
			{
				int key, value, hashedVal;

				key = insertEl[i][0];
				value = insertEl[i][1];


				int counter = 0;

				while(!searchDone[i]){

					hashedVal = doubleDashingFunction(key, counter);
					if(!mapTable[hashedVal].fill)
						break;
					else{
						if(mapTable[hashedVal].key == key && mapTable[hashedVal].value == value){
							searchDone[i]=1;
						}else{

							counter++;
							//After one round of traversal of whole Map Table, break out loop
							if(counter>30)
								break;
						}

					}
				}
				
			}
			//End of Directive to run all iteration in parallel 

			


			//Count the successful search
			// int success=0;

			// /*Start Output*/
			// printf("\n\nSearched Element in the Hash Table:\n\n");
			// for(int i=0 ; i<searchSize ; i++){
			// 	if(searchDone[i]){
			// 		// printf("key = %d  value = %d \n", searchEl[i][0], searchEl[i][1]);
			// 		success++;
			// 	}
			// }			


		
		



		// Search Done


		//Start of Directive to run all iteration in parallel 
		#pragma omp for
		for(int i=0 ; i<insertSize ; i++){

			//taking key and value from input
			int key, value, hashedVal;
			key = insertEl[i][0];
			value = insertEl[i][1];

			//counter helps in probing
			int counter = 0;

			//Run the loop untill the insertion is not marked
			while(!workDone[i] && !searchDone[i]){

				// printf("tid = %d and key = %d\n", omp_get_thread_num(), key);
				hashedVal = doubleDashingFunction(key, counter);
				// printf(" hashedVal = %d \n", hashedVal);

				//Check the condition if the respective postion is occupied or not
				if(mapTable[hashedVal].fill==0 || (mapTable[hashedVal].fill==1 && mapTable[hashedVal].delet==1)){

					//lock the respective index of Map Table where we have to insert
					omp_set_lock(&lock[hashedVal]);

					//Again checking so that other thread should not fill at same location
					if(mapTable[hashedVal].fill==0 || (mapTable[hashedVal].fill==1 && mapTable[hashedVal].delet==1)){						

						mapTable[hashedVal].key = key;
						mapTable[hashedVal].value = value;

						if(mapTable[hashedVal].fill==0)
							mapTable[hashedVal].fill=1;
						else
							mapTable[hashedVal].delet=0;

						workDone[i] = 1;

						//Unset the lock
						omp_unset_lock(&lock[hashedVal]);		

					}
					else{

						counter++;
						//After one round of traversal of whole Map Table, break out loop
						if(counter>30){
							//Unset the lock
							omp_unset_lock(&lock[hashedVal]);	//I don't think it is required here					
							break;	
						}

					}

				}else{

					counter++;
					//After one round of traversal of whole Map Table, break out loop
					if(counter>30){
							break;	
					}

				}
				// printf("tid = %d and key = %d value = %d and work=%d and counter = %d\n", omp_get_thread_num(), key, value, workDone[i], counter);
			}

		}
		//End of Directive to run all iteration in parallel 
		

		t2 = omp_get_wtime();

		

		//Number of Successful Insertion
		int success=0;

		/*Start Output*/
		// printf("\n\nElement in the Hash Table:\n\n");

		for(int i=0 ; i<size ; i++){

			if(mapTable[i].fill==1 && mapTable[i].delet==0){
		//		printf("key = %d  value = %d at i=%d\n", mapTable[i].key, mapTable[i].value,i);
				success++;
			}

		}
		
		printf("\nmap size  = %d Insertion sucess = %d and time = %g\n", size, success, t2-t1);

		/*End Output*/

	}



	// Insert again to check duplicate insertion

	if(insertA){		

		printf("\nInsert Again\n");
		//Insertion Batch Size
		int insertSize = 8000;
		int insertEl[insertSize][2];


		t1 = omp_get_wtime();

		//for Marking the index of Insertion Batch Element
		int *workDone;
		workDone = (int*)malloc(insertSize*sizeof(int));

		int *searchDone;
		searchDone = (int*)malloc(insertSize*sizeof(int));


		/*Start of Dummy Input*/
		insertEl[0][0] = 59;
		insertEl[0][1] = 6;
		workDone[0] = 0;
		searchDone[0] = 0;

		insertEl[1][0] = 59;
		insertEl[1][1] = 8;
		workDone[1] = 0;
		searchDone[1] = 0;

		insertEl[2][0] = 59;
		insertEl[2][1] = 9;
		workDone[2] = 0;
		searchDone[2] = 0;

		for(int i=0 ; i<insertSize ; i++){
			insertEl[i][0] = i;
			insertEl[i][1] = i+8;
			workDone[i] = 0;	//This is important
			searchDone[i] = 0;
		}
		/*End of Dummy Input*/


		// First need to search that all {key, value} pair present or not	
		

		//Start of Directive to run all iteration in parallel 
		
		searchPairFun(insertSize, insertEl, searchDone, mapTable);
		



		// Search Done
		
		


		//Start of Directive to run all iteration in parallel 
		
		insertFun(insertSize, insertEl, workDone, searchDone, mapTable);
		
		
		//End of Directive to run all iteration in parallel 
		

		t2 = omp_get_wtime();

		

		//Number of Successful Insertion
		int success=0;

		/*Start Output*/
		// printf("\n\nElement in the Hash Table:\n\n");

		for(int i=0 ; i<size ; i++){

			if(mapTable[i].fill==1 && mapTable[i].delet==0){
//				printf("key = %d  value = %d at i=%d\n", mapTable[i].key, mapTable[i].value,i);
				success++;
			}

		}
		
		printf("\nmap size  = %d Insertion sucess = %d and time = %g\n", size, success, t2-t1);

		/*End Output*/

		printf("\nInsert Again End\n");

	}


	//Search in Map Data Structure
	if(search==1){

		//Search with pair(key, value)
		if(searchPair==1){
		
			//Search Batch Size
			int searchSize = 100000;
			int searchEl[searchSize][2];

			t1 = omp_get_wtime();

			//Array to mark the elements has been searched or not
			int *searchDone;
			searchDone = (int*)malloc(searchSize*sizeof(int));

			/*Start of Dummy Input*/
			searchEl[0][0] = 0;
			searchEl[0][1] = 7;
			searchDone[0] = 0;

			searchEl[1][0] = 7;
			searchEl[1][1] = 14;
			searchDone[1] = 0;

			searchEl[2][0] = 14;
			searchEl[2][1] = 21;
			searchDone[2] = 0;
			for(int i=3 ; i<searchSize ; i++){
				searchEl[i][0] = i;
				searchEl[i][1] = 7+i;
				searchDone[i] = 0;
			}

			
			/*End of Dummy Input*/


			searchPairFun(searchSize, searchEl, searchDone, mapTable);
			
			t2 = omp_get_wtime();


			//Count the successful search
			int success=0;

			/*Start Output*/
			printf("\n\nSearched Element in the Hash Table:\n\n");
			for(int i=0 ; i<searchSize ; i++){
				if(searchDone[i]){
					// printf("key = %d  value = %d \n", searchEl[i][0], searchEl[i][1]);
					success++;
				}
			}			
			printf("\nSearch sucess = %d and time = %g\n", success, t2-t1);
			/*End Output*/

		}


		//Search with key only
		if(searchKey==1){

			int searchSize = 100000;
			int searchEl[searchSize];			

			t1 = omp_get_wtime();		

			/*Start of Dummy Input*/
			searchEl[0] = 1;
			searchEl[1] = 8;
			searchEl[2] = 9;
			for(int i=0 ; i<searchSize ; i++){
				searchEl[i] = 0;
				// searchDone[i] = 0;
			}
			
			/*End of Dummy Input*/

			int *searchDone = (int*)malloc(searchSize*sizeof(int));

			#pragma omp for
			for(int i = 0; i < searchSize; ++i){
				searchDone[i] = 0;
			}


			int checkSearch[size] = {0};

			//Counter to track number of matched results
			int  ansCounter=0;
			
			searchKeyFun(searchSize, &ansCounter, searchEl, searchDone, mapTable, checkSearch); 
			
			printf("total found = %d\n", ansCounter);

			

			//checkSearch array used to accumulate the results
			#pragma omp parallel for
			for(int i=0 ; i<size ; i++)
				checkSearch[i]=0;


			//Store results in ans array 
			struct element *ans = (struct element*)malloc(ansCounter*sizeof(struct element));
			int insertIndex=0;
			
			searchKeyAnsInsert(searchSize, &insertIndex, searchEl, searchDone, checkSearch, mapTable, ans);

			
			t2 = omp_get_wtime();
		
			/*Start Output*/
			printf("\n\nNumber of elements found = %d and time = %g\n\n",ansCounter, t2-t1);
			printf("\n\nNumber of elements non overlapped found = %d and time = %g\n\n",insertIndex, t2-t1);
			//for(int i=0 ; i<insertIndex ; i++){
			//	printf("key = %d and value = %d\n", ans[i].key, ans[i].value);
			//}
			/*End Output*/

		}//End with key only
	
	}


	//Delete operation in Map Data Structure
	if(del==1){
		
		//Delete with pair(key, value)
		if(deletPair == 1){

			int deletBatchSize = 100000;
			int deletBatchEl[deletBatchSize][2];

			/*Start of Dummy Input*/
			printf("Elements to Delete : \n\n");
			for(int i=0 ; i<deletBatchSize ; i++){
				deletBatchEl[i][0] = 5+i;
				deletBatchEl[i][1] = i+7;
//				//printf("Dkey = %d Dvalue = %d\n", deletBatchEl[i][0], deletBatchEl[i][1]); 
			}
			printf("\n");

			/*End of Dummy Input*/


			t1 = omp_get_wtime();
			int checkDel[size]={0};	
			
			deletePairFun(deletBatchSize, deletBatchEl, checkDel, mapTable);				

			
			t2 = omp_get_wtime();
			int total=0;

			/*Start Output*/
			//printf("\n\nElement in the Hash Table:\n\n");
			for(int i=0 ; i<size ; i++){				
				if(mapTable[i].fill==1 && mapTable[i].delet==0){
			//		printf("key = %d  value = %d at i=%d\n", mapTable[i].key, mapTable[i].value,i);
					total++;
				}
			}			
			printf("\nmap size  = %d total reside = %d and time = %g\n", size, total, t2-t1);
			/*End Output*/
			
		}

		//Delete Element with only key matched
		if(deletKey == 1){

			int deletBatchSize = 100000;
			int deletBatchEl[deletBatchSize];

			/*Start of Dummy Input*/
			//printf("Elements to Delete : \n\n");
			for(int i=0 ; i<deletBatchSize ; i++){
				deletBatchEl[i] = i+5;				
				//printf("Dkey = %d \n", deletBatchEl[i]); 
			}
			printf("\n");
			/*End of Dummy Input*/

			t1 = omp_get_wtime();

			int checkDel[size]={0};
			
			deletKeyFun(deletBatchSize, deletBatchEl, checkDel, mapTable);

			

			t2 = omp_get_wtime();
			int total=0;

			/*Start Output*/
			//printf("\n\nElement in the Hash Table:\n\n");
			for(int i=0 ; i<size ; i++){				
				if(mapTable[i].fill==1 && mapTable[i].delet==0){
					//printf("key = %d  value = %d at i=%d\n", mapTable[i].key, mapTable[i].value,i);
					total++;
				}
			}			
			printf("\nmap size  = %d total reside = %d and time = %g\n", size, total, t2-t1);
			/*End Output*/
		}
	}

	//Destroy the lock
	#pragma omp for
	for(int i = 0; i < size; ++i){
		omp_destroy_lock(&lock[i]);
	}

	return 0;

}
