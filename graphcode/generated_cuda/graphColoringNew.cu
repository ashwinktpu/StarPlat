// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "graphColoringNew.h"

void colorGraph(graph& g)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;
  bool* d_modified_next;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_src, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));
  cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));
  cudaMalloc(&d_modified_next, sizeof(bool)*(V));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(   d_src,    h_src, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);
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

  //BEGIN DSL PARSING 
  int numNodes = g.num_nodes( ); // asst in .cu

  long* d_color;
  cudaMalloc(&d_color, sizeof(long)*(V));

  int* d_color1;
  cudaMalloc(&d_color1, sizeof(int)*(V));

  int* d_color2;
  cudaMalloc(&d_color2, sizeof(int)*(V));

  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  bool* d_modified_next;
  cudaMalloc(&d_modified_next, sizeof(bool)*(V));

  initKernel<long> <<<numBlocks,threadsPerBlock>>>(V,d_color,(long)0);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified_next,(bool)false);

  int fpoint1 = 0; // asst in .cu

  int diff = 0; // asst in .cu

  int diff_old = 0; // asst in .cu

  int cnt = 0; // asst in .cu

  int iter = 0; // asst in .cu

  do{
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(int), 0, cudaMemcpyHostToDevice);
    colorGraph_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_modified,d_color,d_modified_next);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(int), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    iter = iter + 1;
    diff_old = diff;
    diff = fpoint1 - iter;
    if (diff == diff_old){ // if filter begin 
      cnt = cnt + 1;

    } // if filter end
    else
    if (diff != diff_old){ // if filter begin 
      cnt = 0;

    } // if filter end
    if (cnt == 3){ // if filter begin 
      iter = iter + numNodes - fpoint1;
      fpoint1 = numNodes;

    } // if filter end

  }while(fpoint1 < numNodes);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_modified_next);
  cudaFree(d_modified);
  cudaFree(d_color2);
  cudaFree(d_color1);
  cudaFree(d_color);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
