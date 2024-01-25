#include <cstdio>        // Added for printf() function 
#include <sys/time.h>    // Added to get time of day
#include <cuda.h>
#include <fstream>
#include <time.h>
#include <iostream>
#include "graph.hpp"
#define ll long long

using namespace std;
//total size of the heap
#define maxSize 1000000000000

__global__ void Insert_Elem(volatile ll int *heap,ll int *d_elements,ll int *curSize,volatile ll int *lockArr,ll int *elemSize,ll int k){
    ll int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid < *elemSize)
    {
        ll  int childInd = atomicInc((long long unsigned *) curSize,maxSize);
        childInd = childInd*k;
        for(int i = 0;i<k;i++)
            heap[childInd+i] = d_elements[tid*k+i];

        ll  int parInd = ((childInd/k - 1)/2) * k;

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
                            ll int temp = heap[parInd+i];    //swapping the elements
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

bool checkHeap(ll int *ar,ll int size,ll int k)
{
    for(ll int i = 0;i<size/2;i+=k)
    {
        if(ar[i] > ar[2*i + k]){
            printf("\nproblem found at index parent = %lld,child = %lld\n",i,2*i + k);
            printf("problem found at index parentval = %lld,childval = %lld\n",ar[i],ar[2*i + k]); 
            return false;
        } 
        if((2*i + 2) < size && ar[i] > ar[2*i + 2*k]){
            printf("\nproblem found at index parent = %lld,child = %lld\n",i,2*i + 2*k);
            printf("problem found at index parentval = %lld,childval = %lld\n",ar[i],ar[2*i + 2*k]);
            return false;
        }
    }
    return true;
}

ll int getRandom(ll int lower,ll int upper)
{
    ll int num = (rand() % (upper - lower + 1)) + lower;
    return num;  
}
void printArray(ll int arr[],ll int size,ll int k)
{
    for(ll int i = 0;i<size;i++)
    {
        if(arr[i] == 10000000)
            printf("-1, ");
        else
            printf("%lld, ",arr[i]);
    }
    
    cout<<endl;
}
void FillArray(ll int elements[],ll int size,ll int k)
{
    for(ll int i = 0;i<size*k;i++)
    {
        elements[i] = getRandom(1,1000);
    }
}
    
void heapify(ll int hp[],ll int ind,ll int size,ll int k)
{
    while(1)
    {
        ll int leftChild = 2*ind+k;
        ll int rightChild = 2*ind+2*k;
        ll int largeInd = -1;
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
	
    
        for(ll int i = 0;i<k;i++){
            ll int temp = hp[ind+i];
            hp[ind+i] = hp[largeInd+i];
            hp[largeInd+i] = temp;
        }

        ind = largeInd;
        
    }

}

void heapifyBUP(ll int arr[], ll int n, ll int childInd, ll int k) {
    // Find parent 
    ll int parInd = ((childInd/k - 1)/2) * k;
    if (parInd >= 0) { 
        if (arr[childInd] < arr[parInd]) { 
            for(ll int i = 0;i<k;i++){
                ll int temp = arr[parInd+i];
                arr[parInd+i] = arr[childInd+i];
                arr[childInd+i] = temp;
            }
            heapifyBUP(arr, n, parInd,k); 
        } 
    } 
}

void insertNode(ll int arr[], ll  int& n, ll int Key,ll int val,ll int k)
{
    // Increase the size of Heap by 2
    n = n + 1;
    ll int childInd = n*k;
 
    // Insert the element at end of Heap
    arr[childInd - 2] = Key;
    arr[childInd - 1] = val;
 
    // Heapify the new node following a
    // Bottom-up approach
    heapifyBUP(arr, n,childInd-k,k);
}

void deleteRoot(ll int arr[],ll int *n,ll int k)
{
    for(ll int i = 0;i<k;i++){
        arr[i] = arr[(*n -1)*2 + i];
    }
 
    // Decrease size of heap by 1
    *n = *n - 1;
 
    // heapify the root node
    heapify(arr,0,*n,k);
}

void buildHeap(ll int hp[],ll int n,ll int k)
{
    for(ll int i = n/2 -1 ; i>=0;i--)
    {
        heapify(hp,i*k,n,k);
    }
}

__global__ void setLockVar(ll int *curSize,ll int *lockArr,ll int *elemSize)
{
    long long int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid < *elemSize)
        lockArr[tid + *curSize] = 1;
}

double rtclock(){
    struct timezone Tzp;
    struct timeval Tp;
    ll int stat;
    stat = gettimeofday(&Tp, &Tzp);
    if (stat != 0) cout << stat;
    return(Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

void printtime(const char *str, double starttime, double endtime){
    printf("%s%3f seconds\n", str, endtime - starttime);
}

void serDijkstra(ll int *pos,ll int *neigh,ll int *weight,ll int wn,ll int src,ll int dist[],ll int V,ll int k,double &serdijtime)
{
    double starttime = rtclock();

    long long int *hp;
    cudaMalloc(&hp,(maxSize)*sizeof(ll int));
    ll int cursize = 0; 
    insertNode(hp, cursize,0,src,k);
    dist[src] = 0;
    ll int totDeg = 0,count = 0;

    while (cursize != 0) {
        ll int elem = hp[1];
        deleteRoot(hp,&cursize,k);
 
        ll int start = pos[elem];
        ll int end;
        if(elem+1 < V)
            end = pos[elem+1];
        else
            end = wn;

        if(end > start)
        {
            totDeg += (end-start);
            count++;
        }

        for(ll int i = start;i<end;i++)
        {
            ll int v = neigh[i];
            ll int wt = weight[i];
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
    cout << "Avg degree : "<<totDeg/count;
}

void setGraph(graph &G,ll int *pos,ll int *neigh, ll int V, ll int E){

    int *temp = G.getEdgeLen();
    for(ll int i = 0; i < V; i++) {
        pos[i] = G.indexofNodes[i];
    }

    for(ll int i = 0; i < E; i++) {
        neigh[i] = G.edgeList[i];
    }

    // printArray(pos,pn,k);
    // printArray(neigh,wn,k);
    // printArray(weight,wn,k);

    cout <<"No of Verices : "<< V << endl;
    cout <<"No of Edges   : "<< E << endl;
}

void parDijkstra(ll  int *curSize, ll int *elemSize, ll int *d_elements, ll int *lockArr, ll int *d_a, ll int k,double &pardijtime, ll int dist[],
ll int V, ll int E, ll int *pos,ll int *neigh,ll int *weight, ll int Source){
    ll int totDeg = 0,count = 0;
    *curSize = 0;
    *elemSize = 0; //equal to the max degree in the graph

    // printArray(dist,V,k);
    double starttime = rtclock();  
    dist[Source] = 0;
    *elemSize = 1;
    d_elements[0] = 0;
    d_elements[1] = Source;

    //Initialization
    ll int block = ceil((float) *elemSize/1024);
    setLockVar<<<block,1024>>>(curSize,lockArr,elemSize);
    cudaDeviceSynchronize();
    Insert_Elem<<<block,1024>>>(d_a,d_elements,curSize,lockArr,elemSize,k);
    cudaDeviceSynchronize();
    // cout << *curSize<<endl;
        
    while(*curSize != 0)
    {
        // printArray(d_a,*curSize*k,k);
        ll int elem = d_a[1];
        deleteRoot(d_a,curSize,k); //call delete here
        *elemSize = 0;
        // for(auto neigh : adj[elem])
        ll int start = pos[elem];
        ll int end;
        if(elem+1 < V)
            end = pos[elem+1];
        else
            end = E;

        // cout << "elem = "<<elem<<", start = "<<start<<", end = "<<end<<endl;
        if(end > start)
        {
            totDeg += (end-start);
            count++;
        }
        
        for(ll int i = start;i<end;i++)
        {
            ll int v = neigh[i];
            ll int wt = weight[i];
            
            // cout << "dist[elem] = "<<dist[elem]<<", wt = "<<wt<<", v = "<<v<<", dist[v] = "<<dist[v]<<endl;
            if(dist[elem] + wt < dist[v])
            {
                dist[v] = dist[elem]+wt;
                d_elements[*elemSize*2] = dist[v];
                d_elements[*elemSize*2 + 1] = v;
                *elemSize = *elemSize + 1;
            }
        }
        // printArray(d_elements,*elemSize*k,k);
        if(*elemSize != 0)
        {
            ll int block = ceil((float) *elemSize/1024);
            setLockVar<<<block,1024>>>(curSize,lockArr,elemSize);
            cudaDeviceSynchronize();
            Insert_Elem<<<block,1024>>>(d_a,d_elements,curSize,lockArr,elemSize,k);
            cudaDeviceSynchronize();
        }
        // cout << *curSize<<endl;
        // printArray(d_a,*curSize*k,k);
        // printArray(dist,V,k);
        // cout<<"............................................."<<endl;
        
    }
    double endtime = rtclock();  
    pardijtime = endtime - starttime;
    printtime("Parallel Dijkstra Time Taken ", starttime, endtime);
    cout << "Avg degree : "<<totDeg/count;
}

void compareDis(ll int dist[],ll int dist2[],ll int V,ll int k)
{
    ll int flag = 0;
    for(ll int i = 0;i<V;i++){
        if(dist[i] != dist2[i]){
            flag++;
	    if(flag < 100)
	    cout << i << ", "<<dist[i]<<", "<<dist2[i]<<endl;
        }
    }

    cout << "No of Errors : "<<flag << endl;

    //printArray(dist,V,k);
    //printArray(dist2,V,k);
}

int main(int argc, char* argv[]) {

    graph G(argv[1]);
    G.parseGraph();
    srand(time(0));

    ll int *curSize;
    ll int *d_a,*lockArr,*elemSize,Source = 0,*d_elements,k = 2,*pos,*neigh,*weight;
    ll int V = G.num_nodes();
    ll int E = G.num_edges();
    cudaHostAlloc(&curSize, sizeof(ll int), 0);
    cudaHostAlloc(&elemSize, sizeof(ll int), 0);
    cudaMalloc(&lockArr,(V+10)*sizeof(ll int));
    cudaMalloc(&pos,(V+10)*sizeof(ll int));
    cudaMalloc(&neigh,(E+10)*sizeof(ll int));
    cudaMalloc(&weight,(E+10)*sizeof(ll int));
    cudaMemset(lockArr,0,(V+10)*sizeof(ll int));
    cudaHostAlloc(&d_a, maxSize*k*sizeof(ll int),0);
    cudaHostAlloc(&d_elements, maxSize * sizeof(ll int),0);

    //int V = G.num_nodes();
    
    int *temp = G.getEdgeLen();

    for(ll int i = 0; i < E; i++) {
        weight[i] = temp[i];
    }

    setGraph(G,pos,neigh,V,E);

    ll int *dist,*dist2;
    cudaMalloc(&dist,(V+10)*sizeof(ll int));
    cudaMalloc(&dist2,(V+10)*sizeof(ll int));

    for(ll int i = 0;i<V;i++) {
        dist[i] =  1000000000;
        dist2[i] = 1000000000;
    }

    double serdijtime,pardijtime;
    parDijkstra(curSize,elemSize,d_elements,lockArr,d_a,k,pardijtime,dist,V,E,pos,neigh,weight,Source);
    serDijkstra(pos,neigh,weight,E,Source,dist2,V,k,serdijtime);
    
    double perf = serdijtime/pardijtime;
    
    cout << "Performance Gained :"<<perf<<endl;

    compareDis(dist,dist2,V,k);

    return 0;
}
