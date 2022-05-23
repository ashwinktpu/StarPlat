#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <utility>
#include <stdio.h>
#include <cuda.h>
#define MAX_INT 2147483647

// structure for storing edge information
struct edgeInfo {
    int src, dest, weight;
};

// structure for update information
struct updateInfo {
    char type;
    int src, dest, weight;
};

// HOST FUNCTIONS:
bool compareTwoEdges(const edgeInfo &a, const edgeInfo &b);
bool compareTwoEdgesR(const edgeInfo &a, const edgeInfo &b);
void printArray(int *arr, int len);
void checkCudaError();
unsigned long long SSSP_GPU(int numVertices, int numEdges, int *csrOffsets, int *csrCords, int *csrWeights, 
                            int *distances, int *parent, int source);
void processUpdates(int numVertices, int numEdges, int *distances, int *parent, updateInfo *&updates, 
                    int batchSize, int *csrOffsets, int *csrCords, int *csrWeights, 
                    int *csrOffsetsR, int *csrCordsR, int *csrWeightsR, 
                    int &numEdgesDiffCsr, int *&diffCsrOffsets, int *&diffCsrCords, int *&diffCsrWeights, 
                    int &numEdgesDiffCsrR, int *&diffCsrOffsetsR, int *&diffCsrCordsR, int *&diffCsrWeightsR);

// DEVICE FUNCTIONS:
template <typename T>
__global__ void init_kernel(T *array, T val, int arraySize);
__global__ void sssp_kernel(int *csrOffsets_d, int *csrCords_d, int *csrWeights_d, int *distances_d, 
                            int *parent_d, int *locks_d, int numVertices, bool *modified_d, 
                            bool *modified_next_d, bool *finished_d);
__global__ void delete_edges(int batchSize, int *distances_d, int *parent_d, updateInfo *updates_d, 
                             bool *modifiedD_d, int *csrOffsets_d, int *csrCords_d, int *csrWeights_d,
                             int *csrOffsetsR_d, int *csrCordsR_d, int *csrWeightsR_d,
                             int *diffCsrOffsets_d, int *diffCsrCords_d, int *diffCsrWeights_d,
                             int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, int *diffCsrWeightsR_d);
__global__ void mark_descendants(int *distances_d, int *parent_d, bool *modifiedD_d, 
                                 int numVertices, bool *finished_d);
__global__ void mark_not_reachable(int *distances_d, int *parent_d, int numVertices, bool *modifiedD_d);
__global__ void fetch_and_update(int *csrOffsetsR_d, int *csrCordsR_d, int *csrWeightsR_d, 
                                 int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, int *diffCsrWeightsR_d, 
                                 int *distances_d, int *parent_d, int numVertices, 
                                 bool *modifiedD_d, bool *finished_d);
__global__ void add_edges_csr(int batchSize, int *distances_d, int *parent_d, updateInfo *updates_d, 
                              bool *modifiedA_d, int *csrOffsets_d, int *csrCords_d, int *csrWeights_d,
                              int *csrOffsetsR_d, int *csrCordsR_d, int *csrWeightsR_d,
                              int *diffCsrOffsets_next_d, int *diffCsrOffsetsR_next_d);
__global__ void add_diff_edge_counts(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                     int *diffCsrWeights_d, int *diffCsrOffsetsR_d, int *diffCsrCordsR_d,
                                     int *diffCsrWeightsR_d, int *diffCsrOffsets_next_d, 
                                     int *diffCsrOffsetsR_next_d);
template <typename T>
__global__ void copy(T *destArr, T *srcArr, int arraySize);
__global__ void prefix_sum(int numVertices, int off, int *diffCsrOffsets_next_d, 
                           int *diffCsrOffsetsR_next_d, int *tempArr_d, int *tempArrR_d);
__global__ void copy_edges_diffcsr(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                   int *diffCsrWeights_d, int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, 
                                   int *diffCsrWeightsR_d, int *diffCsrOffsets_next_d, 
                                   int *diffCsrCords_next_d, int *diffCsrWeights_next_d,
                                   int *diffCsrOffsetsR_next_d, int *diffCsrCordsR_next_d, 
                                   int *diffCsrWeightsR_next_d);
__global__ void add_edges_diffcsr(int batchSize, int *distances_d, int *parent_d, 
                                  updateInfo *updates_d, bool *modifiedA_d, int *diffCsrOffsets_d, 
                                  int *diffCsrCords_d, int *diffCsrWeights_d, int *diffCsrOffsetsR_d, 
                                  int *diffCsrCordsR_d, int *diffCsrWeightsR_d);
__global__ void push_and_update(int *csrOffsets_d, int *csrCords_d, int *csrWeights_d, 
                                int *distances_d, int *parent_d, int *diffCsrOffsets_d, 
                                int *diffCsrCords_d, int *diffCsrWeights_d, int *locks_d, 
                                int numVertices, bool *modifiedA_d, bool *finished_d);


// main function
// usage: ./a.out inputFile updateFile [percentUpdate] [isDirected]
int main(int argc, char **argv) {
    // if input or update file names are not provided then exit
    if(argc < 3) {
        printf("Enter the input and update file path in the command line.\n");
        return 0;
    }

    // read file names
    char *inputFile = argv[1];
    char *updateFile = argv[2];
    
    // open input and update files
    FILE *inputFilePtr = fopen(inputFile, "r");
    FILE *updateFilePtr = fopen(updateFile, "r");
    
    // if not able to open the input file then exit
    if(inputFilePtr == NULL) {
        printf("Failed to open the input file.\n");
        return 0;
    }

    // if not able to open the update file then exit
    if(updateFilePtr == NULL) {
        printf("Failed to open the update file.\n");
        return 0;
    }

    // declaration of variables
    int numVertices, numEdges, startVertex;
    fscanf(inputFilePtr, "%d", &numVertices);
    fscanf(inputFilePtr, "%d", &numEdges);
    // fscanf(inputFilePtr, "%d", &startVertex);
    startVertex = 0;

    // to store the input graph in COO format
    std::vector<edgeInfo> COO(numEdges);

    // read from the input file and populate the COO
    for(int i=0; i<numEdges; i++) {
        int src, dest, weight;
        fscanf(inputFilePtr, "%d %d", &src, &dest);
        weight = 1;

        COO[i].src = src;
        COO[i].dest = dest;
        COO[i].weight = weight;
    }

    // close input file
    fclose(inputFilePtr);

    // sort the COO
    std::sort(COO.begin(), COO.end(), compareTwoEdges);

    // converting the graph in COO format to CSR format
    int *csrOffsets = (int*)malloc(sizeof(int)*(numVertices+1));
    int *csrCords = (int*)malloc(sizeof(int)*(numEdges));
    int *csrWeights = (int*)malloc(sizeof(int)*(numEdges));

    // initialize the Offsets array
    for(int i=0; i<=numVertices; i++) csrOffsets[i] = 0;

    // update the Coordinates and Weights array
    for(int i=0; i<numEdges; i++) {
        csrCords[i] = COO[i].dest;
        csrWeights[i] = COO[i].weight;
    }

    // update the Offsets array
    for(int i=0; i<numEdges; i++) csrOffsets[COO[i].src+1]++;		    // store the frequency
    for(int i=0; i<numVertices; i++) csrOffsets[i+1] += csrOffsets[i];	// do cumulative sum

    // sort the COO (for reverseCSR)
    std::sort(COO.begin(), COO.end(), compareTwoEdgesR);

    // converting the graph in COO format to reverseCSR format
    int *csrOffsetsR = (int*)malloc(sizeof(int)*(numVertices+1));
    int *csrCordsR = (int*)malloc(sizeof(int)*(numEdges));
    int *csrWeightsR = (int*)malloc(sizeof(int)*(numEdges));

    // initialize the Offsets array
    for(int i=0; i<=numVertices; i++) csrOffsetsR[i] = 0;

    // update the Coordinates and Weights array
    for(int i=0; i<numEdges; i++) {
        csrCordsR[i] = COO[i].src;
        csrWeightsR[i] = COO[i].weight;
    }

    // update the Offsets array
    for(int i=0; i<numEdges; i++) csrOffsetsR[COO[i].dest+1]++;		        // store the frequency
    for(int i=0; i<numVertices; i++) csrOffsetsR[i+1] += csrOffsetsR[i];	// do cumulative sum

    std::vector<edgeInfo>().swap(COO);

    // converting the graph to CSRs done

    // get graph type
    bool isDirected = true;
    if(argc >= 5 && atoi(argv[4])==0) isDirected = false;

    // shortest distances from start vertex
    int *distances_gpu = (int*)malloc(sizeof(int)*numVertices);

    // parent array
    int *parent = (int*)malloc(sizeof(int)*numVertices);

    // compute the shortest paths
    unsigned long long gpuTotalPathSum = SSSP_GPU(numVertices, numEdges, csrOffsets, csrCords, csrWeights, 
                                         distances_gpu, parent, startVertex);
    printf("Initial graph: Dist=%llu\n", gpuTotalPathSum);

    // will be using Diff-CSR along with CSR for dynamic graph
    int numEdgesDiffCsr = 0;
    int numEdgesDiffCsrR = 0;
    int *diffCsrOffsets, *diffCsrCords, *diffCsrWeights;
    int *diffCsrOffsetsR, *diffCsrCordsR, *diffCsrWeightsR;
    diffCsrOffsets = (int*)malloc(sizeof(int)*(numVertices+1));
    diffCsrOffsetsR = (int*)malloc(sizeof(int)*(numVertices+1));
    diffCsrCords = NULL; diffCsrWeights = NULL;
    diffCsrCordsR = NULL; diffCsrWeightsR = NULL;
    for(int i=0; i<=numVertices; i++) {
        diffCsrOffsets[i] = 0;
        diffCsrOffsetsR[i] = 0;
    }
    
    // for measuring time for each update
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    // start updates
    int numUpdates, batchSize, percentUpdate;
    char type;
    int u, v, w;
    double totalTime = 0.0;
    
    percentUpdate = 10;
    numUpdates = percentUpdate * 0.01 * numEdges;
    if(argc >= 4) {
        percentUpdate = atoi(argv[3]);
        numUpdates = percentUpdate * 0.01 * numEdges;
    }
    if(!isDirected && numUpdates&1) numUpdates++;
    batchSize = numUpdates;
    updateInfo *updates = (updateInfo*) malloc(sizeof(updateInfo) * batchSize);
    for(int i=0; i < numUpdates; i+=batchSize) {
        for(int j=0; j<batchSize; j++) {
            fscanf(updateFilePtr, " %c", &type);
            fscanf(updateFilePtr, "%d", &u);
            fscanf(updateFilePtr, "%d", &v);
            w = 1;
            
            updates[j] = {type, u, v, w};
            if(!isDirected) updates[++j] = {type, v, u, w};
        }

        // call the function to process the updates for current batch
        start = std::chrono::high_resolution_clock::now();
        processUpdates(numVertices, numEdges, distances_gpu, parent, updates, batchSize,
                       csrOffsets, csrCords, csrWeights, csrOffsetsR, csrCordsR, csrWeightsR, 
                       numEdgesDiffCsr, diffCsrOffsets, diffCsrCords, diffCsrWeights, 
                       numEdgesDiffCsrR, diffCsrOffsetsR, diffCsrCordsR, diffCsrWeightsR);
        end = std::chrono::high_resolution_clock::now();

        // for measuring total time taken for updates
        std::chrono::duration<double, std::milli> timeTaken = end-start;
        totalTime += timeTaken.count();
        printf("Total time taken for %d percent updates: %.3f ms\n", percentUpdate, totalTime);
    }
    
    gpuTotalPathSum = 0;
    for(int i=0; i<numVertices; i++) {
        if(distances_gpu[i] != MAX_INT)
            gpuTotalPathSum += distances_gpu[i];
    }
    printf("Final graph: Dist=%llu\n\n", gpuTotalPathSum);

    // free memory allocated on host
    free(csrOffsets);
    free(csrCords);
    free(csrWeights);
    free(csrOffsetsR);
    free(csrCordsR);
    free(csrWeightsR);
    free(diffCsrOffsets);
    free(diffCsrCords);
    free(diffCsrWeights);
    free(diffCsrOffsetsR);
    free(diffCsrCordsR);
    free(diffCsrWeightsR);
    free(distances_gpu);
    free(parent);
    free(updates);

    // close files
    fclose(updateFilePtr);

    return 0;
}

// comparator function
bool compareTwoEdges(const edgeInfo &a, const edgeInfo &b) {
    if(a.src != b.src) return a.src < b.src;
    return a.dest < b.dest;
}

// comparator function for reverse CSR
bool compareTwoEdgesR(const edgeInfo &a, const edgeInfo &b) {
    if(a.dest != b.dest) return a.dest < b.dest;
    return a.src < b.src;
}

// host function to print an array content
void printArray(int *arr, int len) {
    for(int i=0; i<len; i++) {
        printf("%d ", arr[i]);
    } printf("\n");
}

// check for cudaError
void checkCudaError() {
    cudaError_t error = cudaGetLastError();
    if(error != cudaSuccess) {
        printf("CUDA error: %s\n", cudaGetErrorString(error));
    }
}

// host function for parallel bellman ford (fixed point)
unsigned long long SSSP_GPU(int numVertices, int numEdges, int *csrOffsets, int *csrCords, int *csrWeights, 
                            int *distances, int *parent, int source=0) {
    // launch config
    const int numThreads = 1024;
    const int numBlocksV = (numVertices+numThreads-1)/numThreads;
    // const numBlocksE = (numEdges+numOfThreads-1)/numThreads;

    // pointers for arrays on host
    bool *modified = (bool*)malloc(sizeof(bool)*numVertices);
    bool *finished = (bool*)malloc(sizeof(bool));
    
    // pointers for arrays on device
    int *csrOffsets_d, *csrCords_d, *csrWeights_d;
    int *distances_d, *parent_d, *locks_d;
    bool *modified_d, *modified_next_d, *finished_d;

    // allocate memory on device
    cudaMalloc(&csrOffsets_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&csrCords_d, sizeof(int)*(numEdges));
    cudaMalloc(&csrWeights_d, sizeof(int)*(numEdges));
    cudaMalloc(&distances_d, sizeof(int)*numVertices);
    cudaMalloc(&parent_d, sizeof(int)*numVertices);
    cudaMalloc(&locks_d, sizeof(int)*numVertices);
    cudaMalloc(&modified_d, sizeof(bool)*numVertices);
    cudaMalloc(&modified_next_d, sizeof(bool)*numVertices);
    cudaMalloc(&finished_d, sizeof(bool));

    // initialize the host arrays
    for(int i=0; i<numVertices; i++) {
        distances[i] = MAX_INT;
        parent[i] = -1;
        modified[i] = false;
    }
    distances[source] = 0;
    modified[source] = true;
    *finished = false;

    // for recording the total time taken
    float milliseconds = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // copy to device
    cudaMemcpy(csrOffsets_d, csrOffsets, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(csrCords_d, csrCords, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(csrWeights_d, csrWeights, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(distances_d, distances, sizeof(int)*(numVertices), cudaMemcpyHostToDevice);
    cudaMemcpy(parent_d, parent, sizeof(int)*(numVertices), cudaMemcpyHostToDevice);
    cudaMemcpy(modified_d, modified, sizeof(bool)*(numVertices), cudaMemcpyHostToDevice);

    // call kernel to compute edge relaxing till no more updates or at max "numVertices-1" times
    int iter = 0;
    init_kernel<bool><<<numBlocksV, numThreads>>>(modified_next_d, false, numVertices);
    init_kernel<int><<<numBlocksV, numThreads>>>(locks_d, 0, numVertices);
    while(*finished != true) {
        init_kernel<bool><<<1, 1>>>(finished_d, true, 1);
        sssp_kernel<<<numBlocksV, numThreads>>>(csrOffsets_d, csrCords_d, csrWeights_d, distances_d, parent_d, 
                                                locks_d, numVertices, modified_d, modified_next_d, finished_d);
        init_kernel<bool><<<numBlocksV, numThreads>>>(modified_d, false, numVertices);
        cudaMemcpy(finished, finished_d, sizeof(bool), cudaMemcpyDeviceToHost);

        bool *tempPtr = modified_next_d;
        modified_next_d = modified_d;
        modified_d = tempPtr;

        if(++iter >= numVertices-1) break;
    }

    // check for error
    checkCudaError();
    
    // copy distances back to host
    cudaMemcpy(distances, distances_d, sizeof(int)*(numVertices), cudaMemcpyDeviceToHost);

    // copy parent array back to host
    cudaMemcpy(parent, parent_d, sizeof(int)*(numVertices), cudaMemcpyDeviceToHost);

    // print time taken
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Static Graph: SSSP using GPU \nTime Taken: %.6f ms \nIterations: %d\n", milliseconds, iter);

    // free up the memory
    free(modified);
    free(finished);
    cudaFree(csrOffsets_d);
    cudaFree(csrCords_d);
    cudaFree(csrWeights_d);
    cudaFree(distances_d);
    cudaFree(parent_d);
    cudaFree(locks_d);
    cudaFree(modified_d);
    cudaFree(modified_next_d);
    cudaFree(finished_d);

    unsigned long long sum = 0;
    for(int i=0; i<numVertices; i++) {
        if(distances[i] != MAX_INT)
            sum += distances[i];
    }
    return sum;
}

// process updates (in batches)
void processUpdates(int numVertices, int numEdges, int *distances, int *parent, updateInfo *&updates, 
                    int batchSize, int *csrOffsets, int *csrCords, int *csrWeights, 
                    int *csrOffsetsR, int *csrCordsR, int *csrWeightsR, 
                    int &numEdgesDiffCsr, int *&diffCsrOffsets, int *&diffCsrCords, int *&diffCsrWeights, 
                    int &numEdgesDiffCsrR, int *&diffCsrOffsetsR, int *&diffCsrCordsR, int *&diffCsrWeightsR) {
    // launch config
    const int numThreads = 1024;
    const int numBlocksB = (batchSize+numThreads-1)/numThreads;
    const int numBlocksV = (numVertices+numThreads-1)/numThreads;

    // pointers for arrays on host
    bool *finished = (bool*)malloc(sizeof(bool));
    
    // pointers for arrays on device
    int *distances_d, *parent_d, *locks_d;
    updateInfo *updates_d;
    int *csrOffsets_d, *csrCords_d, *csrWeights_d;
    int *csrOffsetsR_d, *csrCordsR_d, *csrWeightsR_d;
    int *diffCsrOffsets_d, *diffCsrCords_d, *diffCsrWeights_d;
    int *diffCsrOffsetsR_d, *diffCsrCordsR_d, *diffCsrWeightsR_d;
    bool *modifiedD_d, *modifiedA_d, *finished_d;

    // allocate memory on device
    cudaMalloc(&distances_d, sizeof(int)*numVertices);
    cudaMalloc(&parent_d, sizeof(int)*numVertices);
    cudaMalloc(&locks_d, sizeof(int)*numVertices);
    cudaMalloc(&updates_d, sizeof(updateInfo)*batchSize);
    cudaMalloc(&csrOffsets_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&csrCords_d, sizeof(int)*(numEdges));
    cudaMalloc(&csrWeights_d, sizeof(int)*(numEdges));
    cudaMalloc(&csrOffsetsR_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&csrCordsR_d, sizeof(int)*(numEdges));
    cudaMalloc(&csrWeightsR_d, sizeof(int)*(numEdges));
    cudaMalloc(&diffCsrOffsets_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&diffCsrCords_d, sizeof(int)*(numEdgesDiffCsr));
    cudaMalloc(&diffCsrWeights_d, sizeof(int)*(numEdgesDiffCsr));
    cudaMalloc(&diffCsrOffsetsR_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&diffCsrCordsR_d, sizeof(int)*(numEdgesDiffCsrR));
    cudaMalloc(&diffCsrWeightsR_d, sizeof(int)*(numEdgesDiffCsrR));
    cudaMalloc(&modifiedD_d, sizeof(bool)*numVertices);
    cudaMalloc(&modifiedA_d, sizeof(bool)*numVertices);
    cudaMalloc(&finished_d, sizeof(bool));

    // for recording the time taken
    float milliseconds = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // copy to device
    cudaMemcpy(distances_d, distances, sizeof(int)*(numVertices), cudaMemcpyHostToDevice);
    cudaMemcpy(parent_d, parent, sizeof(int)*(numVertices), cudaMemcpyHostToDevice);
    cudaMemcpy(updates_d, updates, sizeof(updateInfo)*(batchSize), cudaMemcpyHostToDevice);
    cudaMemcpy(csrOffsets_d, csrOffsets, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(csrCords_d, csrCords, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(csrWeights_d, csrWeights, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(csrOffsetsR_d, csrOffsetsR, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(csrCordsR_d, csrCordsR, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(csrWeightsR_d, csrWeightsR, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrOffsets_d, diffCsrOffsets, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrCords_d, diffCsrCords, sizeof(int)*(numEdgesDiffCsr), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrWeights_d, diffCsrWeights, sizeof(int)*(numEdgesDiffCsr), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrOffsetsR_d, diffCsrOffsetsR, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrCordsR_d, diffCsrCordsR, sizeof(int)*(numEdgesDiffCsrR), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrWeightsR_d, diffCsrWeightsR, sizeof(int)*(numEdgesDiffCsrR), cudaMemcpyHostToDevice);    
    init_kernel<bool><<<numBlocksV, numThreads>>>(modifiedD_d, false, numVertices);
    init_kernel<bool><<<numBlocksV, numThreads>>>(modifiedA_d, false, numVertices);

    // delete the edges from CSR & diffCSR and mark modified nodes
    delete_edges<<<numBlocksB, numThreads>>>(batchSize, distances_d, parent_d, updates_d, modifiedD_d,
                                             csrOffsets_d, csrCords_d, csrWeights_d,
                                             csrOffsetsR_d, csrCordsR_d, csrWeightsR_d,
                                             diffCsrOffsets_d, diffCsrCords_d, diffCsrWeights_d,
                                             diffCsrOffsetsR_d, diffCsrCordsR_d, diffCsrWeightsR_d);
    
    // mark the descendants of modified nodes as modified
    int iter = 0;
    *finished = false;
    while(*finished != true) {
        init_kernel<bool><<<1, 1>>>(finished_d, true, 1);
        mark_descendants<<<numBlocksV, numThreads>>>(distances_d, parent_d, modifiedD_d, 
                                                     numVertices, finished_d);
        cudaMemcpy(finished, finished_d, sizeof(bool), cudaMemcpyDeviceToHost);
        if(++iter >= numVertices-1) break;
    }

    // set the distance and parent of marked nodes (MAX_INT, -1)
    mark_not_reachable<<<numBlocksV, numThreads>>>(distances_d, parent_d, numVertices, modifiedD_d);
    
    // update the distances and parents (pull based approach)
    iter = 0;
    *finished = false;
    while(*finished != true) {
        init_kernel<bool><<<1, 1>>>(finished_d, true, 1);
        fetch_and_update<<<numBlocksV, numThreads>>>(csrOffsetsR_d, csrCordsR_d, csrWeightsR_d, 
                                                     diffCsrOffsetsR_d, diffCsrCordsR_d, diffCsrWeightsR_d, 
                                                     distances_d, parent_d, numVertices, modifiedD_d, finished_d);
        cudaMemcpy(finished, finished_d, sizeof(bool), cudaMemcpyDeviceToHost);
        if(++iter >= numVertices-1) break;
    }
    
    // new diffCSR
    int *diffCsrOffsets_next_d, *diffCsrCords_next_d, *diffCsrWeights_next_d;
    int *diffCsrOffsetsR_next_d, *diffCsrCordsR_next_d, *diffCsrWeightsR_next_d;
    cudaMalloc(&diffCsrOffsets_next_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&diffCsrOffsetsR_next_d, sizeof(int)*(numVertices+1));
    init_kernel<int><<<numBlocksV+1, numThreads>>>(diffCsrOffsets_next_d, 0, numVertices+1);
    init_kernel<int><<<numBlocksV+1, numThreads>>>(diffCsrOffsetsR_next_d, 0, numVertices+1);
    
    // add edges to available space in CSR
    add_edges_csr<<<numBlocksB, numThreads>>>(batchSize, distances_d, parent_d, updates_d, modifiedA_d,
                                              csrOffsets_d, csrCords_d, csrWeights_d,
                                              csrOffsetsR_d, csrCordsR_d, csrWeightsR_d,
                                              diffCsrOffsets_next_d, diffCsrOffsetsR_next_d);
    
    // get the count of valid edges in old diffCSR (for computing offsets for new diffCSR)
    add_diff_edge_counts<<<numBlocksV, numThreads>>>(numVertices, diffCsrOffsets_d, diffCsrCords_d, diffCsrWeights_d,
                                                     diffCsrOffsetsR_d, diffCsrCordsR_d, diffCsrWeightsR_d,
                                                     diffCsrOffsets_next_d, diffCsrOffsetsR_next_d);

    // compute offsets
    int *tempArr_d, *tempArrR_d;
    cudaMalloc(&tempArr_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&tempArrR_d, sizeof(int)*(numVertices+1));
    copy<int><<<numBlocksV+1, numThreads>>>(tempArr_d, diffCsrOffsets_next_d, numVertices+1);
    copy<int><<<numBlocksV+1, numThreads>>>(tempArrR_d, diffCsrOffsetsR_next_d, numVertices+1);
    
    for(int off=1; off<=numVertices; off*=2) {
        prefix_sum<<<numBlocksV+1, numThreads>>>(numVertices, off, diffCsrOffsets_next_d, 
                                                 diffCsrOffsetsR_next_d, tempArr_d, tempArrR_d);
        copy<int><<<numBlocksV+1, numThreads>>>(tempArr_d, diffCsrOffsets_next_d, numVertices+1);
        copy<int><<<numBlocksV+1, numThreads>>>(tempArrR_d, diffCsrOffsetsR_next_d, numVertices+1);
    }
    
    cudaFree(tempArr_d);
    cudaFree(tempArrR_d);
    
    // populate new diffCSR
    cudaMemcpy(&numEdgesDiffCsr, &diffCsrOffsets_next_d[numVertices], sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&numEdgesDiffCsrR, &diffCsrOffsetsR_next_d[numVertices], sizeof(int), cudaMemcpyDeviceToHost);
    cudaMalloc(&diffCsrCords_next_d, sizeof(int)*(numEdgesDiffCsr));
    cudaMalloc(&diffCsrWeights_next_d, sizeof(int)*(numEdgesDiffCsr));
    cudaMalloc(&diffCsrCordsR_next_d, sizeof(int)*(numEdgesDiffCsrR));
    cudaMalloc(&diffCsrWeightsR_next_d, sizeof(int)*(numEdgesDiffCsrR));
    init_kernel<int><<<((numEdgesDiffCsr)+numThreads)/numThreads, numThreads>>>(diffCsrWeights_next_d, MAX_INT, numEdgesDiffCsr);
    init_kernel<int><<<((numEdgesDiffCsrR)+numThreads)/numThreads, numThreads>>>(diffCsrWeightsR_next_d, MAX_INT, numEdgesDiffCsrR);
    
    // copy edges from previous diffCSR to new diffCSr
    copy_edges_diffcsr<<<numBlocksV, numThreads>>>(numVertices, diffCsrOffsets_d, diffCsrCords_d, diffCsrWeights_d, 
                                                   diffCsrOffsetsR_d, diffCsrCordsR_d, diffCsrWeightsR_d,
                                                   diffCsrOffsets_next_d, diffCsrCords_next_d, diffCsrWeights_next_d,
                                                   diffCsrOffsetsR_next_d, diffCsrCordsR_next_d, diffCsrWeightsR_next_d);

    cudaFree(diffCsrOffsets_d); diffCsrOffsets_d = diffCsrOffsets_next_d;
    cudaFree(diffCsrCords_d); diffCsrCords_d = diffCsrCords_next_d;
    cudaFree(diffCsrWeights_d); diffCsrWeights_d = diffCsrWeights_next_d;
    cudaFree(diffCsrOffsetsR_d); diffCsrOffsetsR_d = diffCsrOffsetsR_next_d;
    cudaFree(diffCsrCordsR_d); diffCsrCordsR_d = diffCsrCordsR_next_d;
    cudaFree(diffCsrWeightsR_d); diffCsrWeightsR_d = diffCsrWeightsR_next_d;

    // add remaining edges to new diffCSR
    add_edges_diffcsr<<<numBlocksB, numThreads>>>(batchSize, distances_d, parent_d, updates_d, modifiedA_d,
                                                  diffCsrOffsets_d, diffCsrCords_d, diffCsrWeights_d,
                                                  diffCsrOffsetsR_d, diffCsrCordsR_d, diffCsrWeightsR_d);

    // update the distances and parents (push based approach)
    *finished = false;
    iter = 0;
    init_kernel<int><<<numBlocksV, numThreads>>>(locks_d, 0, numVertices);
    while(*finished != true) {
        init_kernel<bool><<<1, 1>>>(finished_d, true, 1);
        push_and_update<<<numBlocksV, numThreads>>>(csrOffsets_d, csrCords_d, csrWeights_d, distances_d, parent_d, 
                                                    diffCsrOffsets_d, diffCsrCords_d, diffCsrWeights_d,
                                                    locks_d, numVertices, modifiedA_d, finished_d);
        cudaMemcpy(finished, finished_d, sizeof(bool), cudaMemcpyDeviceToHost);

        if(++iter >= numVertices-1) break;
    }
    
    // copy the arrays back to the host
    cudaMemcpy(distances, distances_d, sizeof(int)*(numVertices), cudaMemcpyDeviceToHost);
    cudaMemcpy(parent, parent_d, sizeof(int)*(numVertices), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrOffsets, csrOffsets_d, sizeof(int)*(numVertices+1), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrCords, csrCords_d, sizeof(int)*(numEdges), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrWeights, csrWeights_d, sizeof(int)*(numEdges), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrOffsetsR, csrOffsetsR_d, sizeof(int)*(numVertices+1), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrCordsR, csrCordsR_d, sizeof(int)*(numEdges), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrWeightsR, csrWeightsR_d, sizeof(int)*(numEdges), cudaMemcpyDeviceToHost);
    cudaMemcpy(diffCsrOffsets, diffCsrOffsets_d, sizeof(int)*(numVertices+1), cudaMemcpyDeviceToHost);
    diffCsrCords = (int*)realloc(diffCsrCords, sizeof(int)*(numEdgesDiffCsr));
    diffCsrWeights = (int*)realloc(diffCsrWeights, sizeof(int)*(numEdgesDiffCsr));
    cudaMemcpy(diffCsrCords, diffCsrCords_d, sizeof(int)*(numEdgesDiffCsr), cudaMemcpyDeviceToHost);
    cudaMemcpy(diffCsrWeights, diffCsrWeights_d, sizeof(int)*(numEdgesDiffCsr), cudaMemcpyDeviceToHost);
    cudaMemcpy(diffCsrOffsetsR, diffCsrOffsetsR_d, sizeof(int)*(numVertices+1), cudaMemcpyDeviceToHost);
    diffCsrCordsR = (int*)realloc(diffCsrCordsR, sizeof(int)*(numEdgesDiffCsrR));
    diffCsrWeightsR = (int*)realloc(diffCsrWeightsR, sizeof(int)*(numEdgesDiffCsrR));
    cudaMemcpy(diffCsrCordsR, diffCsrCordsR_d, sizeof(int)*(numEdgesDiffCsrR), cudaMemcpyDeviceToHost);
    cudaMemcpy(diffCsrWeightsR, diffCsrWeightsR_d, sizeof(int)*(numEdgesDiffCsrR), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    checkCudaError();
    
    // print the time taken
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Time Taken: %.6f ms\n", milliseconds);

    // free up the memory
    free(finished);
    cudaFree(distances_d);
    cudaFree(parent_d);
    cudaFree(locks_d);
    cudaFree(updates_d);
    cudaFree(csrOffsets_d);
    cudaFree(csrCords_d);
    cudaFree(csrWeights_d);
    cudaFree(csrOffsetsR_d);
    cudaFree(csrCordsR_d);
    cudaFree(csrWeightsR_d);
    cudaFree(diffCsrOffsets_d);
    cudaFree(diffCsrCords_d);
    cudaFree(diffCsrWeights_d);
    cudaFree(diffCsrOffsetsR_d);
    cudaFree(diffCsrCordsR_d);
    cudaFree(diffCsrWeightsR_d);
    cudaFree(modifiedD_d);
    cudaFree(modifiedA_d);
    cudaFree(finished_d);
}

// kernel for value initialization
template <typename T>
__global__ void init_kernel(T *array, T val, int arraySize) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < arraySize) array[id] = val;
}

// kernel for computing SSSP of static graph
__global__ void sssp_kernel(int *csrOffsets_d, int *csrCords_d, int *csrWeights_d, int *distances_d, 
                            int *parent_d, int *locks_d, int numVertices, bool *modified_d, 
                            bool *modified_next_d, bool *finished_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices && modified_d[id]==true && distances_d[id]!=MAX_INT) {
        int distToCurNode = distances_d[id];
        int v, newDist, lock;
        bool gotLock;
        for(int e=csrOffsets_d[id]; e<csrOffsets_d[id+1]; e++) {
            gotLock = false;
            v = csrCords_d[e];
            newDist = distToCurNode + csrWeights_d[e];
            do {
                if(gotLock==false) lock = atomicCAS(&locks_d[v], 0, 1);
                if(lock==0 && newDist < distances_d[v]) {
                    distances_d[v] = newDist;
                    parent_d[v] = id;
                    modified_next_d[v] = true;
                    *finished_d = false;
                }
                if(lock==0) {
                    gotLock = true;
                    lock = 1;
                    atomicExch(&locks_d[v], 0);
                }
            } while(gotLock == false);
        }
    }
}

// kernel for deleting edges from the graph (CSR and diffCSR)
__global__ void delete_edges(int batchSize, int *distances_d, int *parent_d, updateInfo *updates_d, 
                             bool *modifiedD_d, int *csrOffsets_d, int *csrCords_d, int *csrWeights_d,
                             int *csrOffsetsR_d, int *csrCordsR_d, int *csrWeightsR_d,
                             int *diffCsrOffsets_d, int *diffCsrCords_d, int *diffCsrWeights_d,
                             int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, int *diffCsrWeightsR_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type=='d') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        if(parent_d[v] >= 0 && parent_d[v]==u) {
            modifiedD_d[v] = true;
        }
        for(int i=csrOffsets_d[u]; i<csrOffsets_d[u+1]; i++) {
            if(csrCords_d[i]==v) {
                csrWeights_d[i] = MAX_INT;
                break;
            }
        }
        for(int i=csrOffsetsR_d[v]; i<csrOffsetsR_d[v+1]; i++) {
            if(csrCordsR_d[i]==u) {
                csrWeightsR_d[i] = MAX_INT;
                break;
            }
        }
        for(int i=diffCsrOffsets_d[u]; i<diffCsrOffsets_d[u+1]; i++) {
            if(diffCsrCords_d[i]==v) {
                diffCsrWeights_d[i] = MAX_INT;
                break;
            }
        }
        for(int i=diffCsrOffsetsR_d[v]; i<diffCsrOffsetsR_d[v+1]; i++) {
            if(diffCsrCordsR_d[i]==u) {
                diffCsrWeightsR_d[i] = MAX_INT;
                break;
            }
        }
    }
}

// kernel for marking all the descendants of modified nodes in SPT
__global__ void mark_descendants(int *distances_d, int *parent_d, bool *modifiedD_d, int numVertices,
                                 bool *finished_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices && modifiedD_d[id]==false) {
        int parent = parent_d[id];
        if(parent>=0 && modifiedD_d[parent]==true) {
            modifiedD_d[id] = true;
            *finished_d = false;
        }
    }
}

// kernel for setting distance and parent of marked nodes
__global__ void mark_not_reachable(int *distances_d, int *parent_d, int numVertices, bool *modifiedD_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices && modifiedD_d[id]==true) {
        distances_d[id] = MAX_INT;
        parent_d[id] = -1;
    }
}

// kernel for updating the distance and parent of marked nodes (as a result of deletions)
__global__ void fetch_and_update(int *csrOffsetsR_d, int *csrCordsR_d, int *csrWeightsR_d, 
                                 int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, int *diffCsrWeightsR_d, 
                                 int *distances_d, int *parent_d, int numVertices, 
                                 bool *modifiedD_d, bool *finished_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices && modifiedD_d[id]==true) {
        int u;
        for(int e=csrOffsetsR_d[id]; e<csrOffsetsR_d[id+1]; e++) {
            u = csrCordsR_d[e];
            if(distances_d[u] != MAX_INT && csrWeightsR_d[e] != MAX_INT) {
                if(distances_d[id] > distances_d[u]+csrWeightsR_d[e]) {
                    distances_d[id] = distances_d[u]+csrWeightsR_d[e];
                    parent_d[id] = u;
                    *finished_d = false;
                }
            }
        }
        for(int e=diffCsrOffsetsR_d[id]; e<diffCsrOffsetsR_d[id+1]; e++) {
            u = diffCsrCordsR_d[e];
            if(distances_d[u] != MAX_INT && diffCsrWeightsR_d[e] != MAX_INT) {
                if(distances_d[id] > distances_d[u]+diffCsrWeightsR_d[e]) {
                    distances_d[id] = distances_d[u]+diffCsrWeightsR_d[e];
                    parent_d[id] = u;
                    *finished_d = false;
                }
            }
        }
    }
}

// kernel for adding edges in to the CSR and marking modified nodes
__global__ void add_edges_csr(int batchSize, int *distances_d, int *parent_d, updateInfo *updates_d, 
                              bool *modifiedA_d, int *csrOffsets_d, int *csrCords_d, int *csrWeights_d,
                              int *csrOffsetsR_d, int *csrCordsR_d, int *csrWeightsR_d,
                              int *diffCsrOffsets_next_d, int *diffCsrOffsetsR_next_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type=='a') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        int w = updates_d[id].weight;
        int flag=0, flagR=0;
        if(distances_d[u]!=MAX_INT && distances_d[u]+w <= distances_d[v]) {
            modifiedA_d[u] = true;
        }
        for(int i=csrOffsets_d[u]; i<csrOffsets_d[u+1]; i++) {
            if(csrWeights_d[i]==MAX_INT) {
                flag = atomicCAS(&csrWeights_d[i], MAX_INT, w);
                if(flag==MAX_INT) {
                    updates_d[id].type='x';
                    csrCords_d[i] = v;
                    break;
                }
            }
        }
        for(int i=csrOffsetsR_d[v]; i<csrOffsetsR_d[v+1]; i++) {
            if(csrWeightsR_d[i]==MAX_INT) {
                flagR = atomicCAS(&csrWeightsR_d[i], MAX_INT, w);
                if(flagR==MAX_INT) {
                    if(updates_d[id].type=='a') updates_d[id].type='y';
                    else updates_d[id].type='z';
                    csrCordsR_d[i] = u;
                    break;
                }
            }
        }

        if(flag!=MAX_INT) atomicAdd(&diffCsrOffsets_next_d[u+1], 1);
        if(flagR!=MAX_INT) atomicAdd(&diffCsrOffsetsR_next_d[v+1], 1);
    }
}

// kernel for counting valid edges in diffCSR (for updating offset values)
__global__ void add_diff_edge_counts(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                     int *diffCsrWeights_d, int *diffCsrOffsetsR_d, int *diffCsrCordsR_d,
                                     int *diffCsrWeightsR_d, int *diffCsrOffsets_next_d, 
                                     int *diffCsrOffsetsR_next_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices) {
        for(int i=diffCsrOffsets_d[id]; i<diffCsrOffsets_d[id+1]; i++) {
            if(diffCsrWeights_d[i]!=MAX_INT) diffCsrOffsets_next_d[id+1] += 1;
        }
        for(int i=diffCsrOffsetsR_d[id]; i<diffCsrOffsetsR_d[id+1]; i++) {
            if(diffCsrWeightsR_d[i]!=MAX_INT) diffCsrOffsetsR_next_d[id+1] += 1;
        }
    }
}

// kernel for copying values from one array to another array
template <typename T>
__global__ void copy(T *destArr, T *srcArr, int arraySize) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < arraySize) destArr[id] = srcArr[id];
}

// kernel for prefix sum
__global__ void prefix_sum(int numVertices, int off, int *diffCsrOffsets_next_d, 
                           int *diffCsrOffsetsR_next_d, int *tempArr_d, int *tempArrR_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if((id <= numVertices) && (id >= off)) {
        diffCsrOffsets_next_d[id] += tempArr_d[id-off];
        diffCsrOffsetsR_next_d[id] += tempArrR_d[id-off];
    }
}

// kernel for copying the edges from old diffCSR to new diffCSR arrays
__global__ void copy_edges_diffcsr(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, int *diffCsrWeights_d, 
                                   int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, int *diffCsrWeightsR_d,
                                   int *diffCsrOffsets_next_d, int *diffCsrCords_next_d, int *diffCsrWeights_next_d,
                                   int *diffCsrOffsetsR_next_d, int *diffCsrCordsR_next_d, int *diffCsrWeightsR_next_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices) {
        int j=diffCsrOffsets_next_d[id];
        for(int i=diffCsrOffsets_d[id]; i<diffCsrOffsets_d[id+1]; i++) {
            if(diffCsrWeights_d[i]!=MAX_INT) {
                diffCsrCords_next_d[j] = diffCsrCords_d[i];
                diffCsrWeights_next_d[j] = diffCsrWeights_d[i];
                j++;
            }
        }
        j=diffCsrOffsetsR_next_d[id];
        for(int i=diffCsrOffsetsR_d[id]; i<diffCsrOffsetsR_d[id+1]; i++) {
            if(diffCsrWeightsR_d[i]!=MAX_INT) {
                diffCsrCordsR_next_d[j] = diffCsrCordsR_d[i];
                diffCsrWeightsR_next_d[j] = diffCsrWeightsR_d[i];
                j++;
            }
        }
    }
}

// kernel for adding remaining edges (from udpates) to diffCSR
__global__ void add_edges_diffcsr(int batchSize, int *distances_d, int *parent_d, updateInfo *updates_d, bool *modifiedA_d, 
                                  int *diffCsrOffsets_d, int *diffCsrCords_d, int *diffCsrWeights_d,
                                  int *diffCsrOffsetsR_d, int *diffCsrCordsR_d, int *diffCsrWeightsR_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && (updates_d[id].type=='a' || updates_d[id].type=='x' || updates_d[id].type=='y')) {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        int w = updates_d[id].weight;
        if(distances_d[u]!=MAX_INT && distances_d[u]+w <= distances_d[v]) {
            modifiedA_d[u] = true;
        }
        if(updates_d[id].type=='a' || updates_d[id].type=='y') {
            for(int i=diffCsrOffsets_d[u]; i<diffCsrOffsets_d[u+1]; i++) {
                if(diffCsrWeights_d[i]==MAX_INT) {
                    if(atomicCAS(&diffCsrWeights_d[i], MAX_INT, w)==MAX_INT) {
                        diffCsrCords_d[i] = v;
                        break;
                    }
                }
            }
        }
        if(updates_d[id].type=='a' || updates_d[id].type=='x') {
            for(int i=diffCsrOffsetsR_d[v]; i<diffCsrOffsetsR_d[v+1]; i++) {
                if(diffCsrWeightsR_d[i]==MAX_INT) {
                    if(atomicCAS(&diffCsrWeightsR_d[i], MAX_INT, w)==MAX_INT) {
                        diffCsrCordsR_d[i] = u;
                        break;
                    }
                }
            }
        }
    }
}

// kernel for updating the distance and parent of marked nodes (as a result of additions)
__global__ void push_and_update(int *csrOffsets_d, int *csrCords_d, int *csrWeights_d, int *distances_d, int *parent_d,
                                int *diffCsrOffsets_d, int *diffCsrCords_d, int *diffCsrWeights_d,
                                int *locks_d, int numVertices, bool *modifiedA_d, bool *finished_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices && modifiedA_d[id]==true && distances_d[id]!=MAX_INT) {
        int distToCurNode = distances_d[id];
        int v, newDist, lock;
        bool gotLock;
        for(int e=csrOffsets_d[id]; e<csrOffsets_d[id+1]; e++) {
            if(csrWeights_d[e] != MAX_INT) {
                gotLock = false;
                v = csrCords_d[e];
                newDist = distToCurNode + csrWeights_d[e];
                do {
                    if(gotLock==false) lock = atomicCAS(&locks_d[v], 0, 1);
                    if(lock==0 && newDist < distances_d[v]) {
                        distances_d[v] = newDist;
                        parent_d[v] = id;
                        modifiedA_d[v] = true;
                        *finished_d = false;
                    }
                    if(lock==0) {
                        gotLock = true;
                        lock = 1;
                        atomicExch(&locks_d[v], 0);
                    }
                } while(gotLock == false);
            }
        }
        for(int e=diffCsrOffsets_d[id]; e<diffCsrOffsets_d[id+1]; e++) {
            if(diffCsrWeights_d[e] != MAX_INT) {
                gotLock = false;
                v = diffCsrCords_d[e];
                newDist = distToCurNode + diffCsrWeights_d[e];
                do {
                    if(gotLock==false) lock = atomicCAS(&locks_d[v], 0, 1);
                    if(lock==0 && newDist < distances_d[v]) {
                        distances_d[v] = newDist;
                        parent_d[v] = id;
                        modifiedA_d[v] = true;
                        *finished_d = false;
                    }
                    if(lock==0) {
                        gotLock = true;
                        lock = 1;
                        atomicExch(&locks_d[v], 0);
                    }
                } while(gotLock == false);
            }
        }
    }
}