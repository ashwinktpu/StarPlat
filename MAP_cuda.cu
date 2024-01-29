//%%cu
//#include<bits/stdc++.h>
#include<iostream>
#include<sys/time.h>
#include<cuda.h>

#define maxSize 10000000
#define block_size 1024

using namespace std;

struct cmap{
    int key;
    int value;
    int fill;
};


struct element_pair{
    int key;
    int value;
    int find;
};

struct element{
    int key;
    int find;
};


__global__ void kernel_index_to_fill(cmap *d_map, int *d_index_to_fill_in_hash_table, int *counter, int map_size){
    
     int tid = blockDim.x * blockIdx.x + threadIdx.x;                                              
     if(tid < map_size){
      
          if(d_map[tid].fill == 0){               
              
                int index = atomicInc((unsigned *)counter, maxSize);
                d_index_to_fill_in_hash_table[index] = tid;                
          }    
        
     }                     
    
}

__global__ void kernel_to_insert(cmap *d_map, element_pair *d_input, int *d_index_to_fill_in_hash_table, int insert_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;                                              
    if(tid < insert_batch_size && d_input[tid].find==0){
          d_map[d_index_to_fill_in_hash_table[tid]].key = d_input[tid].key;
          d_map[d_index_to_fill_in_hash_table[tid]].value = d_input[tid].value;
          d_map[d_index_to_fill_in_hash_table[tid]].fill = 1; 
    }
    
}


__global__ void search_kernel_pair(element_pair *d_search_input_pair, cmap *d_map, int a_size, int search_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < search_batch_size * a_size){

        int index_of_element = tid % search_batch_size;
        int index_of_location = tid / search_batch_size;

        if(d_search_input_pair[index_of_element].key == d_map[index_of_location].key
           && d_search_input_pair[index_of_element].value == d_map[index_of_location].value && d_map[index_of_location].fill == 1){

            d_search_input_pair[index_of_element].find = 1;

        }

    }
}


__global__ void search_kernel_key(element *d_search_input_key, cmap *d_map, int a_size, int search_batch_size, int *d_count){

    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < search_batch_size * a_size){

        int index_of_element = tid % search_batch_size;
        int index_of_location = tid / search_batch_size;

        if(d_search_input_key[index_of_element].key == d_map[index_of_location].key){

            int temp = atomicInc((unsigned int *)d_count, -1);
            //d_search_input_pair[index_of_element].find = 1;

        }
    }

}

__global__ void fill_search_kernel_key(element *d_search_input_key, cmap *d_map, int a_size, int search_batch_size, element_pair *d_search_input_key_ans, int *index){

    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < search_batch_size * a_size){

        int index_of_element = tid % search_batch_size;
        int index_of_location = tid / search_batch_size;

        if(d_search_input_key[index_of_element].key == d_map[index_of_location].key){
            int ind = atomicInc((unsigned int *)index, -1);
            d_search_input_key_ans[ind].key = d_map[index_of_location].key;
            d_search_input_key_ans[ind].value = d_map[index_of_location].value;
        }
    }
}



__global__ void delete_kernel_pair(element_pair *d_delete_input_pair, cmap *d_map, int a_size, int delete_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < delete_batch_size * a_size){

        int index_of_element = tid % delete_batch_size;
        int index_of_location = tid / delete_batch_size;

        if(d_delete_input_pair[index_of_element].key == d_map[index_of_location].key
           && d_delete_input_pair[index_of_element].value == d_map[index_of_location].value){

            d_map[index_of_location].key = 0;
            d_map[index_of_location].value = 0;
            d_map[index_of_location].fill = 0;
        }

    }

}

__global__ void delete_kernel(element *d_delete_input, cmap *d_map, int a_size, int delete_batch_size){


    int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid < delete_batch_size * a_size){

        int index_of_element = tid % delete_batch_size;
        int index_of_location = tid / delete_batch_size;

        if(d_delete_input[index_of_element].key == d_map[index_of_location].key){

            d_map[index_of_location].key = 0;
            d_map[index_of_location].value = 0;
            d_map[index_of_location].fill = 0;
        }

    }

}



int main(){

    int map_size = 100000002;
    int map_element_counter = 0;

    cmap *d_map, *h_map;

    h_map = (cmap*)malloc(map_size*sizeof(cmap));
    memset(h_map, 0, map_size*sizeof(cmap));

    cudaMalloc(&d_map, map_size*sizeof(cmap));
    cudaMemset(d_map, 0, map_size*sizeof(cmap));

    struct timeval t1, t2;
    double seconds, microSeconds;

    int insert=1, search=0, delet=0,search_with_pair=0, search_with_key=1, insert_again = 0;

    
    if(insert == 1){       
        
        int insert_batch_size = 100000000;

        element_pair *d_input, *h_input;
        
        h_input = (element_pair*)malloc(insert_batch_size * sizeof(element_pair));

        
        for(int i=0 ; i<insert_batch_size ; i++){
            h_input[i].key = 1;
            h_input[i].value = i+1;
            h_input[i].find = 0;
        }
        
        gettimeofday(&t1, NULL);

        map_element_counter += insert_batch_size;

        if(map_element_counter < map_size)
        {

            cudaMalloc(&d_input, insert_batch_size*sizeof(element_pair));
            cudaMemcpy(d_input, h_input, insert_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);

            int block = ceil((float)(map_size*insert_batch_size)/(float)block_size);

            search_kernel_pair<<<block, block_size>>>(d_input, d_map, map_size, insert_batch_size);



            int *d_index_to_fill_in_hash_table, *counter;

            cudaMalloc(&d_index_to_fill_in_hash_table, map_size*sizeof(int));
            cudaMemset(d_index_to_fill_in_hash_table, 0, map_size*sizeof(int));

            cudaMalloc(&counter, sizeof(int));
            cudaMemset(counter, 0, sizeof(int));

            block = ceil((float)(map_size)/(float)block_size);

            kernel_index_to_fill<<<block, block_size>>>(d_map, d_index_to_fill_in_hash_table, counter, map_size);
            
            cudaDeviceSynchronize();

            block = ceil((float)(insert_batch_size)/(float)block_size);

            kernel_to_insert<<<block, block_size>>>(d_map, d_input, d_index_to_fill_in_hash_table, insert_batch_size);

            cudaDeviceSynchronize();

            cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

            gettimeofday(&t2, NULL);
            seconds = t2.tv_sec - t1.tv_sec;
            microSeconds = t2.tv_usec - t1.tv_usec;
            printf("Time taken (ms): %.3f\n", 1000*seconds + microSeconds/1000);


            int count = 0;
            for(int i=0 ; i<map_size ; i++){
                if(h_map[i].fill==1){
                  //cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
                  count++;
                }
            }

            cout<<"Total Value1 = "<<count<<endl;
            cout<<endl;

        }else
        {
            ;
        }
        
    }

    if(insert_again == 1){

        int insert_batch_size = 5;
        
        
        element_pair *d_input, *h_input;

        h_input = (element_pair*)malloc(insert_batch_size * sizeof(element_pair));

        for(int i=0 ; i<insert_batch_size ; i++){
            h_input[i].key = i;
            h_input[i].value = i;
            h_input[i].find = 0;
        }

        cudaMalloc(&d_input, insert_batch_size*sizeof(element_pair));
        cudaMemcpy(d_input, h_input, insert_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);

        
        


        int block = ceil((float)(map_size*insert_batch_size)/(float)block_size);

        search_kernel_pair<<<block, block_size>>>(d_input, d_map, map_size, insert_batch_size);

        cudaDeviceSynchronize();
        int *d_index_to_fill_in_hash_table, *counter;

        cudaMalloc(&d_index_to_fill_in_hash_table, map_size*sizeof(int));
        cudaMemset(d_index_to_fill_in_hash_table, 0, map_size*sizeof(int));

        cudaMalloc(&counter, sizeof(int));
        cudaMemset(counter, 0, sizeof(int));

        block = ceil((float)(map_size)/(float)block_size);

        kernel_index_to_fill<<<block, block_size>>>(d_map, d_index_to_fill_in_hash_table, counter, map_size);
        
        cudaDeviceSynchronize();

        block = ceil((float)(insert_batch_size)/(float)block_size);

        kernel_to_insert<<<block, block_size>>>(d_map, d_input, d_index_to_fill_in_hash_table, insert_batch_size);

        cudaDeviceSynchronize();

        cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

        gettimeofday(&t2, NULL);
        seconds = t2.tv_sec - t1.tv_sec;
        microSeconds = t2.tv_usec - t1.tv_usec;
        printf("Time taken (ms): %.3f\n", 1000*seconds + microSeconds/1000);


        int count = 0;
        for(int i=0 ; i<map_size ; i++){
            if(h_map[i].fill==1){
             // cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
              count++;
            }
        }

        cout<<"Total Value1 = "<<count<<endl;
        cout<<endl;


        

    

    }



     // Search Code Data
    if(search==1){

        if(search_with_pair){

            element_pair *h_search_input_pair;
            element_pair *d_search_input_pair;

            //key, value to search

            int search_batch_size = 10000;
            

            h_search_input_pair = (element_pair*) malloc(search_batch_size * sizeof(element_pair));

            memset(h_search_input_pair, 0, search_batch_size*sizeof(element_pair));

            for(int i=0 ; i<search_batch_size ; i++){
                h_search_input_pair[i].key = i;
                h_search_input_pair[i].value = i+1;
            }

            gettimeofday(&t1, NULL);

            cudaMalloc(&d_search_input_pair, search_batch_size*sizeof(element_pair));
            cudaMemcpy(d_search_input_pair, h_search_input_pair, search_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);

                                      //existing array Size * batch_size
            int sblock = ceil((float)(map_size*search_batch_size)/(float)block_size);

            search_kernel_pair<<<sblock, block_size>>>(d_search_input_pair, d_map, map_size, search_batch_size);

            cudaMemcpy(h_search_input_pair, d_search_input_pair, search_batch_size*sizeof(element_pair), cudaMemcpyDeviceToHost);

            gettimeofday(&t2, NULL);
            seconds = t2.tv_sec - t1.tv_sec;
            microSeconds = t2.tv_usec - t1.tv_usec;
            printf("Time taken search pair(ms): %.3f\n", 1000*seconds + microSeconds/1000);

            int flag = 1;

            cout<<"\n\n\nFound value : \n";
            for(int i=0 ; i<search_batch_size ; i++){
                if(h_search_input_pair[i].find == 1 && flag){
                  cout<<"key = "<<h_search_input_pair[i].key<<" value = "<<h_search_input_pair[i].value<<" ";
                  flag=0;
                }
            }

            cout<<endl;
        }

      if(search_with_key){

            element *h_search_input_key;
            element *d_search_input_key;

            element_pair *h_search_input_key_ans;
            element_pair *d_search_input_key_ans;


            int *d_count, h_count, *d_index;


            cudaMalloc(&d_count, sizeof(int));
            cudaMemset(d_count, 0, sizeof(int));

            cudaMalloc(&d_index, sizeof(int));
            cudaMemset(d_index, 0, sizeof(int));

            //only key to search

            int search_batch_size = 3;
            

            h_search_input_key = (element*) malloc(search_batch_size * sizeof(element));

            memset(h_search_input_key, 0, search_batch_size*sizeof(element));

            for(int i=0 ; i<search_batch_size ; i++){
                h_search_input_key[i].key = 1;
            }

            gettimeofday(&t1, NULL);

            cudaMalloc(&d_search_input_key, search_batch_size*sizeof(element));
            cudaMemcpy(d_search_input_key, h_search_input_key, search_batch_size*sizeof(element), cudaMemcpyHostToDevice);


                                      //existing array Size * batch_size
            int sblock = ceil((float)(map_size*search_batch_size)/(float)block_size);

            search_kernel_key<<<sblock, block_size>>>(d_search_input_key, d_map, map_size, search_batch_size, d_count);

            cudaMemcpy(&h_count, d_count, sizeof(int), cudaMemcpyDeviceToHost);

            cout<<"\nFound Entry with Keys = "<<h_count<<endl;

            h_search_input_key_ans = (element_pair*)malloc(h_count * sizeof(element_pair));
            memset(h_search_input_key_ans, 0, h_count*sizeof(element_pair));


            cudaMalloc(&d_search_input_key_ans, h_count*sizeof(element_pair));
            cudaMemset(d_search_input_key_ans, 0, h_count*sizeof(element_pair));


                                      //existing array Size * batch_size
            sblock = ceil((float)(map_size*search_batch_size)/(float)block_size);

            fill_search_kernel_key<<<sblock, block_size>>>(d_search_input_key, d_map, map_size, search_batch_size, d_search_input_key_ans, d_index);



            cudaMemcpy(h_search_input_key_ans, d_search_input_key_ans, h_count*sizeof(element_pair), cudaMemcpyDeviceToHost);

            gettimeofday(&t2, NULL);
            seconds = t2.tv_sec - t1.tv_sec;
            microSeconds = t2.tv_usec - t1.tv_usec;
            printf("Time taken search keys(ms): %.3f\n", 1000*seconds + microSeconds/1000);



            int flag = 1;

            cout<<"\n\n\nFound value with keys : \n";
            for(int i=0 ; i<h_count ; i++){
                if(flag){
                  cout<<"key = "<<h_search_input_key_ans[i].key<<" value = "<<h_search_input_key_ans[i].value<<" ";
                  flag=0;
                }
            }

            cout<<endl;


      }

    }
    // Search Code ended

    // Delete Code
    if(delet == 1){

        //Delete Element with Key value pair
        int delete_with_pair = 0;
        if(delete_with_pair){

            element_pair *h_delete_input_pair;
            element_pair *d_delete_input_pair;

            int delete_batch_size = 3;
            

            h_delete_input_pair = (element_pair*) malloc(delete_batch_size * sizeof(element_pair));

            memset(h_delete_input_pair, 0, delete_batch_size*sizeof(element_pair));

            for(int i=0 ; i<delete_batch_size ; i++){
                h_delete_input_pair[i].key = 1;
                h_delete_input_pair[i].value = i+1;
            }

            gettimeofday(&t1, NULL);

            cudaMalloc(&d_delete_input_pair, delete_batch_size*sizeof(element_pair));
            cudaMemcpy(d_delete_input_pair, h_delete_input_pair, delete_batch_size*sizeof(element_pair), cudaMemcpyHostToDevice);


                                      //existing array Size * batch_size
            int sblock = ceil((float)(map_size*delete_batch_size)/(float)block_size);

            delete_kernel_pair<<<sblock, block_size>>>(d_delete_input_pair, d_map, map_size, delete_batch_size);

            cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

            gettimeofday(&t2, NULL);
            seconds = t2.tv_sec - t1.tv_sec;
            microSeconds = t2.tv_usec - t1.tv_usec;
            printf("Time taken Delate pair (ms): %.3f\n", 1000*seconds + microSeconds/1000);

            cout<<"\n\n After Delettion:\n\n";
            cout<<endl;
            for(int i=0 ; i<map_size ; i++){
                if(h_map[i].fill==1)
                  cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
                //cout<<h_map[i].value<<" ";
            }
        }else
        {

            element *h_delete_input;
            element *d_delete_input;

            int delete_batch_size = 1;
            

            h_delete_input = (element*) malloc(delete_batch_size * sizeof(element));

            memset(h_delete_input, 0, delete_batch_size*sizeof(element));

            for(int i=0 ; i<delete_batch_size ; i++){
                h_delete_input[i].key = 1;
            }

            cudaMalloc(&d_delete_input, delete_batch_size*sizeof(element));
            cudaMemcpy(d_delete_input, h_delete_input, delete_batch_size*sizeof(element), cudaMemcpyHostToDevice);

                                      //existing array Size * batch_size
            int sblock = ceil((float)(map_size*delete_batch_size)/(float)block_size);

            delete_kernel<<<sblock, block_size>>>(d_delete_input, d_map, map_size, delete_batch_size);

            cudaMemcpy(h_map, d_map, map_size * sizeof(cmap), cudaMemcpyDeviceToHost);

            gettimeofday(&t2, NULL);
            seconds = t2.tv_sec - t1.tv_sec;
            microSeconds = t2.tv_usec - t1.tv_usec;
            printf("Time taken Delate key (ms): %.3f\n", 1000*seconds + microSeconds/1000);

            cout<<"\n\n After Delettion:\n\n";
            cout<<endl;
            for(int i=0 ; i<map_size ; i++){
                if(h_map[i].fill==1)
                  cout<<"key = "<<h_map[i].key<<" "<<h_map[i].value<<"\n";
                //cout<<h_map[i].value<<" ";
            }


        }
    }
    // Delete Code Ended




    return 0;
}
