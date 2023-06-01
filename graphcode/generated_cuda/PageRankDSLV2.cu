// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "PageRankDSLV2.h"

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
  float* d_pageRank_nxt;
  cudaMalloc(&d_pageRank_nxt, sizeof(float)*(V));

  float num_nodes = (float)g.num_nodes( ); // asst in .cu

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank,(float)1 / num_nodes);

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank_nxt,(float)0);

  int iterCount = 0; // asst in .cu

  float diff; // asst in .cu

  bool tempVar_0 = false; // asst in .cu

  do{
    if (tempVar_0){ // if filter begin 

    } // if filter end
    tempVar_0 = true;
    cudaMemcpyToSymbol(::delta, &delta, sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::num_nodes, &num_nodes, sizeof(float), 0, cudaMemcpyHostToDevice);
    Compute_PR_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_src,d_rev_meta,d_pageRank,d_pageRank_nxt);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    cudaMemcpy(d_pageRank, d_pageRank_nxt, sizeof(float)*V, cudaMemcpyDeviceToDevice);
    iterCount++;

  }while((diff > beta) && (iterCount < maxIter));

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_pageRank_nxt);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
