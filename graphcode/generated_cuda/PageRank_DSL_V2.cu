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
  float num_nodes = (float)g.num_nodes( ); // asst in .cu

  float* pageRank_nxt = (float*) malloc(sizeof(float)*V);
  float* d_pageRank_nxt;
  cudaMalloc(&d_pageRank_nxt, sizeof(float)*(V));

  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank,1 / num_nodes);

  int iterCount = 0; // asst in .cu

  float diff; // asst in .cu

  do
  {diff = 0.000000;
    cudaMemcpyToSymbol(::diff, &diff, sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::num_nodes, &num_nodes, sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::delta, &delta, sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpy(d_pageRank, pageRank, sizeof(float)*(V), cudaMemcpyHostToDevice);
    cudaMemcpy(d_pageRank_nxt, pageRank_nxt, sizeof(float)*(V), cudaMemcpyHostToDevice);
    Compute_PR_kernel<<<numBlocks, numThreads>>>(V,E,d_meta,d_data,d_weight,d_rev_meta,d_pageRank,d_pageRank_nxt);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&diff, ::diff, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&num_nodes, ::num_nodes, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&delta, ::delta, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
    cudaMemcpy(pageRank_nxt, d_pageRank_nxt, sizeof(float)*(V), cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    for (int node = 0; node < V; node ++) 
    {
      pageRank [node] = pageRank_nxt [node] ;
    }
    iterCount++;
  }while((diff > beta) && (iterCount < maxIter));//TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
