//Manual implementation for finding number of strongly connected components in the directed graph

#include "../graph.hpp"
#include <stdio.h>
#include <hip/hip_runtime.h>
#include <iostream>

using namespace std;

__global__ void markCurrentComponentVisited(bool *dForwardVisited, bool *dBackwardVisited, bool *dVisited, int nV) {
    int vertexId = blockIdx.x * blockDim.x + threadIdx.x;
    if(vertexId >= nV)
        return;

    if(dForwardVisited[vertexId] && dBackwardVisited[vertexId]) {
        dVisited[vertexId] = true;
    }
    else {
        dForwardVisited[vertexId] = false;
        dBackwardVisited[vertexId] = false;
    }
}

__global__ void initKernel(int *dVisited, int index) {
    dVisited[index] = 1;
}

__global__ void bfsKernel(bool *dVisited, int *dLevelVisitedNodes, int* dOffset, int *dNbr, bool *dNewVertexVisited, int nV, bool *fwVisited, bool isBwPass) {
    int vertexId = blockIdx.x * blockDim.x + threadIdx.x;
    if(vertexId >= nV)
        return;
    if(dLevelVisitedNodes[vertexId] != 1)
        return;
    if(dVisited[vertexId])          //Don't visit node, if it is already part of some component
        return;
    if(isBwPass && !fwVisited[vertexId])        //In bw pass, don't visit vertex which we have not visited in fw pass
        return;
    dVisited[vertexId] = true;
    *dNewVertexVisited = true;
    for(int i=dOffset[vertexId];i<dOffset[vertexId+1];i++)
        if(!dVisited[dNbr[i]] && dLevelVisitedNodes[dNbr[i]] == 0)
            dLevelVisitedNodes[dNbr[i]] = 1;
    dLevelVisitedNodes[vertexId] = 2;
}

__global__ void removeComponentWithSingleNode(int *dOffsetArr, int *dRevOffsetArr, bool *dVisited, int nV, int* dComponentCount, bool *isChanged, int *dInNbr, int *dOutNbr) {
    int vertexId = blockIdx.x * blockDim.x + threadIdx.x;
    if(vertexId >= nV)
        return;
    if(dVisited[vertexId])
        return;
    int inNbrCount = 0, outNbrCount = 0;
    for(int i=dOffsetArr[vertexId];i<dOffsetArr[vertexId+1];i++) {
        if(!dVisited[dOutNbr[i]]) {
            outNbrCount++;
        }
    }
    for(int i=dRevOffsetArr[vertexId];i<dRevOffsetArr[vertexId+1];i++) {
        if(!dVisited[dInNbr[i]]) {
            inNbrCount++;
        }
    }
    if(inNbrCount == 0 || outNbrCount == 0) {
        atomicAdd(&dComponentCount[0], 1);
        dVisited[vertexId] = true;
        *isChanged = true;
    }
}

__global__ void findMaxDegree(bool *dVisited, int nV, int *dOffsetArr, int *dRevOffsetArr, int *maxDegree) {
    int vertexId = blockIdx.x * blockDim.x + threadIdx.x;
    if(vertexId >= nV)
        return;
    if(dVisited[vertexId])
        return;
    int currNodeDegree = (dOffsetArr[vertexId+1]-dOffsetArr[vertexId]) + (dRevOffsetArr[vertexId+1]-dRevOffsetArr[vertexId]);
    atomicMax(maxDegree, currNodeDegree);
}

__global__ void findNodeWithMaxDegree(bool *dVisited, int *dNodeToBeVisited, int nV, int *dOffsetArr, int *dRevOffsetArr, int *maxDegree) {
    int vertexId = blockIdx.x * blockDim.x + threadIdx.x;
    if(vertexId >= nV)
        return;
    if(dVisited[vertexId])
        return;
    int currNodeDegree = (dOffsetArr[vertexId+1]-dOffsetArr[vertexId]) + (dRevOffsetArr[vertexId+1]-dRevOffsetArr[vertexId]);
    if(currNodeDegree >= *maxDegree)
        *dNodeToBeVisited = vertexId;
}

void trim0(int *dOffsetArr, int *dRevOffsetArr, bool *dVisited, int V, int *dComponentCount, int *dInNbr, int *dOutNbr) {
    bool hIsChanged = true;
    bool *dIsChanged;
    hipMalloc(&dIsChanged, sizeof(bool));

    int numBlocks = (V+1023)/1024;
    int numThreads = V >= 1024 ? 1024 : V;

     while(hIsChanged) {
        hIsChanged = false;
        hipMemcpy(dIsChanged, &hIsChanged, sizeof(bool), hipMemcpyHostToDevice);
        removeComponentWithSingleNode<<<numBlocks, numThreads>>>(dOffsetArr, dRevOffsetArr, dVisited, V, dComponentCount, dIsChanged, dInNbr, dOutNbr);
        hipDeviceSynchronize();
        hipMemcpy(&hIsChanged, dIsChanged, sizeof(bool), hipMemcpyDeviceToHost);
     }
}   

void bfs(int nodeToBeVisited, bool *dVisited, int *dOffset, int *dNbr, int nV, int nE, bool *dFwVisited, bool isBwPass, int *dLevelVisitedNodes) {
    bool hNewVertexVisited = true;
    bool *dNewVertexVisited;
    hipMalloc(&dNewVertexVisited, sizeof(bool));
    hipMemset(dLevelVisitedNodes, 0, nV*sizeof(int));
    initKernel<<<1, 1>>>(dLevelVisitedNodes, nodeToBeVisited);

    int numBlocks = (nV+1023)/1024;
    int numThreads = nV >= 1024 ? 1024 : nV;

    while(hNewVertexVisited) {
        hipMemset(dNewVertexVisited, false, sizeof(bool));
        bfsKernel<<<numBlocks, numThreads>>>(dVisited, dLevelVisitedNodes, dOffset, dNbr, dNewVertexVisited, nV, dFwVisited, isBwPass);
        hipDeviceSynchronize();
        hipMemcpy(&hNewVertexVisited, dNewVertexVisited, sizeof(bool), hipMemcpyDeviceToHost);
    }
}

void scc(graph& g) {
    // CSR BEGIN
    int nV = g.num_nodes();
    int nE = g.num_edges();

    printf("#nodes:%d\n",nV);
    printf("#edges:%d\n",nE);
    
    vector<edge> graphEdges = g.graph_edge;

    int *hOffsetArr = (int*) malloc((nV + 1) * sizeof(int));
    int *hOffsetPtr = (int*) malloc((nV + 1) * sizeof(int));
    int *hRevOffsetArr = (int*) malloc((nV + 1) * sizeof(int));
    int *hRevOffsetPtr = (int*) malloc((nV + 1) * sizeof(int));
    int *hOutNbr = (int*) malloc(nE * sizeof(int));
    int *hInNbr = (int*) malloc(nE * sizeof(int));
    int hNodeToBeVisited;
    int *hInDegree = (int*) malloc(nV * sizeof(int));
    int *hOutDegree = (int*) malloc(nV * sizeof(int));

    memset(hOutNbr, 0, nE * sizeof(int));
    memset(hInNbr, 0, nE * sizeof(int));

    
    for(int i = 0; i <= nV; i++) {
        hOffsetArr[i] = g.indexofNodes[i];
        hOffsetPtr[i] = g.indexofNodes[i];
        hRevOffsetPtr[i] = g.rev_indexofNodes[i];
        hRevOffsetArr[i] = g.rev_indexofNodes[i];
    }
    
    for(int i=0;i<nV;i++) {
        hOutDegree[i] = g.outDeg[i];
        hInDegree[i] = g.inDeg[i];
    }

    for(int i = 0; i < nE; i++) {
        int src = graphEdges[i].source;
        int dest = graphEdges[i].destination;
        hOutNbr[hOffsetPtr[src]] = dest;
        hInNbr[hRevOffsetPtr[dest]] = src;
        hOffsetPtr[src]++;
        hRevOffsetPtr[dest]++;
    }

    int *dOffsetArr;
    int *dRevOffsetArr;
    int *dOutNbr;
    int *dInNbr;
    bool *dVisited;
    bool *dForwardVisited;
    bool *dBackwardVisited;
    int *dNodeToBeVisited;
    int *dComponentCount;
    int *dSccWithOneNode;
    int *maxDegree;
    int *dLevelVisitedNodes;

    hipMalloc(&dOffsetArr, (nV+1)*sizeof(int));
    hipMalloc(&dRevOffsetArr, (nV+1)*sizeof(int));
    hipMalloc(&dOutNbr, nE*sizeof(int));
    hipMalloc(&dInNbr, nE*sizeof(int));
    hipMalloc(&dVisited, nV*sizeof(bool));
    hipMalloc(&dForwardVisited, nV*sizeof(bool));
    hipMalloc(&dBackwardVisited, nV*sizeof(bool));
    hipMalloc(&dNodeToBeVisited, sizeof(int));
    hipMalloc(&dComponentCount, sizeof(int));
    hipMalloc(&dSccWithOneNode, sizeof(int));
    hipMalloc(&maxDegree, sizeof(int));
    hipMalloc(&dLevelVisitedNodes, nV*sizeof(int));

    hipMemcpy(dOffsetArr, hOffsetArr, sizeof(int)*(nV+1), hipMemcpyHostToDevice);
    hipMemcpy(dRevOffsetArr, hRevOffsetArr, sizeof(int)*(nV+1), hipMemcpyHostToDevice);
    hipMemcpy(dOutNbr, hOutNbr, sizeof(int)*nE, hipMemcpyHostToDevice);
    hipMemcpy(dInNbr, hInNbr, sizeof(int)*nE, hipMemcpyHostToDevice);

    hipMemset(dVisited, false, sizeof(bool)*nV);
    hipMemset(dForwardVisited, false, sizeof(bool)*nV);
    hipMemset(dBackwardVisited, false, sizeof(bool)*nV);
    hipMemset(dComponentCount, 0, sizeof(int));
    hipMemset(dSccWithOneNode, 0, sizeof(int));
    hipMemset(dLevelVisitedNodes, 0, nV*sizeof(int));

    bool *hVisited = (bool*) malloc(nV * sizeof(bool));
    bool *hForwardVisited = (bool*) malloc(nV * sizeof(bool));
    bool *hBackwardVisited = (bool*) malloc(nV * sizeof(bool));
    int hComponentCount = 0;
    int hSccWithOneNode;
    int count = 0;
    int numBlocks = (nV+1023)/1024;
    int numThreads = nV >= 1024 ? 1024 : nV;

    memset(hVisited, false, nV * sizeof(bool));

    hipEvent_t start, stop;
    hipEventCreate(&start);
    hipEventCreate(&stop);
    float milliseconds = 0;
    hipEventRecord(start,0);

    while(1) {
        trim0(dOffsetArr, dRevOffsetArr, dVisited, nV, dSccWithOneNode, dInNbr, dOutNbr);
        hipMemcpy(&hSccWithOneNode, dSccWithOneNode, sizeof(int), hipMemcpyDeviceToHost);
        count += hSccWithOneNode;
        hipMemset(dNodeToBeVisited, -1, sizeof(int));
        hipMemset(maxDegree, -1, sizeof(int));
        hipMemset(dSccWithOneNode, 0, sizeof(int));
        findMaxDegree<<<numBlocks, numThreads>>>(dVisited, nV, dOffsetArr, dRevOffsetArr, maxDegree);
        findNodeWithMaxDegree<<<numBlocks, numThreads>>>(dVisited, dNodeToBeVisited, nV, dOffsetArr, dRevOffsetArr, maxDegree);
        hipDeviceSynchronize();
        hipMemcpy(&hNodeToBeVisited, dNodeToBeVisited, sizeof(int), hipMemcpyDeviceToHost);
        if(hNodeToBeVisited == -1)
            break;
        bfs(hNodeToBeVisited, dForwardVisited, dOffsetArr, dOutNbr, nV, nE, dForwardVisited, false, dLevelVisitedNodes);
        bfs(hNodeToBeVisited, dBackwardVisited, dRevOffsetArr, dInNbr, nV, nE, dForwardVisited, true, dLevelVisitedNodes);
        markCurrentComponentVisited<<<numBlocks, numThreads>>>(dForwardVisited, dBackwardVisited, dVisited, nV);
        hComponentCount++;
        
     }
    cout << "Number of component in the given graph is " << hComponentCount+count << endl;

    hipEventRecord(stop,0);
    hipEventSynchronize(stop);
    hipEventElapsedTime(&milliseconds, start, stop);
    printf("GPU Time: %.6f ms\n", milliseconds);
}

int main(int argc, char* argv[]) {
    graph g(argv[1]);
    cout << "Started parsing" << endl;
    g.parseGraph();
    cout << "Parsing done" << endl;
    scc(g);
    return 0;
}
