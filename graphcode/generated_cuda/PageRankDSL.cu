#include "PageRankDSL.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)

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
  float num_nodes = (float)g.num_nodes( );
  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank,1 / num_nodes);

  int iterCount = 0;
  float diff = 0.0 ;
  do
  diff = 0.000000;
  unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
  unsigned int v =id;
  if (id < V)
  {iterCount++;
    while((diff > beta) && (iterCount < maxIter));//TIMER STOP
    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("GPU Time: %.6f ms\n", milliseconds);

    cudaMemcpy(BC,d_BC , sizeof(double) * (V), cudaMemcpyDeviceToHost);
  } //end FUN
