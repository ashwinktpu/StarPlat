// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "APFB_V1.h"

void APFB(graph& g,int nc)

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
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
  unsigned numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  int* d_rmatch;
  cudaMalloc(&d_rmatch, sizeof(int)*(V));

  int* d_cmatch;
  cudaMalloc(&d_cmatch, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_rmatch,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_cmatch,(int)-1);

  bool noNewPaths = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  while(!noNewPaths) {

    noNewPaths = true;
    cudaMemcpyToSymbol(::noNewPaths, &noNewPaths, sizeof(bool), 0, cudaMemcpyHostToDevice);
    int L0 = 0; // asst in .cu

    int bfsLevel = L0; // asst in .cu

    int* d_bfsArray;
    cudaMalloc(&d_bfsArray, sizeof(int)*(V));

    int NOT_VISITED = L0 - 1; // asst in .cu

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_bfsArray,(int)NOT_VISITED);

    cudaMemcpyToSymbol(::L0, &L0, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
    APFB_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_cmatch,d_bfsArray;
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&L0, ::L0, sizeof(int), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);



      int* d_predeccesor;
      cudaMalloc(&d_predeccesor, sizeof(int)*(V));

      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_predeccesor,(int)-1);

      bool noNewVertices = false; // asst in .cu

      // FIXED POINT variables
      //BEGIN FIXED POINT
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      while(!noNewVertices) {

        noNewVertices = true;
        cudaMemcpyToSymbol(::noNewVertices, &noNewVertices, sizeof(bool), 0, cudaMemcpyHostToDevice);
        cudaMemcpyToSymbol(::NOT_VISITED, &NOT_VISITED, sizeof(int), 0, cudaMemcpyHostToDevice);
        cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
        cudaMemcpyToSymbol(::bfsLevel, &bfsLevel, sizeof(int), 0, cudaMemcpyHostToDevice);
        cudaMemcpyToSymbol(::noNewPaths, &noNewPaths, sizeof(bool), 0, cudaMemcpyHostToDevice);
        cudaMemcpyToSymbol(::noNewVertices, &noNewVertices, sizeof(bool), 0, cudaMemcpyHostToDevice);
        APFB_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_bfsArray,d_rmatch,d_predeccesor;
          cudaDeviceSynchronize();
          cudaMemcpyFromSymbol(&NOT_VISITED, ::NOT_VISITED, sizeof(int), 0, cudaMemcpyDeviceToHost);
          cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);
          cudaMemcpyFromSymbol(&bfsLevel, ::bfsLevel, sizeof(int), 0, cudaMemcpyDeviceToHost);
          cudaMemcpyFromSymbol(&noNewPaths, ::noNewPaths, sizeof(bool), 0, cudaMemcpyDeviceToHost);
          cudaMemcpyFromSymbol(&noNewVertices, ::noNewVertices, sizeof(bool), 0, cudaMemcpyDeviceToHost);



          ; // asst in .cu

          bfsLevel = bfsLevel + 1;

          cudaMemcpyFromSymbol(&noNewVertices, ::noNewVertices, sizeof(bool), 0, cudaMemcpyDeviceToHost);
          cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
          initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
        } // END FIXED POINT

        bool* d_compress;
        cudaMalloc(&d_compress, sizeof(bool)*(V));

        initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_compress,(bool)false);

        cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
        APFB_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_rmatch,d_compress;
          cudaDeviceSynchronize();
          cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);



          bool compressed = false; // asst in .cu

          // FIXED POINT variables
          //BEGIN FIXED POINT
          initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
          while(!compressed) {

            compressed = true;
            cudaMemcpyToSymbol(::compressed, &compressed, sizeof(bool), 0, cudaMemcpyHostToDevice);
            cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
            cudaMemcpyToSymbol(::compressed, &compressed, sizeof(bool), 0, cudaMemcpyHostToDevice);
            APFB_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_predeccesor,d_cmatch,d_compress,d_rmatch;
              cudaDeviceSynchronize();
              cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);
              cudaMemcpyFromSymbol(&compressed, ::compressed, sizeof(bool), 0, cudaMemcpyDeviceToHost);



              ; // asst in .cu

              ; // asst in .cu


              cudaMemcpyFromSymbol(&compressed, ::compressed, sizeof(bool), 0, cudaMemcpyDeviceToHost);
              cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
              initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
            } // END FIXED POINT

            cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
            APFB_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_rmatch,d_cmatch;
              cudaDeviceSynchronize();
              cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);



              ; // asst in .cu


              //cudaFree up!! all propVars in this BLOCK!
              cudaFree(d_compress);
              cudaFree(d_predeccesor);
              cudaFree(d_bfsArray);

              cudaMemcpyFromSymbol(&noNewPaths, ::noNewPaths, sizeof(bool), 0, cudaMemcpyDeviceToHost);
              cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
              initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
            } // END FIXED POINT


            //cudaFree up!! all propVars in this BLOCK!
            cudaFree(d_cmatch);
            cudaFree(d_rmatch);
            cudaFree(d_modified);

            //TIMER STOP
            cudaEventRecord(stop,0);
            cudaEventSynchronize(stop);
            cudaEventElapsedTime(&milliseconds, start, stop);
            printf("GPU Time: %.6f ms\n", milliseconds);

          } //end FUN
