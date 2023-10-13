#include <cstdio>        // Added for printf() function 
#include <sys/time.h>    // Added to get time of day
#include <cuda.h>
#include <bits/stdc++.h>
#include <fstream>
#include <time.h>
#include <iostream>
#include "../graph.hpp"

using namespace std;
//total size of the heap
#define maxSize 100

__global__ void Insert_Elem(volatile int *heap,int *d_elements,int *curSize,volatile int *lockArr,int *elemSize,int k){
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid < *elemSize)
    {
        int childInd = atomicInc((unsigned *) curSize,maxSize+10);
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
		                }

                        __threadfence();//necessary

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
                // __threadfence(); //doesnt seem necessary
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
    for(int i = 0;i<size;i++)
        printf("%d, ",arr[i]);
    
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

void deleteRoot(int arr[], int *n,int k)
{
    for(int i = 0;i<k;i++){
        arr[i] = arr[(*n -1)*2 + i];
    }
 
    // Decrease size of heap by 1
    *n = *n - 1;
 
    // heapify the root node
    heapify(arr,0,*n,k);
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


// void edgeListToCSR(){
//     vector <vector <int>> edge = {{0,1,9}};

//     int n;
//     cin>>
// }

// void init(graph& g,int pos[],int neigh[],int weight[])
// {
//     int nV = g.num_nodes();
//     int nE = g.num_edges();
    
// }

int main(int argc, char* argv[]) {
    graph G(argv[1]);
	G.parseGraph();
    srand(time(0));
    int countvalid = 0;
    int inivalid = 0;
    int k = 2;

    int *d_a;
    int *curSize;
    int *lockArr;
    int *elemSize;

    //lets assume i have the input as of now
    //in the csr format pos[],neigh[],weight[] 
    //                  pn,  wn

    // int pos[] = {0,2,4};
    // int neigh[] = {1,2,0,2,0,1};
    // int weight[] = {1,4,1,2,4,2};
    int Source = 0;

    int pn = G.num_nodes();
    int wn = G.num_edges();
    int V = pn;

    int pos[pn];
    int neigh[wn];
    int weight[wn];

    int *edgeLens = G.getEdgeLen();

    for(int i = 0; i < pn; i++) {
        pos[i] = G.indexofNodes[i];
    }

    for(int i = 0; i < wn; i++) {
        neigh[i] = G.edgeList[i];
        weight[i] = edgeLens[i];
    }

    cudaHostAlloc(&curSize, sizeof(int), 0);
    cudaHostAlloc(&elemSize, sizeof(int), 0);
    cudaMalloc(&lockArr,(maxSize)*sizeof(int));
    cudaMemset(lockArr,0,(maxSize)*sizeof(int));

    *curSize = 0;
 
    cudaHostAlloc(&d_a, maxSize*k*sizeof(int),0);
    

    *elemSize = 0; //equal to the max degree in the graph
    int *d_elements;
    cudaHostAlloc(&d_elements, maxSize * sizeof(int),0);

    int dist[V];
    for(int i = 0;i<V;i++) dist[i] = 1000;
    printArray(dist,V,k);

    dist[Source] = 0;
    *elemSize = 1;
    d_elements[0] = 0;
    d_elements[1] = Source;

    //Initialization
    int block = ceil((float) *elemSize/1024);
    setLockVar<<<block,1024>>>(curSize,lockArr,elemSize);
    cudaDeviceSynchronize();
    Insert_Elem<<<block,1024>>>(d_a,d_elements,curSize,lockArr,elemSize,k);
    cudaDeviceSynchronize();
    cout << *curSize<<endl;
        
    while(*curSize != 0)
    {
        // printArray(d_a,*curSize*k,k);
        int elem = d_a[1];
        deleteRoot(d_a,curSize,k); //call delete here
        *elemSize = 0;
        // for(auto neigh : adj[elem])
        int start = pos[elem];
        int end;
        if(elem+1 < pn)
            end = pos[elem+1];
        else
            end = wn;

        cout << "elem = "<<elem<<", start = "<<start<<", end = "<<end<<endl;
        for(int i = start;i<end;i++)
        {
            int v = neigh[i];
            int wt = weight[i];
            cout << "dist[elem] = "<<dist[elem]<<", wt = "<<wt<<", v = "<<v<<", dist[v] = "<<dist[v]<<endl;
            if(dist[elem] + wt < dist[v])
            {
                dist[v] = dist[elem]+wt;
                d_elements[*elemSize*2] = dist[v];
                d_elements[*elemSize*2 + 1] = v;
                *elemSize = *elemSize + 1;
            }
        }
        printArray(d_elements,*elemSize*k,k);
        if(*elemSize != 0)
        {
            int block = ceil((float) *elemSize/1024);
            setLockVar<<<block,1024>>>(curSize,lockArr,elemSize);
            cudaDeviceSynchronize();
            Insert_Elem<<<block,1024>>>(d_a,d_elements,curSize,lockArr,elemSize,k);
            cudaDeviceSynchronize();
        }
        // cout << *curSize<<endl;
        printArray(d_a,*curSize*k,k);
        printArray(dist,V,k);
        cout<<"............................................."<<endl;
        
    }

    printArray(dist,V,k);
    return 0;
}
