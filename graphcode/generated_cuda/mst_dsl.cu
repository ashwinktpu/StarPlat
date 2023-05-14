// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "mst_dsl.h"

void Boruvka(graph& g)

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
  unsigned numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int* d_nodeId;
  cudaMalloc(&d_nodeId, sizeof(int)*(V));

  int* d_color;
  cudaMalloc(&d_color, sizeof(int)*(V));

  int* d_color_next;
  cudaMalloc(&d_color_next, sizeof(int)*(V));

  bool* d_isMSTEdge;
  cudaMalloc(&d_isMSTEdge, sizeof(bool)*(E));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_nodeId,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_color,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_color_next,(int)-1);

  initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(E,d_isMSTEdge,(bool)false);

  Boruvka_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color_next,d_color,d_nodeId);
  cudaDeviceSynchronize();



  cudaMemcpy(d_color_next, d_color, sizeof(int)*V, cudaMemcpyDeviceToDevice);
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  bool noNewComp = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  // int k=0; // #fixpt-Iterations
  while(!noNewComp) {

    noNewComp = true;
    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    int* d_minEdge;
    cudaMalloc(&d_minEdge, sizeof(int)*(V));

    int* d_minEdge_next;
    cudaMalloc(&d_minEdge_next, sizeof(int)*(V));

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge,(int)-1);

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge_next,(int)-1);

    Boruvka_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdge,d_color);
    cudaDeviceSynchronize();



    ; // asst in .cu

    int* d_minEdgeOfComp;
    cudaMalloc(&d_minEdgeOfComp, sizeof(int)*(V));

    int* d_minEdgeOfComp_next;
    cudaMalloc(&d_minEdgeOfComp_next, sizeof(int)*(V));

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp,(int)-1);

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp_next,(int)-1);

    bool finishedMinEdge = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
   // int k=0; // #fixpt-Iterations
    while(!finishedMinEdge) {

      finishedMinEdge = true;
      cudaMemcpyToSymbol(::finishedMinEdge, &finishedMinEdge, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpy(d_minEdgeOfComp_next, d_minEdgeOfComp, sizeof(int)*V, cudaMemcpyDeviceToDevice);
      cudaMemcpyToSymbol(::finishedMinEdge, &finishedMinEdge, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Boruvka_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfComp_next);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finishedMinEdge, ::finishedMinEdge, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      cudaMemcpy(d_minEdgeOfComp, d_minEdgeOfComp_next, sizeof(int)*V, cudaMemcpyDeviceToDevice);

      cudaMemcpyFromSymbol(&finishedMinEdge, ::finishedMinEdge, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    } // END FIXED POINT

    Boruvka_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color,d_nodeId,d_minEdgeOfComp,d_minEdgeOfComp_next);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    cudaMemcpy(d_minEdgeOfComp, d_minEdgeOfComp_next, sizeof(int)*V, cudaMemcpyDeviceToDevice);
    Boruvka_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color,d_nodeId,d_minEdgeOfComp,d_isMSTEdge);
    cudaDeviceSynchronize();



    cudaMemcpy(d_color_next, d_color, sizeof(int)*V, cudaMemcpyDeviceToDevice);
    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Boruvka_kernel_6<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color,d_nodeId,d_minEdgeOfComp,d_color_next);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    cudaMemcpy(d_color, d_color_next, sizeof(int)*V, cudaMemcpyDeviceToDevice);
    bool finished = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!finished) {

      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpy(d_color_next, d_color, sizeof(int)*V, cudaMemcpyDeviceToDevice);
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Boruvka_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color,d_color_next);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu

      cudaMemcpy(d_color, d_color_next, sizeof(int)*V, cudaMemcpyDeviceToDevice);

      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    } // END FIXED POINT


    //cudaFree up!! all propVars in this BLOCK!
    cudaFree(d_minEdgeOfComp_next);
    cudaFree(d_minEdgeOfComp);
    cudaFree(d_minEdge_next);
    cudaFree(d_minEdge);

    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  } // END FIXED POINT


  //cudaFree up!! all propVars in this BLOCK!
  

  //TIMER STOP
  bool *h_isMSTEdge = (bool *)malloc(E * sizeof(bool));
  cudaMemcpy(h_isMSTEdge, d_isMSTEdge, E * sizeof(bool), cudaMemcpyDeviceToHost);
  int mst = 0;
  for (int i = 0; i < E; i++)
  {
    if (h_isMSTEdge[i]==true)
      mst += h_weight[i];
  }

  
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
  printf("mst=%d\n", mst);

  cudaFree(d_modified);
  cudaFree(d_isMSTEdge);
  cudaFree(d_color_next);
  cudaFree(d_color);
  cudaFree(d_nodeId);
}
int main(int argc, char *argv[])
{
  char *file_name = argv[1];
  graph g(file_name);
  g.parseGraph();
  Boruvka(g);
  return 0;
}
