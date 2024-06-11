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
    #define maxSize 100000000

    int getRandom(int lower, int upper)
    {
        int num = (rand() % (upper - lower + 1)) + lower;
        return num;  
    }

    void printArray(int arr[],int size)
    {
        for(int i = 0;i<size;i++)
            printf("%d, ",arr[i]);

        cout << endl;
    }

    void FillArray(int elements[],int size)
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
        int *heap,*heap_val,curSize;
        ParallelSort ob;
        public:
        Heap(){
            heap = (int*) malloc(maxSize * sizeof(int));
            heap_val = (int*) malloc(maxSize * sizeof(int));
            curSize = 0;
        }

        int getSize(){
            return curSize;
        }

        void print()
        {
            for(int i = 0;i<curSize;i++)
                printf("%d, ",heap[i]);

            cout << endl;
        }

        //Insert If only key is there
        void insert(int *elements,int size){

            #pragma omp parallel for
            for(int i = 0;i<size;i++)
            {
                heap[i + curSize] = elements[i];
            }
            curSize = curSize + size;
	    heap[0] = -1; 
        }

        //Insert if both key and values are there
        void insert(int *elements,int *val,int size){

            #pragma omp parallel for
            for(int i = 0;i<size;i++)
            {
                heap[i + curSize] = elements[i];
                heap_val[i + curSize] = val[i];
            }
            curSize = curSize + size;
        }

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

        void deleteElem(){
             ob.par_q_sort_tasks(0,curSize-1,heap);
        }

    };

    int main() {
        srand(time(0));
        Heap hp;

        for(int lk = 0;lk<1;lk++)
        {
            int elemSize = maxSize-2;
            // do{
            //     elemSize = getRandom(1,maxSize-hp.getSize());
            // }while(elemSize + hp.getSize() > maxSize);
            
            int elements[elemSize];
            FillArray(elements,elemSize);
            // printArray(elements,elemSize);  

            printf("No of Inserted Elements are = %d\n",elemSize);
            double starttime = rtclock(); 
            hp.insert(elements,elemSize);
            double endtime = rtclock(); 
            printtime("Insertion time: ", starttime, endtime); 
            // hp.print();

            starttime = rtclock();
            hp.serfindMin();
            endtime = rtclock();
            printtime("Serial Min: ", starttime, endtime);
            // hp.print();

            starttime = rtclock();
            hp.findMin();
            endtime = rtclock();
            printtime("Parallel Min: ", starttime, endtime);
            // hp.print();

            starttime = rtclock();
            hp.deleteElem();
            endtime = rtclock();
            printtime("Sorting: ", starttime, endtime);
            // hp.print();
        }

        printf( "Over");
        return 0;
    }
