// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "PageRank_DSL_V2.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)

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
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

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

  for(int i=0; i<= V; i++) {
    int temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_weight;
  int* d_rev_meta;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));
  cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_rev_meta, h_rev_meta, sizeof(int)*(E), cudaMemcpyHostToDevice);

  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (numThreads+threadsPerBlock-1)/threadsPerBlock;

  // For PageRank delta, beta and maxIter values
  float beta = 0.001;
  float delta = 0.85;
  int maxIter = 100;

  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params
  float* d_pageRank;
  cudaMalloc(&d_pageRank, sizeof(float)*(V));


  //BEGIN DSL PARSING 
  float num_nodes(float) = g.num_nodes( ); // asst in .cu

  float* d_pageRank_nxt;
  cudaMalloc(&d_pageRank_nxt, sizeof(float)*(V));

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank,1 / num_nodes);

  int iterCount = 0; // asst in .cu

  float diff
  initIndex<<<1,1>>>(1,d_diff,0, 0);
  ; // asst in .cu

  float val
  initIndex<<<1,1>>>(1,d_val,0, 0);
  ; // asst in .cu

  do
  {diff = 0.000000;
    Compute_PR_kernel<<<numBlocks, numThreads>>>(V,E,d_meta,d_data,d_weight,g,beta,delta,maxIter,
      d_pageRank);
    ; // asst in .cu

    pageRank = pageRank_nxt;
    iterCount++;
  }while((diff > beta) && (iterCount < maxIter));//TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
