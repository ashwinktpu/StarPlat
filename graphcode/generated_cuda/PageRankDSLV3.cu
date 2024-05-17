// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "PageRankDSLV3.h"

void ComputePageRank(graph& g,float beta,float delta,int maxIter,
  float* pageRank)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_src;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.srcList[i];
    h_src[i] = temp;
  }


  int* d_meta;
  int* d_src;
  int* d_rev_meta;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_src, sizeof(int)*(E));
  cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(   d_src,    h_src, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1)), cudaMemcpyHostToDevice);

  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


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
  float numNodes = (float)g.num_nodes( ); // asst in .cu

  float* d_pageRankNext;
  cudaMalloc(&d_pageRankNext, sizeof(float)*(V));

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank,(float)1 / numNodes);

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRankNext,(float)0);

  int iterCount = 0; // asst in .cu

  float diff; // asst in .cu

  do{
    diff = 0.000000;
    cudaMemcpyToSymbol(::diff, &diff, sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::delta, &delta, sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::numNodes, &numNodes, sizeof(float), 0, cudaMemcpyHostToDevice);
    ComputePageRank_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_src,d_rev_meta,d_pageRank);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&diff, ::diff, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&delta, ::delta, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&numNodes, ::numNodes, sizeof(float), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    cudaMemcpy(d_pageRank, d_pageRankNext, sizeof(float)*V, cudaMemcpyDeviceToDevice);
    iterCount++;

  }while((diff > beta) && (iterCount < maxIter));

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_pageRankNext);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
