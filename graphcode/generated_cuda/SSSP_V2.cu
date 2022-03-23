#include "SSSP_V2.h"
#include "../graph.hpp"

void Compute_SSSP(graph& g,int* dist,int src)

{
  // CSR BEGIN
  unsigned V = g.num_nodes();
  unsigned E = g.num_edges();

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

  //END CSR 

  //DECLAR DEVICE AND HOST vars in params
  double* d_BC; cudaMalloc(&d_BC, sizeof(double)*(V)); ///TODO from func

  //BEGIN DSL PARSING 
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_dist,INT_MAX);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,false);

  initIndex<double><<<1,1>>>(V,d_modified,src,true);
  initIndex<double><<<1,1>>>(V,d_dist,src,0);
  bool finished = false;
  while ( !finished[0] )
  {
    finished[0] = true;
    {
      Compute_SSSP_kernel<<<num_blocks, block_size>>>(gpu_OA, gpu_edgeList, V, E );
      cudaDeviceSynchronize();

    }
     initKernel<bool> <<< 1, 1>>>(1, gpu_finished, true);
     Compute_SSSP_kernel<<<num_blocks , block_size>>>(gpu_OA,gpu_edgeList, gpu_edgeLen ,gpu_dist,src, V ,MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);
     initKernel<bool><<<num_blocks,block_size>>>(V, gpu_modified_prev, false);
     cudaMemcpy(finished, gpu_finished,  sizeof(bool) *(1), cudaMemcpyDeviceToHost);
    bool* tempModPtr = modified_nxt ;
    modified_nxt = modified_prev ;
    modified_prev = tempModPtr ;
    modified_nxt[v] = false ;
    //TIMER STOP
    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("GPU Time: %.6f ms\n", milliseconds);

    cudaMemcpy(BC,d_BC , sizeof(double) * (V), cudaMemcpyDeviceToHost);
  } //end FUN
