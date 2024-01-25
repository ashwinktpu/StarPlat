#include <stdio.h>        // Added for printf() function 
#include <sys/time.h>    // Added to get time of day
#include <omp.h>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include "../library/graph.hpp"

using namespace std;

//total size of the heap
#define maxSize 1000
omp_lock_t lock[maxSize];

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
    for(int i = 0;i<size;i++)
    {
        if(arr[i] == 10000000)
            printf("-1, ");
        else
            printf("%d, ",arr[i]);
    }
    
    cout<<endl;
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

void heapifyBUP(int arr[], int n, int childInd,int k) {
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

void insertNode(int arr[], int& n, int Key,int val,int k)
{
    // Increase the size of Heap by 2
    n = n + 1;
    int childInd = n*k;
 
    // Insert the element at end of Heap
    arr[childInd - 2] = Key;
    arr[childInd - 1] = val;
 
    // Heapify the new node following a
    // Bottom-up approach
    heapifyBUP(arr, n,childInd-k,k);
}

void deleteRoot(int arr[], int &n,int k)
{
    for(int i = 0;i<k;i++){
        arr[i] = arr[(n -1)*2 + i];
    }
 
    // Decrease size of heap by 1
    n = n - 1;
 
    // heapify the root node
    heapify(arr,0,n,k);
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

void serDijkstra(int pos[],int neigh[],int weight[],int wn,int src,int dist[],int V,int k,double &serdijtime)
{
    double starttime = rtclock();

    for(int i = 0;i<V;i++) {
        dist[i] = 10000000;
    }

    int hp[maxSize];
    int cursize = 0; 
    insertNode(hp, cursize,0,src,k);
    dist[src] = 0;
 

    while (cursize != 0) {
        int elem = hp[1];
        deleteRoot(hp,cursize,k);
 
        int start = pos[elem];
        int end;
        if(elem+1 < V)
            end = pos[elem+1];
        else
            end = wn;

        for(int i = start;i<end;i++)
        {
            int v = neigh[i];
            int wt = weight[i];
            if (dist[v] > dist[elem] + wt) {
                // Updating distance of v
                dist[v] = dist[elem] + wt ;
                insertNode(hp, cursize,dist[v],v,k);
            }
        }
    }
    double endtime = rtclock();  
    serdijtime = endtime - starttime;
    printtime("Serial Dijkstra Time Taken ", starttime, endtime);
}

void setGraph(graph &G,int pos[],int neigh[],int V,int E){

    for(int i = 0; i < V; i++) {
        pos[i] = G.indexofNodes[i];
    }

    for(int i = 0; i < E; i++) {
        neigh[i] = G.edgeList[i];
    }

    // printArray(pos,pn,k);
    // printArray(neigh,wn,k);
    // printArray(weight,wn,k);

    cout <<"No of Verices : "<< V << endl;
    cout <<"No of Edges   : "<< E << endl;
}
//pos,neigh,weight,Source,E,V,dist,k,pardijtime
void parDijkstra(int pos[],int neigh[],int weight[],int Source,int E,int V,int dist[],int k,double &pardijtime){
    int curSize = 0;
    int elemSize = 0; //equal to the max degree in the graph
    int d_elements[maxSize*k];
    int heap[maxSize*k];

    // printArray(dist,V,k);
    double starttime = rtclock();

    for(int i = 0;i<V;i++) {
        dist[i] = 10000000;
    }

    dist[Source] = 0;
    elemSize = 1;
    d_elements[0] = 0;
    d_elements[1] = Source;

    //Initialization
    kernel(heap,d_elements,elemSize,curSize,k);
    // cout << *curSize<<endl;
        
    while(curSize != 0)
    {
        cout << "Heap Before Deletion: ";
        printArray(heap,curSize*k,k);
        int elem = heap[1];
        deleteRoot(heap,curSize,k); //call delete here
        cout << "Heap After Deletion : ";
        printArray(heap,curSize*k,k);
        elemSize = 0;
        // for(auto neigh : adj[elem])
        int start = pos[elem];
        int end;
        if(elem+1 < V)
            end = pos[elem+1];
        else
            end = E;

        // cout << "elem = "<<elem<<", start = "<<start<<", end = "<<end<<endl;
        for(int i = start;i<end;i++)
        {
            int v = neigh[i];
            int wt = weight[i];
            // cout << "dist[elem] = "<<dist[elem]<<", wt = "<<wt<<", v = "<<v<<", dist[v] = "<<dist[v]<<endl;
            if(dist[elem] + wt < dist[v])
            {
                dist[v] = dist[elem]+wt;
                d_elements[elemSize*2] = dist[v];
                d_elements[elemSize*2 + 1] = v;
                elemSize = elemSize + 1;
            }
        }
        cout << "Inserted Elements in Heap : ";
        printArray(d_elements,elemSize*k,k);
        if(elemSize != 0)
        {
            kernel(heap,d_elements,elemSize,curSize,k);
        }
        cout << "CurSize of the Heap : "<<curSize<<endl;
        cout << "Heap After Insertion : ";  
        printArray(heap,curSize*k,k);
        cout << "Distance Vector : ";
        printArray(dist,V,k);
        cout<<"............................................."<<endl;
        
    }
    double endtime = rtclock();  
    pardijtime = endtime - starttime;
    printtime("Parallel Dijkstra Time Taken ", starttime, endtime);
}

void compareDis(int dist[],int dist2[],int V,int k)
{
    int flag = 0;
    for(int i = 0;i<V;i++){
        if(dist[i] != dist2[i]){
            flag++;
        }
    }

    cout << "No of Errors : "<<flag << endl;

    printArray(dist,V,k);
    printArray(dist2,V,k);
}

int main(int argc, char* argv[]) {

    int k = 2,Source = 0;
    double st = rtclock();
    graph G(argv[1]);
	G.parseGraph();
     int V = G.num_nodes();
    int E = G.num_edges();
    double et = rtclock();
    printtime("Parsing Graph Time :", st, et);
    cout <<"No of Verices : "<< V << endl;
    cout <<"No of Edges   : "<< E << endl;
    srand(time(0));
    //omp_set_dynamic(0);
    //omp_set_num_threads(8);
    int num = omp_get_num_threads();
    // #pragma omp single
    cout << "No of Threads: "<<num<<endl;

    for(int i = 0;i<maxSize;i++)
        omp_init_lock(&(lock[i]));

    int pos[V],neigh[E],*weight = G.getEdgeLen();;
    setGraph(G,pos,neigh,V,E);
    int dist[V],dist2[V];

    // printArray(pos,V,k);
    // printArray(neigh,E,k);
    // printArray(weight,E,k);

    double serdijtime,pardijtime;
    parDijkstra(pos,neigh,weight,Source,E,V,dist,k,pardijtime);
     serDijkstra(pos,neigh,weight,E,Source,dist2,V,k,serdijtime);
    
     double perf = serdijtime/pardijtime;
    
     cout << "Performance Gained :"<<perf<<endl;

     compareDis(dist,dist2,V,k);

    for(int i = 0;i<maxSize;i++)
            omp_destroy_lock(&(lock[i]));

    return 0;
}
