    #include <cstdio>        // Added for printf() function 
    #include <sys/time.h>    // Added to get time of day
    #include <cuda.h>
    #include <fstream>
    #include <time.h>
    #include <iostream>
    #include <thrust/host_vector.h>
    #include <thrust/device_vector.h>
    #include <thrust/sort.h>
    #include <thrust/iterator/reverse_iterator.h>

    using namespace std;
    //total size of the heap
    #define maxSize 100000000

    // __global__ void delete_Elem(int *heap,int *d_elements,int *curSize,int *elemSize,int k){
       
    // }

    int getRandom(int lower, int upper)
    {
        int num = (rand() % (upper - lower + 1)) + lower;
        return num;  
    }

    void printArray(thrust::host_vector<int> &arr,int size)
    {
        for(int i = 0;i<size;i++)
            printf("%d, ",arr[i]);
    }

    void FillArray(thrust::host_vector<int> &elements,int size)
    {
        for(int i = 0;i<size;i++)
        {
            elements[i] = getRandom(1,maxSize*10);
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

    //Insert If only key is there
    __global__ void Insert_Elem(int *heap,int *curSize,int *d_elements,int *elemSize)
    {
        int tid = blockIdx.x * blockDim.x + threadIdx.x;
        if(tid < *elemSize)
        {
            heap[tid + *curSize] = d_elements[tid];
        }
    }

    //Insert if both key and values are there
    __global__ void Insert_Elem(int *d_val, int *heap_val,int *heap,int *curSize,int *d_elements,int *elemSize)
    {
        int tid = blockIdx.x * blockDim.x + threadIdx.x;
        if(tid < *elemSize)
        {
            heap[tid + *curSize] = d_elements[tid];
            heap_val[tid + *curSize] = d_val[tid];
        }
    }

    class Heap{
        private:
        int *heap,*heap_val,*curSize;
        bool isSorted;
        int isType;
        public:
        Heap(){
            srand(time(0));
            // cout << "Constructor Called" << endl;
            cudaHostAlloc(&curSize, sizeof(int), 0);
            cudaMalloc(&heap,maxSize*sizeof(int));
            cudaMalloc(&heap_val,maxSize*sizeof(int));
            isSorted = false;
            isType = 0;
            // *curSize = 0;
        }

        int getSize(){
            return *curSize;
        }

        //Insert If only key is there
        void insert(thrust::host_vector<int> &elements,int size1){
            isType = 1;
            isSorted = false;
            thrust::device_vector<int> d_el = elements;
            int *elemSize;
            cudaMalloc(&elemSize,sizeof(int));
            cudaMemcpy(elemSize,&size1,sizeof(int),cudaMemcpyHostToDevice);
            
            int *d_elements = thrust::raw_pointer_cast(d_el.data());

            int block = ceil((float) size1/1024);
            Insert_Elem<<<block,1024>>>(heap,curSize,d_elements,elemSize);
            cudaDeviceSynchronize();
            *curSize = *curSize + size1; 
            // printArray(heap,*curSize);
        }

        //Insert if both key and values are there
        void insert(thrust::host_vector<int> &elements,thrust::host_vector<int> &val,int size1){
            isType = 2;
            isSorted = false;
            thrust::device_vector<int> d_el = elements;
            thrust::device_vector<int> d_v = val;
            int *elemSize;
            
            int *d_elements = thrust::raw_pointer_cast(d_el.data());
            int *d_val = thrust::raw_pointer_cast(d_v.data());
            cudaMalloc(&elemSize,sizeof(int));
            cudaMemcpy(elemSize,&size1,sizeof(int),cudaMemcpyHostToDevice);
            
            int block = ceil((float) size1/1024);
            Insert_Elem<<<block,1024>>>(d_val,heap_val,heap,curSize,d_elements,elemSize);
            cudaDeviceSynchronize();
            *curSize = *curSize + size1; 
        }


        thrust::host_vector<int>  deleteElem(){
            return deleteElem(1);
        }

        // 1 2 3 4 5 6 7 8 9 10
        thrust::host_vector<int>  deleteElem(int n){
            
            //wrap raw pointer with a device_ptr
            thrust::device_ptr<int> d_vec(heap);
            thrust::device_ptr<int> d_values(heap_val);

            //use device_ptr in thrust algorithms
            if(isSorted == false)
            {
                if(isType == 2)
                    thrust::sort_by_key(d_vec, d_vec+*curSize,d_values,thrust::greater<int>());
                else if(isType == 1)
                    thrust::sort(d_vec, d_vec+*curSize,thrust::greater<int>());

            }
            
            isSorted = true;

            cout << endl<<"Array after sorting"<<endl;
            for(int i = 0;i<*curSize;i++){
                cout << d_vec[i] << "->"<< d_values[i] << " ; ";
            }
            cout << endl;

            thrust::host_vector<int> ret_elements(n);
            if(isType == 2)
                ret_elements.resize(2*n);

            typedef thrust::device_vector<int>::iterator Iterator; 
            thrust::reverse_iterator<Iterator> r_iter = thrust::make_reverse_iterator(d_vec + *curSize); // note that we point the iterator to the "end" of the device pointer area
            // thrust::copy(d_vec + (*curSize - n), d_vec + *curSize, ret_elements.begin());
            thrust::copy_n(r_iter,n,ret_elements.begin());
            
            if(isType == 2)
            {
                r_iter = thrust::make_reverse_iterator(d_values + *curSize);
                thrust::copy_n(r_iter,n,ret_elements.begin()+n);
            }

            *curSize -= n;
            return ret_elements;
        }

    };
    

    /*int main() {
        
        Heap hp;

        for(int lk = 0;lk<1;lk++)
        {
            int elemSize = maxSize-5;
            // do{
            //     elemSize = getRandom(1,maxSize-hp.getSize());
            // }while(elemSize + hp.getSize() > maxSize);
            
            thrust::host_vector<int> elements(elemSize);
            thrust::host_vector<int> val(elemSize);
            FillArray(elements,elemSize);
            FillArray(val,elemSize);
            printArray(elements,elemSize);
            printArray(val,elemSize);
            printf("No of Inserted Elements are = %d\n",elemSize);
            double starttime = rtclock(); 
            hp.insert(elements,elemSize);
            double endtime = rtclock(); 
            printtime("Insertion time: ", starttime, endtime); 

            starttime = rtclock();
            thrust::host_vector<int> res = hp.deleteElem(3);
            endtime = rtclock();
            printtime("Sorting: ", starttime, endtime);
            for(int i = 0;i<6;i++)
                cout << res[i] <<", ";
        }

        printf( " Over ");
        return 0;
    }*/
