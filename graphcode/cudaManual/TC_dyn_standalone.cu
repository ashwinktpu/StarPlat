#include <vector>
#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#define MAX_INT 2147483647

// structure for storing edge information
struct edgeInfo {
    int src, dest;
};

// structure for update information
struct updateInfo {
    char type;
    int src, dest;
};

// HOST FUNCTIONS:
bool compareTwoEdges(const edgeInfo &a, const edgeInfo &b);
unsigned long long TC_GPU(int numVertices, int numEdges, 
                          int *csrOffsets, int *csrCords, bool isDirected);
void checkCudaError();
long long int processUpdates(updateInfo *updates, int batchSize, int numVertices, bool isDirected, 
                             int numEdges, int *csrOffsets, int *csrCords, 
                             int &numEdgesDiffCsr, int *&diffCsrOffsets, int *&diffCsrCords);

// DEVICE FUNCTIONS:
__global__ void tc_kernel(int numVertices, int *csrOffsets_d, int *csrCords_d, 
                          unsigned long long int *Tcount_d, bool isDirected);
template <typename T>
__global__ void init_kernel(T *array, T val, int arraySize);
__global__ void mark_del_modified(updateInfo *updates_d, int batchSize, 
                                  int *csrOffsets_d, int *csrCords_d, bool *modifiedCsr_d, 
                                  int *diffCsrOffsets_d, int *diffCsrCords_d, bool *modifiedDiffCsr_d);
__global__ void count_deleted_triangles(updateInfo *updates_d, int batchSize, unsigned long long int *count1_d,
                                        unsigned long long int *count2_d, unsigned long long int *count3_d, 
                                        int *csrOffsets_d, int *csrCords_d, bool *modifiedCsr_d,
                                        int *diffCsrOffsets_d, int *diffCsrCords_d, bool *modifiedDiffCsr_d);
__global__ void del_edges(int *csrCords_d, bool *modifiedCsr_d, int numEdges, 
                          int *diffCsrCords_d, bool *modifiedDiffCsr_d, int numEdgesDiffCsr);
__global__ void add_edges_csr(updateInfo *updates_d, int batchSize, int *csrOffsets_d, int *csrCords_d, 
                              bool *modifiedCsr_d, int *diffCsrOffsets_next_d);
__global__ void add_diff_edge_counts(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                     int *diffCsrOffsets_next_d);
template <typename T>
__global__ void copy(T *destArr, T *srcArr, int arraySize);
__global__ void prefix_sum(int numVertices, int off, int *diffCsrOffsets_next_d, int *tempArr_d);
__global__ void copy_edges_diffcsr(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                   int *diffCsrOffsets_next_d, int *diffCsrCords_next_d);
__global__ void add_edges_diffcsr(updateInfo *updates_d, int batchSize, int *diffCsrOffsets_d, 
                                  int *diffCsrCords_d, bool *modifiedDiffCsr_d);
__global__ void count_added_triangles(updateInfo *updates_d, int batchSize, unsigned long long int *count1_d,
                                      unsigned long long int *count2_d, unsigned long long int *count3_d, 
                                      int *csrOffsets_d, int *csrCords_d, bool *modifiedCsr_d,
                                      int *diffCsrOffsets_d, int *diffCsrCords_d, bool *modifiedDiffCsr_d);


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
    int numVertices, numEdges;
    fscanf(inputFilePtr, "%d", &numVertices);
    fscanf(inputFilePtr, "%d", &numEdges);

    // to store the input graph in COO format
    std::vector<edgeInfo> COO(numEdges);

    // read from the input file and populate the COO
    for(int i=0; i<numEdges; i++) {
        int src, dest;
        fscanf(inputFilePtr, "%d %d", &src, &dest);

        COO[i].src = src;
        COO[i].dest = dest;
    }

    // close input file
    fclose(inputFilePtr);

    // sort the COO
    std::sort(COO.begin(), COO.end(), compareTwoEdges);

    // converting the graph in COO format to CSR format
    int *csrOffsets = (int*)malloc(sizeof(int)*(numVertices+1));
    int *csrCords = (int*)malloc(sizeof(int)*(numEdges));

    // initialize the Offsets array
    for(int i=0; i<=numVertices; i++) csrOffsets[i] = 0;

    // update the Coordinates array
    for(int i=0; i<numEdges; i++) csrCords[i] = COO[i].dest;

    // update the Offsets array
    for(int i=0; i<numEdges; i++) csrOffsets[COO[i].src+1]++;		    // store the frequency
    for(int i=0; i<numVertices; i++) csrOffsets[i+1] += csrOffsets[i];	// do cumulative sum

    std::vector<edgeInfo>().swap(COO);

    // converting the graph to CSR done

    // get graph type
    bool isDirected = true;
    if(argc >= 5 && atoi(argv[4])==0) isDirected = false;

    // compute the number of triangles
    unsigned long long gpuTC = TC_GPU(numVertices, numEdges, csrOffsets, csrCords, isDirected);
    printf("Initial graph: TC=%llu\n", gpuTC);
    
    // will be using Diff-CSR along with CSR for dynamic graph
    int numEdgesDiffCsr = 0;
    int *diffCsrOffsets, *diffCsrCords;
    diffCsrOffsets = (int*)malloc(sizeof(int)*(numVertices+1));
    diffCsrCords = NULL; 
    for(int i=0; i<=numVertices; i++)
        diffCsrOffsets[i] = 0;
    
    // for measuring time for each update
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    // start updates
    int numUpdates, batchSize, percentUpdate;
    char type;
    int u, v;
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

            updates[j] = {type, u, v};
            if(!isDirected) updates[++j] = {type, v, u};
        }

        // call the function to process the updates for current batch
        start = std::chrono::high_resolution_clock::now();
        gpuTC += processUpdates(updates, batchSize, numVertices, isDirected, 
                                numEdges, csrOffsets, csrCords, 
                                numEdgesDiffCsr, diffCsrOffsets, diffCsrCords);
        end = std::chrono::high_resolution_clock::now();

        // for measuring total time taken for updates
        std::chrono::duration<double, std::milli> timeTaken = end-start;
        totalTime += timeTaken.count();
        printf("Total time taken for %d percent updates: %.3f ms\n", percentUpdate, totalTime);
    }
    
    printf("Final graph: TC=%llu\n\n", gpuTC);

    // free memory allocated on host
    free(csrOffsets);
    free(csrCords);
    free(diffCsrOffsets);
    free(diffCsrCords);
    free(updates);

    // close update file
    fclose(updateFilePtr);

    return 0;
}

// comparator function
bool compareTwoEdges(const edgeInfo &a, const edgeInfo &b) {
    if(a.src != b.src) return a.src < b.src;
    return a.dest < b.dest;
}

// Triangle Counting using GPU
unsigned long long TC_GPU(int numVertices, int numEdges, 
                          int *csrOffsets, int *csrCords, bool isDirected) {
    // launch config
    const int numThreads = 1024;
    const int numBlocksV = (numVertices+numThreads-1)/numThreads;

    // variables on host
    unsigned long long int Tcount = 0;
    
    // pointers for arrays on device
    int *csrOffsets_d, *csrCords_d;
    unsigned long long int *Tcount_d;

    // allocate memory on device
    cudaMalloc(&csrOffsets_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&csrCords_d, sizeof(int)*(numEdges));
    cudaMalloc(&Tcount_d, sizeof(unsigned long long int));

    // for recording the total time taken
    float milliseconds = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // copy to device
    cudaMemcpy(csrOffsets_d, csrOffsets, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(csrCords_d, csrCords, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(Tcount_d, &Tcount, sizeof(unsigned long long int), cudaMemcpyHostToDevice);

    // call kernel to compute the triangle counts
    tc_kernel<<<numBlocksV, numThreads>>>(numVertices, csrOffsets_d, csrCords_d, Tcount_d, isDirected);

    // check for error
    checkCudaError();
    
    // copy distances back to host
    cudaMemcpy(&Tcount, Tcount_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);

    // print time taken
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Initial graph, TC using GPU - time taken: %.3f ms \n", milliseconds);

    // free up the memory
    cudaFree(csrOffsets_d);
    cudaFree(csrCords_d);
    cudaFree(Tcount_d);
    
    return Tcount;
}

// check for cudaError
void checkCudaError() {
    cudaError_t error = cudaGetLastError();
    if(error != cudaSuccess) {
        printf("CUDA error: %s\n", cudaGetErrorString(error));
    }
}

// process updates (in batches)
long long int processUpdates(updateInfo *updates, int batchSize, int numVertices, bool isDirected,
                             int numEdges, int *csrOffsets, int *csrCords,
                             int &numEdgesDiffCsr, int *&diffCsrOffsets, int *&diffCsrCords) {
    // launch config
    const int numThreads = 1024;
    const int numBlocksB = (batchSize+numThreads-1)/numThreads;
    const int numBlocksV = (numVertices+numThreads-1)/numThreads;

    // number of triangles added or deleted as a result of this batch update
    long long int Tcount;
    unsigned long long int count1, count2, count3;
    Tcount = count1 = count2 = count3 = 0;

    // pointers for arrays on device
    updateInfo *updates_d;
    unsigned long long int *count1_d, *count2_d, *count3_d;
    int *csrOffsets_d, *csrCords_d;
    int *diffCsrOffsets_d, *diffCsrCords_d;
    bool *modifiedCsr_d, *modifiedDiffCsr_d;

    // allocate memory on device
    cudaMalloc(&updates_d, sizeof(updateInfo)*batchSize);
    cudaMalloc(&count1_d, sizeof(unsigned long long int));
    cudaMalloc(&count2_d, sizeof(unsigned long long int));
    cudaMalloc(&count3_d, sizeof(unsigned long long int));
    cudaMalloc(&csrOffsets_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&csrCords_d, sizeof(int)*(numEdges));
    cudaMalloc(&diffCsrOffsets_d, sizeof(int)*(numVertices+1));
    cudaMalloc(&diffCsrCords_d, sizeof(int)*(numEdgesDiffCsr));
    cudaMalloc(&modifiedCsr_d, sizeof(bool)*(numEdges));
    cudaMalloc(&modifiedDiffCsr_d, sizeof(bool)*(numEdgesDiffCsr));

    // for recording the time taken
    float milliseconds = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // copy to device
    cudaMemcpy(updates_d, updates, sizeof(updateInfo)*(batchSize), cudaMemcpyHostToDevice);
    cudaMemcpy(count1_d, &count1, sizeof(unsigned long long int), cudaMemcpyHostToDevice);
    cudaMemcpy(count2_d, &count2, sizeof(unsigned long long int), cudaMemcpyHostToDevice);
    cudaMemcpy(count3_d, &count3, sizeof(unsigned long long int), cudaMemcpyHostToDevice);
    cudaMemcpy(csrOffsets_d, csrOffsets, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(csrCords_d, csrCords, sizeof(int)*(numEdges), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrOffsets_d, diffCsrOffsets, sizeof(int)*(numVertices+1), cudaMemcpyHostToDevice);
    cudaMemcpy(diffCsrCords_d, diffCsrCords, sizeof(int)*(numEdgesDiffCsr), cudaMemcpyHostToDevice);
    init_kernel<bool><<<numBlocksV, numThreads>>>(modifiedCsr_d, false, numEdges);
    init_kernel<bool><<<numBlocksV, numThreads>>>(modifiedDiffCsr_d, false, numEdgesDiffCsr);

    // mark modified edges
    mark_del_modified<<<numBlocksB, numThreads>>>(updates_d, batchSize,
                                                  csrOffsets_d, csrCords_d, modifiedCsr_d,
                                                  diffCsrOffsets_d, diffCsrCords_d, modifiedDiffCsr_d);
    
    // count the number of deleted triangles
    count_deleted_triangles<<<numBlocksB, numThreads>>>(updates_d, batchSize, count1_d, count2_d, count3_d,
                                                        csrOffsets_d, csrCords_d, modifiedCsr_d,
                                                        diffCsrOffsets_d, diffCsrCords_d, modifiedDiffCsr_d);

    // kernel to delete the edges
    del_edges<<<numBlocksV, numThreads>>>(csrCords_d, modifiedCsr_d, numEdges, 
                                          diffCsrCords_d, modifiedDiffCsr_d, numEdgesDiffCsr);

    cudaMemcpy(&count1, count1_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&count2, count2_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&count3, count3_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);

    if(isDirected) Tcount -= (count1 + count2/2 + count3/3);
    else Tcount -= (count1 + count2/2 + count3/3)/2;
    count1 = count2 = count3 = 0;

    cudaMemcpy(count1_d, &count1, sizeof(unsigned long long int), cudaMemcpyHostToDevice);
    cudaMemcpy(count2_d, &count2, sizeof(unsigned long long int), cudaMemcpyHostToDevice);
    cudaMemcpy(count3_d, &count3, sizeof(unsigned long long int), cudaMemcpyHostToDevice);

    // new diffCSR
    int *diffCsrOffsets_next_d, *diffCsrCords_next_d;
    cudaMalloc(&diffCsrOffsets_next_d, sizeof(int)*(numVertices+1));
    init_kernel<int><<<numBlocksV+1, numThreads>>>(diffCsrOffsets_next_d, 0, numVertices+1);
    init_kernel<bool><<<numBlocksV, numThreads>>>(modifiedCsr_d, false, numEdges);
    
    // add edges to available space in CSR
    add_edges_csr<<<numBlocksB, numThreads>>>(updates_d, batchSize, csrOffsets_d, csrCords_d, 
                                              modifiedCsr_d, diffCsrOffsets_next_d);
    
    // get the count of valid edges in old diffCSR (for computing offsets for new diffCSR)
    add_diff_edge_counts<<<numBlocksV, numThreads>>>(numVertices, diffCsrOffsets_d, diffCsrCords_d, 
                                                     diffCsrOffsets_next_d);

    // compute offsets
    int *tempArr_d;
    cudaMalloc(&tempArr_d, sizeof(int)*(numVertices+1));
    copy<int><<<numBlocksV+1, numThreads>>>(tempArr_d, diffCsrOffsets_next_d, numVertices+1);
    
    for(int off=1; off<=numVertices; off*=2) {
        prefix_sum<<<numBlocksV+1, numThreads>>>(numVertices, off, diffCsrOffsets_next_d, tempArr_d);
        copy<int><<<numBlocksV+1, numThreads>>>(tempArr_d, diffCsrOffsets_next_d, numVertices+1);
    }
    cudaFree(tempArr_d);
    
    // populate new diffCSR
    cudaMemcpy(&numEdgesDiffCsr, &diffCsrOffsets_next_d[numVertices], sizeof(int), cudaMemcpyDeviceToHost);
    cudaMalloc(&diffCsrCords_next_d, sizeof(int)*(numEdgesDiffCsr));
    init_kernel<int><<<numBlocksV+1, numThreads>>>(diffCsrCords_next_d, MAX_INT, numEdgesDiffCsr);
    
    // copy edges from previous diffCSR to new diffCSr
    copy_edges_diffcsr<<<numBlocksV, numThreads>>>(numVertices, diffCsrOffsets_d, diffCsrCords_d, 
                                                   diffCsrOffsets_next_d, diffCsrCords_next_d);

    cudaFree(diffCsrOffsets_d); diffCsrOffsets_d = diffCsrOffsets_next_d;
    cudaFree(diffCsrCords_d); diffCsrCords_d = diffCsrCords_next_d;

    cudaFree(modifiedDiffCsr_d);
    cudaMalloc(&modifiedDiffCsr_d, sizeof(bool)*(numEdgesDiffCsr));
    init_kernel<bool><<<numBlocksV, numThreads>>>(modifiedDiffCsr_d, false, numEdgesDiffCsr);

    // add remaining edges to new diffCSR
    add_edges_diffcsr<<<numBlocksB, numThreads>>>(updates_d, batchSize, diffCsrOffsets_d, diffCsrCords_d, 
                                                  modifiedDiffCsr_d);

    // count the number of added triangles
    count_added_triangles<<<numBlocksB, numThreads>>>(updates_d, batchSize, count1_d, count2_d, count3_d, 
                                                      csrOffsets_d, csrCords_d, modifiedCsr_d,
                                                      diffCsrOffsets_d, diffCsrCords_d, modifiedDiffCsr_d);

    cudaMemcpy(&count1, count1_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&count2, count2_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&count3, count3_d, sizeof(unsigned long long int), cudaMemcpyDeviceToHost);

    if(isDirected) Tcount += (count1 + count2/2 + count3/3);
    else Tcount += (count1 + count2/2 + count3/3)/2;
    
    // copy the arrays back to the host
    cudaMemcpy(csrOffsets, csrOffsets_d, sizeof(int)*(numVertices+1), cudaMemcpyDeviceToHost);
    cudaMemcpy(csrCords, csrCords_d, sizeof(int)*(numEdges), cudaMemcpyDeviceToHost);
    cudaMemcpy(diffCsrOffsets, diffCsrOffsets_d, sizeof(int)*(numVertices+1), cudaMemcpyDeviceToHost);
    diffCsrCords = (int*)realloc(diffCsrCords, sizeof(int)*(numEdgesDiffCsr));
    cudaMemcpy(diffCsrCords, diffCsrCords_d, sizeof(int)*(numEdgesDiffCsr), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    checkCudaError();
    
    // print the time taken
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Final graph, TC using GPU - time taken: %.3f ms \n", milliseconds);

    // free up the memory
    cudaFree(updates_d);
    cudaFree(count1_d);
    cudaFree(count2_d);
    cudaFree(count3_d);
    cudaFree(csrOffsets_d);
    cudaFree(csrCords_d);
    cudaFree(diffCsrOffsets_d);
    cudaFree(diffCsrCords_d);
    cudaFree(modifiedCsr_d);
    cudaFree(modifiedDiffCsr_d);

    return Tcount;
}

// kernel for computing TC
// edges are u->v, v->w & w->u
__global__ void tc_kernel(int numVertices, int *csrOffsets_d, int *csrCords_d, 
                          unsigned long long int *Tcount_d, bool isDirected) {
    unsigned int u = blockDim.x*blockIdx.x + threadIdx.x;
    if(u < numVertices) {
        for(int idx1=csrOffsets_d[u]; idx1<csrOffsets_d[u+1]; idx1++) {
            int v = csrCords_d[idx1];
            if(u < v) {
                for(int idx2=csrOffsets_d[v]; idx2<csrOffsets_d[v+1]; idx2++) {
                    int w = csrCords_d[idx2];
                    if(v < w) {                
                        // check for w->u edge
                        for(int idx3=csrOffsets_d[w]; idx3<csrOffsets_d[w+1]; idx3++) {
                            if(csrCords_d[idx3]==u) atomicAdd(Tcount_d, 1);
                        }
                    }
                }
            }
        }
    }
    if(isDirected && u < numVertices) {
        for(int idx1=csrOffsets_d[u]; idx1<csrOffsets_d[u+1]; idx1++) {
            int v = csrCords_d[idx1];
            if(u > v) {
                for(int idx2=csrOffsets_d[v]; idx2<csrOffsets_d[v+1]; idx2++) {
                    int w = csrCords_d[idx2];
                    if(v > w) {                
                        // check for w->u edge
                        for(int idx3=csrOffsets_d[w]; idx3<csrOffsets_d[w+1]; idx3++) {
                            if(csrCords_d[idx3]==u) atomicAdd(Tcount_d, 1);
                        }
                    }
                }
            }
        }
    }
}

// kernel for value initialization
template <typename T>
__global__ void init_kernel(T *array, T val, int arraySize) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    unsigned int numThreads = blockDim.x*gridDim.x;
    if(id < arraySize) {
        while(id < arraySize) {
            array[id] = val;
            id += numThreads;
        }
    }
}

// kernel for marking modified (to be deleted) edges in the graph (CSR and diffCSR)
__global__ void mark_del_modified(updateInfo *updates_d, int batchSize, 
                                  int *csrOffsets_d, int *csrCords_d, bool *modifiedCsr_d, 
                                  int *diffCsrOffsets_d, int *diffCsrCords_d, bool *modifiedDiffCsr_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type=='d') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        for(int i=csrOffsets_d[u]; i<csrOffsets_d[u+1]; i++) {
            if(csrCords_d[i]==v) {
                modifiedCsr_d[i] = true;
                return;
            }
        }
        for(int i=diffCsrOffsets_d[u]; i<diffCsrOffsets_d[u+1]; i++) {
            if(diffCsrCords_d[i]==v) {
                modifiedDiffCsr_d[i] = true;
                return;
            }
        }
    }
}

// kernel for counting the number of deleted triangles
__global__ void count_deleted_triangles(updateInfo *updates_d, int batchSize, unsigned long long int *count1_d,
                                        unsigned long long int *count2_d, unsigned long long int *count3_d, 
                                        int *csrOffsets_d, int *csrCords_d, bool *modifiedCsr_d,
                                        int *diffCsrOffsets_d, int *diffCsrCords_d, bool *modifiedDiffCsr_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type=='d') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        if(u!=v) {
            int count = 1;
            for(int idx1=csrOffsets_d[v]; idx1<csrOffsets_d[v+1]; idx1++) {
                int w = csrCords_d[idx1];
                if(w==MAX_INT || w==u || w==v) continue;
                if(modifiedCsr_d[idx1]) count++;
                for(int idx2=csrOffsets_d[w]; idx2<csrOffsets_d[w+1]; idx2++) {
                    if(csrCords_d[idx2] == u) {
                        if(modifiedCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedCsr_d[idx2]) count--;
                    }
                }
                for(int idx2=diffCsrOffsets_d[w]; idx2<diffCsrOffsets_d[w+1]; idx2++) {
                    if(diffCsrCords_d[idx2] == u) {
                        if(modifiedDiffCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedDiffCsr_d[idx2]) count--;
                    }
                }
                if(modifiedCsr_d[idx1]) count--;
            }
            for(int idx1=diffCsrOffsets_d[v]; idx1<diffCsrOffsets_d[v+1]; idx1++) {
                int w = diffCsrCords_d[idx1];
                if(w==MAX_INT || w==u || w==v) continue;
                if(modifiedDiffCsr_d[idx1]) count++;
                for(int idx2=csrOffsets_d[w]; idx2<csrOffsets_d[w+1]; idx2++) {
                    if(csrCords_d[idx2] == u) {
                        if(modifiedCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedCsr_d[idx2]) count--;
                    }
                }
                for(int idx2=diffCsrOffsets_d[w]; idx2<diffCsrOffsets_d[w+1]; idx2++) {
                    if(diffCsrCords_d[idx2] == u) {
                        if(modifiedDiffCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedDiffCsr_d[idx2]) count--;
                    }
                }
                if(modifiedDiffCsr_d[idx1]) count--;
            }
        }
    }
}

// kernel to delete edges
__global__ void del_edges(int *csrCords_d, bool *modifiedCsr_d, int numEdges, 
                          int *diffCsrCords_d, bool *modifiedDiffCsr_d, int numEdgesDiffCsr) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    unsigned int numThreads = blockDim.x*gridDim.x;
    if(id < numEdges) {
        while(id < numEdges) {
            if(modifiedCsr_d[id]) csrCords_d[id] = MAX_INT;
            id += numThreads;
        }
    }
    id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < numEdgesDiffCsr) {
        while(id < numEdgesDiffCsr) {
            if(modifiedDiffCsr_d[id]) diffCsrCords_d[id] = MAX_INT;
            id += numThreads;
        }
    }
}

// kernel for adding edges in to the CSR and marking modified nodes
__global__ void add_edges_csr(updateInfo *updates_d, int batchSize, int *csrOffsets_d, int *csrCords_d, 
                              bool *modifiedCsr_d, int *diffCsrOffsets_next_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type=='a') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        int flag=0;
        for(int i=csrOffsets_d[u]; i<csrOffsets_d[u+1]; i++) {
            if(csrCords_d[i]==MAX_INT) {
                flag = atomicCAS(&csrCords_d[i], MAX_INT, v);
                if(flag==MAX_INT) {
                    updates_d[id].type='x';
                    modifiedCsr_d[i] = true;
                    break;
                }
            }
        }

        if(flag!=MAX_INT) atomicAdd(&diffCsrOffsets_next_d[u+1], 1);
    }
}

// kernel for counting valid edges in diffCSR (for updating offset values)
__global__ void add_diff_edge_counts(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                     int *diffCsrOffsets_next_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices) {
        for(int i=diffCsrOffsets_d[id]; i<diffCsrOffsets_d[id+1]; i++) {
            if(diffCsrCords_d[i]!=MAX_INT) diffCsrOffsets_next_d[id+1] += 1;
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
__global__ void prefix_sum(int numVertices, int off, int *diffCsrOffsets_next_d, int *tempArr_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if((id <= numVertices) && (id >= off)) {
        diffCsrOffsets_next_d[id] += tempArr_d[id-off];
    }
}

// kernel for copying the edges from old diffCSR to new diffCSR arrays
__global__ void copy_edges_diffcsr(int numVertices, int *diffCsrOffsets_d, int *diffCsrCords_d, 
                                   int *diffCsrOffsets_next_d, int *diffCsrCords_next_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id<numVertices) {
        int j=diffCsrOffsets_next_d[id];
        for(int i=diffCsrOffsets_d[id]; i<diffCsrOffsets_d[id+1]; i++) {
            if(diffCsrCords_d[i]!=MAX_INT) {
                diffCsrCords_next_d[j] = diffCsrCords_d[i];
                j++;
            }
        }
    }
}

// kernel for adding remaining edges (from udpates) to diffCSR
__global__ void add_edges_diffcsr(updateInfo *updates_d, int batchSize, int *diffCsrOffsets_d, 
                                  int *diffCsrCords_d, bool *modifiedDiffCsr_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type=='a') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        for(int i=diffCsrOffsets_d[u]; i<diffCsrOffsets_d[u+1]; i++) {
            if(diffCsrCords_d[i]==MAX_INT) {
                if(atomicCAS(&diffCsrCords_d[i], MAX_INT, v)==MAX_INT) {
                    updates_d[id].type='y';
                    modifiedDiffCsr_d[i] = true;
                    break;
                }
            }
        }
    }
}

// kernel for counting the number of newly formed triangles
__global__ void count_added_triangles(updateInfo *updates_d, int batchSize, unsigned long long int *count1_d,
                                      unsigned long long int *count2_d, unsigned long long int *count3_d, 
                                      int *csrOffsets_d, int *csrCords_d, bool *modifiedCsr_d,
                                      int *diffCsrOffsets_d, int *diffCsrCords_d, bool *modifiedDiffCsr_d) {
    unsigned int id = blockDim.x*blockIdx.x + threadIdx.x;
    if(id < batchSize && updates_d[id].type!='d') {
        int u = updates_d[id].src;
        int v = updates_d[id].dest;
        if(u!=v) {
            int count = 1;
            for(int idx1=csrOffsets_d[v]; idx1<csrOffsets_d[v+1]; idx1++) {
                int w = csrCords_d[idx1];
                if(w==MAX_INT || w==u || w==v) continue;
                if(modifiedCsr_d[idx1]) count++;
                for(int idx2=csrOffsets_d[w]; idx2<csrOffsets_d[w+1]; idx2++) {
                    if(csrCords_d[idx2] == u) {
                        if(modifiedCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedCsr_d[idx2]) count--;
                    }
                }
                for(int idx2=diffCsrOffsets_d[w]; idx2<diffCsrOffsets_d[w+1]; idx2++) {
                    if(diffCsrCords_d[idx2] == u) {
                        if(modifiedDiffCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedDiffCsr_d[idx2]) count--;
                    }
                }
                if(modifiedCsr_d[idx1]) count--;
            }
            for(int idx1=diffCsrOffsets_d[v]; idx1<diffCsrOffsets_d[v+1]; idx1++) {
                int w = diffCsrCords_d[idx1];
                if(w==MAX_INT || w==u || w==v) continue;
                if(modifiedDiffCsr_d[idx1]) count++;
                for(int idx2=csrOffsets_d[w]; idx2<csrOffsets_d[w+1]; idx2++) {
                    if(csrCords_d[idx2] == u) {
                        if(modifiedCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedCsr_d[idx2]) count--;
                    }
                }
                for(int idx2=diffCsrOffsets_d[w]; idx2<diffCsrOffsets_d[w+1]; idx2++) {
                    if(diffCsrCords_d[idx2] == u) {
                        if(modifiedDiffCsr_d[idx2]) count++;
                        if(count==1)      atomicAdd(count1_d, 1);
                        else if(count==2) atomicAdd(count2_d, 1);
                        else if(count==3) atomicAdd(count3_d, 1);
                        if(modifiedDiffCsr_d[idx2]) count--;
                    }
                }
                if(modifiedDiffCsr_d[idx1]) count--;
            }
        }
    }
}