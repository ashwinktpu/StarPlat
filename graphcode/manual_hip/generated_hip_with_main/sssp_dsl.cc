/**
 * Manual implementation for Single Source Shortest Path
 * Parallel Algorithm using AMD HIP Code.
 *
 * @author: cs22m056
 */

#include <hip/hip_runtime.h>

#include <climits>
#include <iostream>

#include "../graph.hpp"

#define NL '\n'

using std::cin;
using std::cout;

__global__ void SSSP(
        int nV,
        bool *modified,
        bool *modifiedNext,
        int *offsetArr,
        int *edgeList,
        int *distance,
        int *weight,
        bool *isChanged
    ) {
    int vertexId = blockIdx.x * blockDim.x + threadIdx.x;

    if (vertexId >= nV)
        return;

    // printf("%d %d\n", vertexId, modified[vertexId]);

    if (modified[vertexId] && distance[vertexId] != INT_MAX) {
        for (int edgeId = offsetArr[vertexId]; edgeId < offsetArr[vertexId + 1]; edgeId++) {
            int neighbour = edgeList[edgeId];
            int updated_dist = distance[vertexId] + weight[edgeId];
            if (updated_dist < distance[neighbour]) {
                atomicMin((unsigned *)&distance[neighbour], updated_dist);
                modifiedNext[neighbour] = true;
                *isChanged = true;
            }
        }
    }
}




template <typename T>
__global__ void printArray(T *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d : %d -> ", i, array[i]);
    }
}

template <typename T>
__global__ void initKernel(int V, T *init_array, T init_value) {
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V) {
        init_array[id] = init_value;
    }
}

template <typename T>
__global__ void initIndex(int V, T *init_array, int s, T init_value) {  // intializes an index 1D array with init val

    if (s < V) {  // bound check
        init_array[s] = init_value;
    }
}

void ComputeSSSP(graph &g, int src, int *dist) {
    int nV = g.num_nodes();
    int nE = g.num_edges();
    int *edgeLens = g.getEdgeLen();  // edgeWeight

    cout << "nV: " << nV << " nE: " << nE << " " << edgeLens[100] << NL;

    int *hOffsetArr = (int *)malloc((nV + 1) * sizeof(int));
    int *hRevOffsetArr = (int *)malloc((nV + 1) * sizeof(int));
    int *hEdgeList = (int *)malloc(nE * sizeof(int));
    int *hWeight = (int *)malloc(nE * sizeof(int));

    bool hIsChanged = true;

    for (int i = 0; i <= nV; i++) {
        hOffsetArr[i] = g.indexofNodes[i];  // meta
        hRevOffsetArr[i] = g.rev_indexofNodes[i];
    }

    for (int i = 0; i < nE; i++) {
        hEdgeList[i] = g.edgeList[i];  // data
        hWeight[i] = edgeLens[i];
        cout << hWeight[i] << " ";
    }
    cout << NL;

    int *dOffsetArr;
    int *dEdgeList;
    int *dDistance;
    int *dWeight;
    bool *dModifiedNext;
    bool *dModified;
    bool *dIsChanged;

    hipMalloc(&dOffsetArr, sizeof(int) * (nV + 1));
    hipMalloc(&dEdgeList, sizeof(int) * nE);
    hipMalloc(&dModified, sizeof(bool) * nV);
    hipMalloc(&dModifiedNext, sizeof(bool) * nV);
    hipMalloc(&dDistance, sizeof(int) * nV);
    hipMalloc(&dWeight, sizeof(int) * nE);

    hipMalloc(&dIsChanged, sizeof(bool));

    hipMemcpy(dEdgeList, hEdgeList, sizeof(int) * nE, hipMemcpyHostToDevice);
    hipMemcpy(dOffsetArr, hOffsetArr, sizeof(int) * (nV + 1), hipMemcpyHostToDevice);
    hipMemcpy(dWeight, hWeight, sizeof(int) * nE, hipMemcpyHostToDevice);

    // int threads = 8;
    int threads = 1024;
    // int blocks = 1;
    int blocks = ceil((nV + 1) / (float)threads);
    // initKernel<int><<<blocks, threads>>>(nV, dWeight, 1); //! TODO : Weighted graphs???? Remove this
    std::cout << threads << " " << blocks << "\n";

    hipEvent_t start, stop;
    hipEventCreate(&start);
    hipEventCreate(&stop);
    float milliseconds = 0;
    hipEventRecord(start, 0);

    initKernel<bool><<<blocks, threads>>>(nV, dModified, false);
    initKernel<bool><<<blocks, threads>>>(nV, dModifiedNext, false);
    initKernel<int><<<blocks, threads>>>(nV, dDistance, INT_MAX);  //! TODO: Combine into one call when inside loop
    //! TODO: Above is fine for now

    initIndex<bool><<<1, 1>>>(nV, dModified, src, (bool)true);
    initIndex<int><<<1, 1>>>(nV, dDistance, src, (int)0);

    // printWeight<<<1,1>>>(src, dModified);
    // printWeight<<<1,1>>>(src, dDistance);
    // printWeight<<<1,1>>>(3, dModified);
    // printWeight<<<1,1>>>(3, dDistance);

    hipDeviceSynchronize();

    int k = 0;
    while (hIsChanged) {
        // std::cout << "Iteration: " << k++ << "\n";

        hIsChanged = false;
        hipMemcpy(dIsChanged, &hIsChanged, sizeof(bool), hipMemcpyHostToDevice);

        SSSP<<<blocks, threads>>>(
            nV, dModified, dModifiedNext,
            dOffsetArr, dEdgeList,
            dDistance, dWeight, dIsChanged);

        hipDeviceSynchronize();

        hipMemcpy(&hIsChanged, dIsChanged, sizeof(bool), hipMemcpyDeviceToHost);
        hipMemcpy(dModified, dModifiedNext, sizeof(bool) * nV, hipMemcpyDeviceToDevice);  //! TODO: Check email
        //! TODO: Launch a copy kernel?????
        initKernel<bool><<<blocks, threads>>>(  //! TODO: Avoid above memcpy byt modifying in the kernel itself.
            nV, dModifiedNext, false);
    }

    hipEventRecord(stop, 0);
    hipEventSynchronize(stop);
    hipEventElapsedTime(&milliseconds, start, stop);
    printf("GPU Time: %.6f ms\n", milliseconds);

    printWeight<<<1, 1>>>(0, dDistance);
    printWeight<<<1, 1>>>(1, dDistance);
    printWeight<<<1, 1>>>(3, dDistance);
    printWeight<<<1, 1>>>(7, dDistance);

    hipDeviceSynchronize();

    hipMemcpy(dist, dDistance, sizeof(int) * (nV), hipMemcpyDeviceToHost);
}


int main(int argc, char *argv[]) {
    graph G(argv[1]);
    G.parseGraph();

    int *distance = (int *)malloc((G.num_nodes() + 1) * sizeof(int));
    int src = 5;

    ComputeSSSP(G, src, distance);

    std::ofstream file(std::string(argv[1]) + ".out", std::ios::out);

    for (int i = 0; i < G.num_nodes(); i++) {
        file << distance[i] << "\n";
    }
    std::cout << "Completed" << std::endl;

    return 0;
}
