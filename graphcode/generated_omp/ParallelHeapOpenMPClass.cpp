    #include <cstdio>        // Added for printf() function 
    #include <sys/time.h>    // Added to get time of day
    #include <omp.h>
    #include <fstream>
    #include <iostream>
    #include <algorithm>
    #include <climits>
    #include "ParallelSortingOpenMP.cpp"

    using namespace std;
    //total size of the heap
    #define maxSize 1000

    int getRandom(int lower, int upper)
    {
        int num = (rand() % (upper - lower + 1)) + lower;
        return num;  
    }

    void printArray(vector <int> &arr,int size)
    {
        for(int i = 0;i<size;i++)
            printf("%d, ",arr[i]);

        cout << endl;
    }

    void FillArray(vector <int> &elements,int size)
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

    class Heap{
        private:
        vector <int> heap,heap_val;
        int curSize,isType;
        bool isSorted;
        //int *heap,*heap_val,curSize;
        ParallelSort ob;
        void serFillHole(int index){
            for(int i = index+1;i<curSize-1;i++){
                heap[i] = heap[i+1];
            }
            curSize--;
        }

        void fillHole(int index){
            //if the min element is present at the last position
            if(index == curSize-1){
                curSize--;
                return;
            }

            int nElements = curSize-index;
            int nthreads;

            #pragma omp parallel
            {
                nthreads = omp_get_num_threads();
            }
        
            if(nElements < 10*nthreads){
                serFillHole(index);
                return;
            }

            cout << "nthreads  = "<<nthreads<<endl;
            int elementsPerThread = ceil((float)nElements/nthreads);

            #pragma omp parallel
            {
                int threadId = omp_get_thread_num();
                int startInd = (threadId * elementsPerThread) + index;
                int endInd = min(((threadId+1)*elementsPerThread) + index - 1,curSize);

                int saveFirstVal = heap[startInd];

                #pragma omp barrier

                for(int i = startInd;i<endInd-1;i++){
                    heap[i] = heap[i+1];
                }

                #pragma omp barrier

                if(threadId > 0)
                    heap[startInd-1] = saveFirstVal; 
            }

            curSize--;
        }

        void serfindMin(){

            int finalmin = INT_MAX,finalpos = -1;
           
            for(int i = 0; i < curSize; ++i)
            {
                if(heap[i] < finalmin)
                {
                    finalmin = heap[i];
                    finalpos = i;
                }  
            }

            serFillHole(finalpos);
            cout << "minelem is " << finalmin << " at " << finalpos << " index." << endl;    
        }

        void findMin(){

            int minelem = INT_MAX, minpos = -1, finalmin = INT_MAX,finalpos = -1;
           
            #pragma omp parallel firstprivate(minelem,minpos)
            {
                #pragma omp for 
                for(int i = 0; i < curSize; ++i)
                {
                    if(heap[i] < minelem)
                    {
                        minelem = heap[i];
                        minpos = i;
                    }  
                }

                #pragma omp critical
                {
                    if(minelem < finalmin){
                        finalmin = minelem;
                        finalpos = minpos;
                    }
                }
            }
           
            fillHole(finalpos);
            cout << "minelem is " << finalmin << " at " << finalpos << " index." << endl;       
        }

        public:
        Heap(){
            srand(time(0));
            heap.resize(maxSize);
            heap_val.resize(maxSize);
            // heap = (int*) malloc(maxSize * sizeof(int));
            // heap_val = (int*) malloc(maxSize * sizeof(int));
            curSize = 0;
            isSorted = true;
            isType = 0;
        }

        int getSize(){
            return curSize;
        }

        void print()
        {
            for(int i = 0;i<curSize;i++)
                printf("%d = %d, \n",heap[i],heap_val[i]);

            cout << endl;
        }

        //Insert If only key is there
        void insertE(vector <int> &elements,int size){
            isType = 1;
            isSorted = false;
            #pragma omp parallel for
            for(int i = 0;i<size;i++)
            {
                heap[i + curSize] = elements[i];
            }
            curSize = curSize + size;
        }

        //Insert if both key and values are there
        void insertE(vector <int> &elements,vector <int> &val,int size){
            isType = 2;
            isSorted = false;
            #pragma omp parallel for
            for(int i = 0;i<size;i++)
            {
                heap[i + curSize] = elements[i];
                heap_val[i + curSize] = val[i];
            }
            curSize = curSize + size;
        }

        vector<int> deleteE(){
            return deleteE(1);
        }

        vector<int> deleteE(int n){
            if(isSorted == false)
            {
                cout << "Inside Sorting"<<endl;
                if(isType == 2)
                    ob.par_q_sort_tasks(0,curSize-1,heap,heap_val);
                else if(isType == 1)
                    ob.par_q_sort_tasks(0,curSize-1,heap);
            }

            vector <int> ret_elements;

            if(isType == 2)
                ret_elements.resize(n*2);
            else if(isType == 1)
                ret_elements.resize(n);

            #pragma omp for 
            for(int i = 0;i<n;i++){
                ret_elements[i] = heap[curSize-i-1];
            }

            if(isType == 2)
            {
                #pragma omp for 
                for(int i = 0;i<n;i++){
                    ret_elements[curSize+i] = heap_val[curSize-i-1];
                }
            }

            isSorted = true;
            curSize -= n;
            return ret_elements;
        }

    };

    // int main() {
    //     Heap hp;

    //     for(int lk = 0;lk<1;lk++)
    //     {
    //         int elemSize = maxSize-2;
    //         // do{
    //         //     elemSize = getRandom(1,maxSize-hp.getSize());
    //         // }while(elemSize + hp.getSize() > maxSize);
            
    //         vector <int> elements(elemSize);
    //         FillArray(elements,elemSize);
    //         // printArray(elements,elemSize);  

    //         printf("No of Inserted Elements are = %d\n",elemSize);
    //         double starttime = rtclock(); 
    //         hp.insertE(elements,elements,elemSize);
    //         double endtime = rtclock(); 
    //         printtime("Insertion time: ", starttime, endtime); 
    //         // hp.print();

    //         starttime = rtclock();
    //         vector <int> elem = hp.deleteE(10);
    //         hp.print();

    //         for(int i = 0;i<10;i++)
    //             cout << elem[i] << ", ";
            
    //         cout << endl;

    //         endtime = rtclock();
    //         printtime("Sorting: ", starttime, endtime);
    //     }

    //     printf( "Over");
    //     return 0;
    // }
