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
  float* d_pageRank;
  cudaMalloc(&d_pageRank, sizeof(float)*(V));

  double* d_BC; cudaMalloc(&d_BC, sizeof(double)*(V)); ///TODO from func

  //BEGIN DSL PARSING 
  float* d_num_nodes; cudaMalloc(&d_num_nodes,sizeof(float)*(1));

  (float)initIndex<<<1,1>>>(1,d_num_nodes,0, 0);
  float* d_pageRank_nxt;
  cudaMalloc(&d_pageRank_nxt, sizeof(float)*(V));

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank,1 / num_nodes);

  int* d_iterCount; cudaMalloc(&d_iterCount,sizeof(int)*(1));

  initIndex<<<1,1>>>(1,d_iterCount,0, 0);
  float* d_diff; cudaMalloc(&d_diff,sizeof(float)*(1));

  initIndex<<<1,1>>>(1,d_diff,0, 0);
  do
  {diff = 0.000000;
    Compute_PR_kernel<<<numBlocks, numThreads>>>( V, E, d_meta, d_data, d_weight ,g,beta,delta,maxIter,
      d_pageRank);
    cudaDeviceSynchronize();

    float* d_sum; cudaMalloc(&d_sum,sizeof(float)*(1));

    float* d_val; cudaMalloc(&d_val,sizeof(float)*(1));

    pageRank = pageRank_nxt;
    iterCount++;
  }while((diff > beta) && (iterCount < maxIter));//TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
  cudaMemcpy(BC,d_BC , sizeof(double) * (V), cudaMemcpyDeviceToHost);
} //end FUN
