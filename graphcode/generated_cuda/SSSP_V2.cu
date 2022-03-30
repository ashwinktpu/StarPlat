// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "SSSP_V2.h"

void Compute_SSSP(graph& g,int* dist,int src)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_weight;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_weight;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);

  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (numThreads+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params
  int* d_dist;
  cudaMalloc(&d_dist, sizeof(int)*(V));


  //BEGIN DSL PARSING 
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_dist,INT_MAX);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,false);

  initIndex<bool><<<1,1>>>(V,d_modified,src,(bool)true); //InitIndexDevice
  initIndex<int><<<1,1>>>(V,d_dist,src,(int)0); //InitIndexDevice
  bool finished = false; // asst in .cu

  // FIXED POINT variables
  bool* d_finished; cudaMalloc(&d_finished,sizeof(bool)*(1));
  bool* d_modified_prev; cudaMalloc(&d_modified_prev,sizeof(bool)*(V));
  bool* d_modified_next; cudaMalloc(&d_modified_next,sizeof(bool)*(V));

  //BEGIN FIXED POINT
  int k=0; // #fixpt-Iterations
  while(!finished) {
    initIndex<bool> <<<1,1>>>(1,d_finished,0,true);
    Compute_SSSP_kernel<<<numBlocks, numThreads>>>(V,E,d_meta,d_data,d_weight,g,d_dist,src);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_prev, false);
    cudaMemcpy(&finished, d_finished, sizeof(bool)*(1), cudaMemcpyDeviceToHost);
    bool* tempModPtr = d_modified_next ; // SWAP next and prev ptrs
    d_modified_next = d_modified_prev;
    d_modified_prev = tempModPtr;
    k++;
  } // END FIXED POINT

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(    dist,   d_dist, sizeof(int)*(V), cudaMemcpyDeviceToHost);
} //end FUN
