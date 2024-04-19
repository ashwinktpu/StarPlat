#include <cstdio>        // Added for printf() function 
#include <sys/time.h>    // Added to get time of day
#include <omp.h>
#include <fstream>
#include <time.h>
#include <iostream>

//total size of the heap
#define maxSize 2000000
omp_lock_t lock[maxSize];

void kernel(int heap[],int elements[],int elemSize,int &curSize,int k){
    int count = 0;int ind,childInd;

    if(curSize == 0){
        for(int j = 0;j<k;j++)
            heap[0+j] = elements[(elemSize-1)*k+j];
        curSize++;
        elemSize--;
    }

    #pragma omp parallel for private(ind,childInd)
    for(int i = 0;i<elemSize;i++)
    {
        #pragma omp critical
        {
            ind = count++;
            childInd = curSize++;
            childInd = childInd*k;
        }

        omp_set_lock(&(lock[childInd/k]));            
        for(int j = 0;j<k;j++)
            heap[childInd+j] = elements[ind*k+j];   

        int parInd = ((childInd/k - 1)/2) * k;
        while (1)
        {   
            omp_set_lock(&(lock[parInd/k]));
            if(heap[parInd] > heap[childInd])
            {
                for(int j = 0;j<k;j++){
                    int temp = heap[parInd+j];    //swapping the elements
                    heap[parInd+j] = heap[childInd+j];
                    heap[childInd+j] = temp;
                }

                omp_unset_lock(&(lock[childInd/k]));
                childInd = parInd;
                parInd = ((childInd/k - 1)/2) * k;

                if(childInd == 0){
                    omp_unset_lock(&(lock[childInd/k]));
                    break;
                }  
            }
            else{
                omp_unset_lock(&(lock[childInd/k]));
                omp_unset_lock(&(lock[parInd/k]));
                break;
            }
        }
    }
}

bool checkHeap(int *ar,int size,int k)
{
    for(int i = 0;i<size/2;i+=k)
    {
        if(ar[i] > ar[2*i + k]){
            printf("\nproblem found at index parent = %d,child = %d\n",i,2*i + k);
            printf("problem found at index parentval = %d,childval = %d\n",ar[i],ar[2*i + k]); 
            return false;
        } 
        if((2*i + 2) < size && ar[i] > ar[2*i + 2*k]){
            printf("\nproblem found at index parent = %d,child = %d\n",i,2*i + 2*k);
            printf("problem found at index parentval = %d,childval = %d\n",ar[i],ar[2*i + 2*k]);
            return false;
        }
    }
    return true;
}
int getRandom(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;  
}
void printArray(int arr[],int size,int k)
{
    printf("\n");
    for(int i = 0;i<size;i++)
        printf("%d, ",arr[i]);
    
    printf("\n");
}
void FillArray(int elements[],int size,int k)
{
    for(int i = 0;i<size*k;i++)
    {
        elements[i] = getRandom(1,1000);
    }
}
    
void heapify(int hp[],int ind,int size,int k)
{
    while(1)
    {
        int leftChild = 2*ind+k;
        int rightChild = 2*ind+2*k;
        int largeInd = -1;
        if(rightChild < size*k && hp[ind] > hp[rightChild]){
            if(hp[leftChild] < hp[rightChild])
                largeInd = leftChild;
            else
                largeInd = rightChild;
        }
        else if(leftChild < size*k && hp[ind] > hp[leftChild]){
            largeInd = leftChild;
        }
        
        if(largeInd == -1)  return;
	
        

	for(int i = 0;i<k;i++){
		int temp = hp[ind+i];
        	hp[ind+i] = hp[largeInd+i];
        	hp[largeInd+i] = temp;
	}

	ind = largeInd;
        
    }

}

void buildHeap(int hp[],int n,int k)
{
    for(int i = n/2 -1 ; i>=0;i--)
    {
        heapify(hp,i*k,n,k);
    }
}

void heapifyBUP(int arr[], int n, int childInd, int k) {
    // Find parent 
    int parInd = ((childInd/k - 1)/2) * k;
    if (parInd >= 0) { 
        if (arr[childInd] < arr[parInd]) { 
            for(int i = 0;i<k;i++){
                int temp = arr[parInd+i];
                arr[parInd+i] = arr[childInd+i];
                arr[childInd+i] = temp;
            }
            heapifyBUP(arr, n, parInd,k); 
        } 
    } 
}

void insertNode(int arr[],  int& n,int val,int k)
{
    // Increase the size of Heap by 2
    n = n + 1;
    int childInd = n*k;
 
    // Insert the element at end of Heap
    // arr[childInd - 2] = Key;
    arr[childInd - 1] = val;
 
    // Heapify the new node following a
    // Bottom-up approach
    heapifyBUP(arr, n,childInd-k,k);
}


double rtclock(){
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday(&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d", stat);
    return(Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

void printtime(const char *str, double starttime, double endtime){
    printf("%s%3f seconds\n", str, endtime - starttime);
}

int main() {

    omp_set_num_threads(8);
   // int num = omp_get_num_threads();
    #pragma omp parallel
    printf("Hello World\n");

    for(int i = 0;i<maxSize;i++)
        omp_init_lock(&(lock[i]));

    srand(time(0));
    int countvalid = 0;
    int inivalid = 0;
    int servalid = 0;
    int k = 1;
    
    for(int lk = 0;lk<10;lk++)
    {
        int heap[maxSize*k];
        int serHeap[maxSize*k];
        int curSize;
        int elemSize;
        
        curSize = getRandom(1,maxSize/10);
        int serSize = curSize;

        //Initialise Heap with some random values
        FillArray(heap,curSize,k);

       //heapify the heap
        buildHeap(heap,curSize,k);

        for(int i = 0;i<curSize;i++)   serHeap[i] = heap[i];

       //check if satisfies the heap property
        if(checkHeap(heap,curSize,k)) inivalid++;

        elemSize = getRandom(1,maxSize-curSize-2);
        int elements[elemSize*k];
        
        FillArray(elements,elemSize,k);
        printf("%d. No of Inserted Elements are = %d\n",inivalid,elemSize);

        double starttime = rtclock(); 
        kernel(heap,elements,elemSize,curSize,k);
        double endtime = rtclock();  
        printtime("GPU Kernel time: ", starttime, endtime);

        starttime = rtclock();
        for(int i = 0;i<elemSize;i++){
            insertNode(serHeap,serSize,elements[i],k);
        }
        endtime = rtclock();
        printtime("GPU Kernel time: ", starttime, endtime);
        
        if(checkHeap(heap,curSize,k)) {
            // printf("Valid\n");
            countvalid++;
        }
        if(checkHeap(serHeap,serSize,k)) {
            // printf("Valid\n");
            servalid++;
        }
        printf("\nInitial valid : %d",inivalid);
    printf("\nvalid : %d\n\n",countvalid);
    printf("\nvalid : %d\n\n",servalid);
    }
    

    for(int i = 0;i<maxSize;i++)
            omp_destroy_lock(&(lock[i]));
    return 0;
}
