#include <stdio.h>        // Added for printf() function 
#include <sys/time.h>    // Added to get time of day
#include <omp.h>
#include <time.h>
#include <cstdlib>
#include <iostream>

using namespace std;
//total size of the heap
#define maxSize 10000
omp_lock_t lock[maxSize];
omp_lock_t lock1;

bool checkHeap(int *ar,int size)
{
    for(int i = 0;i<size/2;i++)
    {
        if(ar[i] > ar[2*i + 1]){
            printf("\nproblem found at index parent = %d,child = %d\n",i,2*i + 1);
            printf("problem found at index parentval = %d,childval = %d\n",ar[i],ar[2*i + 1]); 
            return false;
        } 
        if((2*i + 2) < size && ar[i] > ar[2*i + 2]){
            printf("\nproblem found at index parent = %d,child = %d\n",i,2*i + 2);
            printf("problem found at index parentval = %d,childval = %d\n",ar[i],ar[2*i + 2]);
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
void printArray(int arr[],int size)
{
    for(int i = 0;i<size;i++)
        printf("%d, ",arr[i]);
}
void FillArray(int elements[],int size)
{
    for(int i = 0;i<size;i++)
    {
        elements[i] = getRandom(1,maxSize*10);
    }
}
    
void heapify(int hp[],int ind,int size)
{
    while(1)
    {
        int leftChild = 2*ind+1;
        int rightChild = 2*ind+2;
        int largeInd = -1;
        if(rightChild < size && hp[ind] > hp[rightChild]){
            if(hp[leftChild] < hp[rightChild])
                largeInd = leftChild;
            else
                largeInd = rightChild;
        }
        else if(leftChild < size && hp[ind] > hp[leftChild]){
            largeInd = leftChild;
        }
        
        if(largeInd == -1)  return;
        int temp = hp[ind];
        hp[ind] = hp[largeInd];
        hp[largeInd] = temp;
        ind = largeInd;
    }

}

void buildHeap(int hp[],int n)
{
    for(int i = n/2 -1 ; i>=0;i--)
    {
        heapify(hp,i,n);
    }
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

void merge(int array[], int const left, int const mid,
           int const right)
{
    int const subArrayOne = mid - left + 1;
    int const subArrayTwo = right - mid;
 
    // Create temp arrays
    auto *leftArray = new int[subArrayOne],
         *rightArray = new int[subArrayTwo];
 
    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = array[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = array[mid + 1 + j];
 
    auto indexOfSubArrayOne = 0, indexOfSubArrayTwo = 0;
    int indexOfMergedArray = left;
 
    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne
           && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne]
            <= rightArray[indexOfSubArrayTwo]) {
            array[indexOfMergedArray]
                = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else {
            array[indexOfMergedArray]
                = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
 
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        array[indexOfMergedArray]
            = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
 
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        array[indexOfMergedArray]
            = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}
 
void mergeSort(int array[], int const begin, int const end)
{
    if (begin >= end)
        return;
 
    int mid = begin + (end - begin) / 2;
    mergeSort(array, begin, mid);
    mergeSort(array, mid + 1, end);
    merge(array, begin, mid, end);
}
bool compareVal(int arr1[],int n1,int arr2[],int n2)
{
    mergeSort(arr2,0,n2-1);
    for(int i = 0;i<n1;i++)
    {
        if(arr1[i] != arr2[i])  return false;
    }
    return true;
}

int main() {
    srand(time(0));
    int countvalid = 0;
    int inivalid = 0;
    for(int i = 0;i<maxSize;i++)
        omp_init_lock(&(lock[i]));
    
    omp_init_lock(&lock1);
    double starttime = rtclock();
    for(int lk = 0;lk<100;lk++)
    {
        int curSize;
        int elemSize;
        int newSize;

        int heap[maxSize];
        int oldheap[maxSize];
        curSize = getRandom(15,maxSize);
        int saveSize = curSize;

        //Initialise Heap with some random values
        FillArray(heap,curSize);

       //heapify the heap
        buildHeap(heap,curSize);

        for(int i = 0;i<curSize;i++)    oldheap[i] = heap[i];

        // printf("%d. Initially the array is ",inivalid);
        // printArray(heap,curSize);
        // printf("\n");

       //check if satisfies the heap property
        if(checkHeap(heap,curSize)) inivalid++;

        elemSize = getRandom(1,curSize-14);
        int delElem[elemSize];
        printf("No of elements to be deleted = %d, ",elemSize);
        newSize = curSize - elemSize;

        
        int parInd = 0,childInd = 0,count = 0,globalInd = 0,largeInd = -1,leftChild = 0,rightChild = 0;
        // int globalInd = 0;
        // int flag = 0;
        #pragma omp parallel for private(parInd,childInd,count,largeInd,leftChild,rightChild)
        for(int i = 0;i<elemSize;i++)
        {
            count = 0,largeInd = -1,leftChild = 0,rightChild = 0,parInd = 0,childInd = 0;
            // int parInd = 0,childInd = 0,count = 0,largeInd = -1,leftChild = 0,rightChild = 0;
            omp_set_lock(&(lock[parInd]));
            // #pragma omp critical
            while (1)
            {   
                leftChild = 2*parInd + 1;
                rightChild = 2*parInd + 2;

                if(count == 0)
                {
                    // omp_set_lock(&lock1);
                    delElem[globalInd] = heap[parInd];
                    #pragma omp critical
                    {
                        childInd = --curSize;
                        heap[parInd] = heap[childInd];
                    }
                    // omp_set_lock(&(lock[childInd])); 
                    
                    // cout<<heap[parInd]<<endl;
                    globalInd++;
                    // printf("Root Value before del = %d,%d, ",heap[parInd],childInd);
                    
                    
                    // heap[childInd] = 1000000000;
                    // omp_unset_lock(&(lock[childInd]));
                    // printf("Root Value after del = %d\n",heap[parInd]);
                    // omp_unset_lock(&lock1);
                }

                largeInd = -1;
                #pragma omp flush
                if(leftChild < curSize)
                {   
                    // omp_set_lock(&lock1);
                    omp_set_lock(&(lock[leftChild]));
                    if(rightChild < curSize) omp_set_lock(&(lock[rightChild]));

                    if((rightChild < curSize) && (heap[parInd] > heap[rightChild]))
                    {
                        if(heap[leftChild] < heap[rightChild])
                        {
                            largeInd = leftChild;
                            omp_unset_lock(&(lock[rightChild]));
                        }
                        else
                        {
                            largeInd = rightChild;
                            omp_unset_lock(&(lock[leftChild]));
                        }
                    }
                    else if((leftChild < curSize) && (heap[parInd] > heap[leftChild])){
                        largeInd = leftChild;
                        omp_unset_lock(&(lock[rightChild]));
                    }
        
                    if(largeInd == -1) //that means no longer need to go down
                    {
                        if(rightChild < curSize) omp_unset_lock(&(lock[rightChild]));
                        if(leftChild < curSize) omp_unset_lock(&(lock[leftChild]));
                        omp_unset_lock(&(lock[parInd]));
                        break;
                    }
                    else
                    {
                        // printf("value before swap : %d,%d\n",heap[parInd],heap[largeInd]);
                        int temp = heap[largeInd];    //swapping the elements
                        heap[largeInd] = heap[parInd];
                        heap[parInd] = temp;

                        // printf("value after swap : %d,%d\n",heap[parInd],heap[largeInd]);
                        omp_unset_lock(&(lock[parInd]));
                        parInd = largeInd; 
                    }
                }
                else //if heap property satisfied release the locks
                {
                    if(rightChild < curSize) omp_unset_lock(&(lock[rightChild]));
                    if(leftChild < curSize) omp_unset_lock(&(lock[leftChild]));
                    omp_unset_lock(&(lock[parInd]));
                    break;
                } 
                count = count + 1;
                // omp_unset_lock(&lock1);
            }
        }

        
        

        // printf("Elements Deleted are ");
        // printArray(delElem,elemSize);
        // printf("\n\n");

        bool res = compareVal(delElem,elemSize,oldheap,saveSize);
        if(!res)    {
            cout<<"\nproblem found at "<<inivalid;
            printf("Elements Deleted are ");
            printArray(delElem,elemSize);
            printf("\n\n");
            printf("\nBefore Deletion the array is ");
            printArray(oldheap,saveSize);
            printf("\n\n");
            printf("\nAfter Deletion the array is ");
            printArray(heap,curSize);
            printf("\n\n");
        }
        // printf("\nAfter Deletion the array is ");
        // printArray(heap,curSize);
        // printf("\n\n");
        
        if(checkHeap(heap,curSize) && res) {
            // printf("Valid\n");
            countvalid++;
        }
        printf("\nInitial valid : %d",inivalid);
        printf("\nvalid : %d\n\n",countvalid);
    }
    double endtime = rtclock();
    printtime("Time: ", starttime, endtime);

    for(int i = 0;i<maxSize;i++)
        omp_destroy_lock(&(lock[i]));
    
    omp_destroy_lock(&lock1);
    
    
    return 0;
}