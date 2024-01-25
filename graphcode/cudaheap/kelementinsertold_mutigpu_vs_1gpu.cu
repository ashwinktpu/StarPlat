    #include <cstdio>        // Added for printf() function 
    #include <sys/time.h>    // Added to get time of day
    #include <cuda.h>
    #include <fstream>
    #include <time.h>
    #include <iostream>

    //total size of the heap
    #define maxSize 100000000

    __global__ void Insert_Elem(volatile int *heap,int *d_elements,int *curSize,volatile int *lockArr,int *elemSize,int k){
        int tid = blockIdx.x * blockDim.x + threadIdx.x;
        if(tid < *elemSize)
        {
            int childInd = atomicInc((unsigned *) curSize,maxSize);
            childInd = childInd*k;
            for(int i = 0;i<k;i++)
                heap[childInd+i] = d_elements[tid*k+i];

            int parInd = ((childInd/k - 1)/2) * k;

            if(childInd == 0){
                lockArr[childInd] = 0;
            }

            if(childInd != 0)
            {
                int oldval = 1;
                do
                {
                    oldval = atomicCAS((int*)&lockArr[parInd/k],0,1);
                    if(oldval == 0) //if we got the lock on parent
                    {
                        if(heap[parInd] > heap[childInd])
                        {
                            for(int i = 0;i<k;i++){
                                int temp = heap[parInd+i];    //swapping the elements
                                heap[parInd+i] = heap[childInd+i];
                                heap[childInd+i] = temp;
                            // __threadfence();//necessary
                            }
                            
                            __threadfence();

                            lockArr[childInd/k] = 0; //unlock the child
            
                            childInd = parInd;
                            parInd = ((childInd/k - 1)/2) * k;
                            oldval = 1; //we need to heapify again

                            //if we have reached the root
                            if(childInd == 0){
                                oldval = 0; //we need not heapify again
                                lockArr[childInd/k] = 0;
                            }  
                        }
                        else //if heap property satisfied release the locks
                        {
                            lockArr[childInd/k] = 0;
                            lockArr[parInd/k] = 0;
                        } 
                        
                    }
                    //__threadfence(); //doesnt seem necessary
                }while(oldval != 0);
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

    __global__ void setLockVar(int *curSize,int *lockArr,int *elemSize)
    {
        int tid = blockIdx.x * blockDim.x + threadIdx.x;
        if(tid < *elemSize)
            lockArr[tid + *curSize] = 1;
    }

    __device__ void heapifyBUP(int arr[], int n, int childInd, int k) {
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

    __device__ void insertNode(int arr[],  int *n,int val,int k)
    {
        // Increase the size of Heap by 2
        *n = *n + 1;
        int childInd = *n * k;
    
        // Insert the element at end of Heap
        // arr[childInd - 2] = Key;
        arr[childInd - 1] = val;
    
        // Heapify the new node following a
        // Bottom-up approach
        heapifyBUP(arr, *n,childInd-k,k);
    }

    //(serHeap,*serSize,*elements,*elemSize);
    __global__ void insertNodeHelper(int *arr,int *size,int *elements,int *elemSize)
    {
        int k = 1;
        for(int i = 0;i<*elemSize;i++){
            insertNode(arr,size,elements[i],k);
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

    int main() {
        srand(time(0));
        int countvalid = 0;
        int newValid = 0;
        int inivalid = 0;
        int k = 1;
        
        for(int lk = 0;lk<10;lk++)
        {
            int *d_a;
            int *curSize;
            int *lockArr;
            int *elemSize;
            int *serHeap;
            int  *serSize;
            int newHeap[maxSize*k];

            cudaHostAlloc(&curSize, sizeof(int), 0);
            cudaHostAlloc(&elemSize, sizeof(int), 0);
            cudaHostAlloc(&serSize, sizeof(int), 0);

            int h_a[maxSize*k];
            *curSize = getRandom(1,maxSize/10);
            *serSize = *curSize;

            //Initialise Heap with some random values
            FillArray(h_a,*curSize,k);

        //heapify the heap
            buildHeap(h_a,*curSize,k);

        //check if satisfies the heap property
            if(checkHeap(h_a,*curSize,k)) inivalid++;

            cudaMalloc(&d_a,maxSize*sizeof(int)); 
            cudaMalloc(&serHeap,maxSize*sizeof(int)); 

            cudaMemcpy(d_a,h_a,maxSize * sizeof(int),cudaMemcpyHostToDevice);
            cudaMemcpy(serHeap,h_a,maxSize * sizeof(int),cudaMemcpyHostToDevice);

            *elemSize = getRandom(1,maxSize-*curSize-2);
            int elements[*elemSize*k];
            
            FillArray(elements,*elemSize,k);
            printf("%d. No of Inserted Elements are = %d\n",inivalid,*elemSize);

            int *d_elements;
            cudaMalloc(&d_elements,*elemSize*k*sizeof(int));
            cudaMemcpy(d_elements,elements,*elemSize * k* sizeof(int),cudaMemcpyHostToDevice);
            cudaMalloc(&lockArr,(*elemSize + *curSize)*sizeof(int));
            cudaMemset(lockArr,0,(*elemSize + *curSize)*sizeof(int));
        
            int block = ceil((float) *elemSize/1024);

            double starttime = rtclock(); 
            setLockVar<<<block,1024>>>(curSize,lockArr,elemSize);
            cudaDeviceSynchronize();
            Insert_Elem<<<block,1024>>>(d_a,d_elements,curSize,lockArr,elemSize,k);
            cudaDeviceSynchronize();
            double endtime = rtclock();  
            printtime("GPU Kernel time: ", starttime, endtime);

            starttime = rtclock();
            insertNodeHelper<<<1,1>>>(serHeap,serSize,d_elements,elemSize);
            cudaDeviceSynchronize();
            endtime = rtclock();
            printtime("GPU (1 thread time)Kernel time: ", starttime, endtime);
            
            cudaMemcpy(h_a,d_a,maxSize*k*sizeof(int),cudaMemcpyDeviceToHost);
            cudaMemcpy(newHeap,serHeap,maxSize*k*sizeof(int),cudaMemcpyDeviceToHost);
            if(checkHeap(h_a,*curSize,k)) {
                // printf("Valid\n");
                countvalid++;
            }

            if(checkHeap(newHeap,*serSize,k)) {
                // printf("Valid\n");
                newValid++;
            }
            // bool flag = 0;
            // for(int i = 0;i<*curSize;i++){
            //     if(h_a[i] != newHeap[i]){
            //         flag = 1;
            //         break;
            //     }
            // }

            // if(flag == 0)   newValid++;
            // else    printf("Not Equal\n");

            // printArray(h_a,*curSize,k);
            // printArray(newHeap,*serSize,k);
        }
        printf("\nInitial valid : %d",inivalid);
        printf("\nSingle Thread : %d",newValid);
        printf("\nMulti Thread  : %d",countvalid);
        return 0;
    }
