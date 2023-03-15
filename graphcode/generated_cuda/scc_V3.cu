// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "scc_V3.h"

void Hong(graph& g)

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
  int* d_modified;
  cudaMalloc(&d_modified, sizeof(int)*(V));

  int* d_scc;
  cudaMalloc(&d_scc, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(int)false);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_scc,(int)-1);

  bool fpoint1 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  int k=0; // #fixpt-Iterations
  while(!fpoint1) {

    fpoint1 = true;
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Hong_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT

  int* d_visitFw;
  cudaMalloc(&d_visitFw, sizeof(int)*(V));

  int* d_visitBw;
  cudaMalloc(&d_visitBw, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_visitFw,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_visitBw,(int)-1);

  bool fpoint2 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  int k=0; // #fixpt-Iterations
  while(!fpoint2) {

    fpoint2 = true;
    cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Hong_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_visitBw,d_visitFw);
    cudaDeviceSynchronize();



    bool fpoint3 = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!fpoint3) {

      fpoint3 = true;
      cudaMemcpyToSymbol(::fpoint3, &fpoint3, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::fpoint3, &fpoint3, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Hong_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_visitFw,d_scc);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&fpoint3, ::fpoint3, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      cudaMemcpyToSymbol(::fpoint3, &fpoint3, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Hong_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_visitBw,d_scc);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&fpoint3, ::fpoint3, sizeof(bool), 0, cudaMemcpyDeviceToHost);




      cudaMemcpyFromSymbol(&fpoint3, ::fpoint3, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Hong_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_visitBw,d_visitFw);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    fpoint1 = false;
    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!fpoint1) {

      fpoint1 = true;
      cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Hong_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu


      cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT


    cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT


  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_visitFw);
  cudaFree(d_visitBw);
  cudaFree(d_scc);
  cudaFree(d_modified);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
