#include "graph.hpp"
#include <stdio.h>
#include<hip/hip_runtime.h>
#include<bits/stdc++.h>

using namespace std;

struct node_set{
    int rank;
    int parent;
};

//find ultimate parent
__device__ int findParent(int id, node_set* d_components){
    int ultimate_parent = id;
    while(d_components[ultimate_parent].parent != ultimate_parent){
        ultimate_parent = d_components[ultimate_parent].parent;
    }

    int currNode = id;
    while(currNode != ultimate_parent){
        int temp = d_components[currNode].parent;
        d_components[currNode].parent = ultimate_parent;
        currNode = temp;
    }
    d_components[id].parent = ultimate_parent;
    return ultimate_parent;
}

//unionByRank for components
__device__ void unionByRank(int id1, int id2, node_set* d_components){

    int p1 = findParent(id1, d_components);
    int p2 = findParent(id2, d_components);

    if(p1 == p2)
        return;
    
    if(d_components[p1].rank < d_components[p2].rank){
        d_components[p1].parent = p2;
    }
    else if(d_components[p1].rank > d_components[p2].rank){
        d_components[p2].parent = p1;
    }
    else{
        d_components[p2].parent = p1;
        d_components[p1].rank ++;
    }
    return;
}

__global__ void start(){
    printf("\nstarting...");
}
//initialise the union find data structure
__global__ void init_components(node_set* d_components, int nV){
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid < nV){
        d_components[tid].parent = tid;
        d_components[tid].rank = 0;
    }
}

__global__ void init_min_weight_kernel(int *d_min_weight, int nV){
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if(tid < nV){
        d_min_weight[tid]= 1e9;
    }
}

__global__ void mst_min_kernel(node_set *d_components, volatile int *min_edge, int *d_edge_covered, volatile int *lock_tid, 
        edge *edges, int *dOffsetArr, int nV, int nE, bool *d_hasComponent, unsigned long long *d_mstWeight, int *d_min_weight){

    unsigned tid = blockDim.x * blockIdx.x + threadIdx.x; 
    
    if(tid < nV){
        int no_of_nbr = dOffsetArr[tid + 1] - dOffsetArr[tid];
        for(int i = 0; i < no_of_nbr; i++){
            int edgeind = dOffsetArr[tid] + i;

            if(d_edge_covered[edgeind] == 1)
                continue;

            int comp1 = d_components[edges[edgeind].source].parent;
            atomicMin(&d_min_weight[comp1], edges[edgeind].weight);
        }
    }
}

__global__ void set_min_index_kernel(node_set* d_components, volatile int* min_edge, int* d_edge_covered, volatile int* lock_tid, 
        edge* edges, int *dOffsetArr, int nV, int nE, bool *d_hasComponent, unsigned long long *d_mstWeight, int *d_min_weight){
    unsigned tid = blockDim.x * blockIdx.x + threadIdx.x; 
    if(tid < nV){
        int no_of_nbr = dOffsetArr[tid + 1] - dOffsetArr[tid];
        for(int i = 0; i < no_of_nbr; i++){
            int edgeind = dOffsetArr[tid] + i;

            if(d_edge_covered[edgeind] == 1)
                continue;

            int comp1 = d_components[edges[edgeind].source].parent;

            if(d_min_weight[comp1] == edges[edgeind].weight)
                min_edge[comp1] = edgeind;
        }
    }
}

__global__ void set_comp_kernel(node_set* d_components, volatile int* min_edge, int* d_edge_covered, volatile int* lock_tid, 
        edge* edges, int *dOffsetArr, int nV, int nE, bool *d_hasComponent, unsigned long long *d_mstWeight, int *d_min_weight, int *d_comp1_arr, int *d_comp2_arr){
    unsigned tid = blockDim.x * blockIdx.x + threadIdx.x; 
    
    if(tid < nV){
        int parent = d_components[tid].parent;
        if(parent == tid && min_edge[tid] != -1){
            d_comp1_arr[tid] = d_components[edges[min_edge[tid]].source].parent;
            d_comp2_arr[tid] = d_components[edges[min_edge[tid]].destination].parent;
        }
    }
}
__global__ void mst_merge_kernel(node_set* d_components, volatile int* min_edge, int* d_edge_covered, volatile int* lock_tid, 
        edge* edges, int *dOffsetArr, int nV, int nE, bool *d_hasComponent, unsigned long long *d_mstWeight, int *d_min_weight, int *d_oppedgeindex, int *d_comp1_arr, int *d_comp2_arr){

    unsigned tid = blockDim.x * blockIdx.x + threadIdx.x; 
    if(tid < nV){
        if(d_comp1_arr[tid] != -1){

            int comp1 = d_comp1_arr[tid];
            int comp2 = d_comp2_arr[tid];

            if(comp1 == comp2){
                d_edge_covered[min_edge[tid]] = 1;
                d_edge_covered[d_oppedgeindex[min_edge[tid]]] = 1;
            }
            else{  
                if(comp1 > comp2){
                    int tmp = comp2;
                    comp2 = comp1;
                    comp1 = tmp;
                }
                if(atomicCAS((int*)&lock_tid[comp1], -1, comp1) == -1){                    
                    if(atomicCAS((int*)&lock_tid[comp2], -1, comp2) == -1){
                        int comp1var = findParent(comp1, d_components);
                        int comp2var = findParent(comp2, d_components);
                        if(comp1var != comp2var){
                            atomicAdd(d_mstWeight, (unsigned long)edges[min_edge[tid]].weight);
                            unionByRank(comp1var, comp2var, d_components);
                            d_edge_covered[min_edge[tid]] = 1;
                            d_edge_covered[d_oppedgeindex[min_edge[tid]]] = 1;
                        }
                        lock_tid[comp2] = -1;
                    }
                    lock_tid[comp1] = -1;
                }
            }
        }
    }
}

__global__ void mst_components_kernel(node_set* d_components, volatile int* min_edge, int* d_edge_covered, volatile int* lock_tid, 
        edge* edges, int *dOffsetArr, int nV, int nE, bool *d_hasComponent, unsigned long long *d_mstWeight){

    unsigned tid = blockDim.x * blockIdx.x + threadIdx.x; 

    if(tid < nV){
        if(d_components[0].parent != d_components[tid].parent)
            *d_hasComponent = true;
    }
}

//to store opposite edge to each edge
vector<string> splitString(string &s) {
    vector<string> nums;
    string curr = "";
    for(int i=0;i<s.size();i++) {
        if(s[i] == ',') {
            nums.push_back(curr);
            curr = "";
        }
        else {
            curr += s[i];
        }
    }
    nums.push_back(curr);
    return nums;
}

unsigned long mst_boruvkas(graph& g){

    int nV = g.num_nodes();
    int nE = g.num_edges();

    std::vector<edge> graphEdges = g.graph_edge;

    edge *h_edgeArr = (edge*)malloc(nE * sizeof(edge));
    bool *h_hasComponent = (bool*)malloc(sizeof(bool));
    int *oppedgeindex = (int*) malloc(nE * sizeof(int));
    int *hOutNbrWeight = (int*) malloc(nE * sizeof(int));
    int *hOffsetArr = (int*) malloc((nV + 1) * sizeof(int));
    int *hOffsetPtr = (int*) malloc((nV + 1) * sizeof(int));
    unsigned long long *h_mstWeight = (unsigned long long*)malloc(sizeof(unsigned long long));

    hOffsetArr = g.indexofNodes;

    for(int i = 0; i <= nV; i++) {
        hOffsetArr[i] = g.indexofNodes[i];
        hOffsetPtr[i] = g.indexofNodes[i];
    }
    
    unordered_map<string, int> mp;

    for(int i = 0; i < nE; i++) {
        int src = graphEdges[i].source;
        int dest = graphEdges[i].destination;
        int wt = graphEdges[i].weight;
        h_edgeArr[hOffsetPtr[src]].source = src;
        h_edgeArr[hOffsetPtr[src]].destination = dest;
        h_edgeArr[hOffsetPtr[src]].weight = wt;
        hOffsetPtr[src]++;
        string key = to_string(src) + "," + to_string(dest);
        mp[key] = hOffsetPtr[src]-1;
    }

    for(auto itr : mp){
        string key = itr.first;
        int index = itr.second;
        vector<string> nums = splitString(key);
        string src = nums[0], dest = nums[1];
        oppedgeindex[mp[dest + "," + src]] = index;

    }

    int *dOffsetArr;
    volatile int *d_min_edge;
    int *d_edge_covered;
    int *d_min_weight;
    bool *d_hasComponent;
    edge *d_edgeArr;
    node_set *d_components;
    volatile int *d_lock_tid;
    unsigned long long *d_mstWeight;
    int *d_oppedgeindex;
    int *d_comp1_arr;
    int *d_comp2_arr;
    
    hipMalloc(&dOffsetArr, (nV+1) * sizeof(int));
    hipMalloc(&d_mstWeight, sizeof(unsigned long long));
    hipMalloc(&d_hasComponent, sizeof(bool));
    hipMalloc(&d_min_weight, nV * sizeof(int));
    hipMalloc(&d_components, (nV) * sizeof(node_set));
    hipMalloc(&d_min_edge, (nV) * sizeof(volatile int));
    hipMalloc(&d_lock_tid, (nV) * sizeof(volatile int));
    hipMalloc(&d_comp1_arr, (nV) * sizeof(int));
    hipMalloc(&d_comp2_arr, (nV) * sizeof(int));
    hipMalloc(&d_edgeArr, (nE) * sizeof(edge));
    hipMalloc(&d_edge_covered, (nE) * sizeof(int));
    hipMalloc(&d_oppedgeindex, (nE) * sizeof(int));

    memset(h_hasComponent, true, sizeof(bool));
    memset(h_mstWeight, 0, sizeof(unsigned long long*));

    hipMemset(d_hasComponent, true, sizeof(bool));
    hipMemset(d_mstWeight, 0, sizeof(unsigned long long*));
    hipMemset(d_edge_covered, 0, (nE) * sizeof(int));
    hipMemset((void*)d_lock_tid, -1, (nV) * sizeof(volatile int));

    hipMemcpy(dOffsetArr, hOffsetArr, sizeof(int)*(nV+1), hipMemcpyHostToDevice);
    hipMemcpy(d_edgeArr, h_edgeArr, (nE) * sizeof(edge), hipMemcpyHostToDevice);
    hipMemcpy(d_oppedgeindex, oppedgeindex, (nE) * sizeof(int), hipMemcpyHostToDevice);

    int numThreads = 1024;
    int numBlocks = (nV + 1024) / 1024;
    

    //initialise the union set data structure 
    init_components<<<numBlocks, numThreads>>>(d_components, nV);
    int t=0;
    cout<<"kernel starting"<<endl;
    
    while(*h_hasComponent){
        //reset variables
        hipMemset((void*)d_min_edge, -1, (nV) * sizeof(volatile int));
        hipMemset((void*)d_comp1_arr, -1, (nV) * sizeof(int));
        hipMemset((void*)d_comp2_arr, -1, (nV) * sizeof(int));

        //set hasComponent to false
        hipMemset(d_hasComponent, false, sizeof(bool));
        //Kernel launched if more than two components exists to calculate min_edge for all vertex
        init_min_weight_kernel<<<numBlocks, numThreads>>>(d_min_weight, nV);
        //kernel to calculate min weighted edge from each component
        mst_min_kernel<<<numBlocks, numThreads>>>(d_components, d_min_edge, d_edge_covered, d_lock_tid, d_edgeArr, dOffsetArr, nV, nE, d_hasComponent, d_mstWeight, d_min_weight);
        //kernel to set index of min weighted edge from each component
        set_min_index_kernel<<<numBlocks, numThreads>>>(d_components, d_min_edge, d_edge_covered, d_lock_tid, d_edgeArr, dOffsetArr, nV, nE, d_hasComponent, d_mstWeight, d_min_weight);
        //calculate parent of both nodes to be merged
        set_comp_kernel<<<numBlocks, numThreads>>>(d_components, d_min_edge, d_edge_covered, d_lock_tid, d_edgeArr, dOffsetArr, nV, nE, d_hasComponent, d_mstWeight, d_min_weight, d_comp1_arr, d_comp2_arr);
        //Kernel launched to merge different components if exist
        mst_merge_kernel<<<numBlocks, numThreads>>>(d_components, d_min_edge, d_edge_covered, d_lock_tid, d_edgeArr, dOffsetArr, nV, nE, d_hasComponent, d_mstWeight, d_min_weight, d_oppedgeindex, d_comp1_arr, d_comp2_arr);
        //check if only one component left
        mst_components_kernel<<<numBlocks, numThreads>>>(d_components, d_min_edge, d_edge_covered, d_lock_tid, d_edgeArr, dOffsetArr, nV, nE, d_hasComponent, d_mstWeight);
        hipDeviceSynchronize();
        hipMemcpy(h_mstWeight, d_mstWeight, sizeof(unsigned long long), hipMemcpyDeviceToHost);
        hipMemcpy(h_hasComponent, d_hasComponent, sizeof(bool), hipMemcpyDeviceToHost);
        
        printf("iteration : %d : %llu \n", t, *h_mstWeight);
    }
    hipMemcpy(h_mstWeight, d_mstWeight, sizeof(unsigned long long), hipMemcpyDeviceToHost);
    return *h_mstWeight;
}

int main(int argc, char* argv[]) {
    graph g(argv[1]);
    std::cout << "Started parsing" << std::endl;
    g.parseGraph();
    std::cout << "Parsing done" << std::endl;
    std::cout<<"The Weight of the minimum spanning tree is : "<<  mst_boruvkas(g) <<std::endl;
    return 0;
}