// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "MST_V2_Verified.h"
#include<bits/stdc++.h>
using namespace std;
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
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
  unsigned numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;

  bool* d_isMSTEdge;
  cudaMalloc(&d_isMSTEdge, sizeof(bool)*(E));

  initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(E,d_isMSTEdge,(bool)false);
  
 
  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  cudaEvent_t start1, stop1;
  cudaEventCreate(&start1);
  cudaEventCreate(&stop1);
  
  float milliseconds = 0;
  cudaEventRecord(start1,0);

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
   int* d_nodeId;
  cudaMalloc(&d_nodeId, sizeof(int)*(V));

  int* d_color;
  cudaMalloc(&d_color, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_nodeId,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_color,(int)-1);
  
  float time = 0;
  
  // cudaEventRecord(start, 0);  
  Boruvka_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color,d_nodeId,d_isMSTEdge);
  cudaDeviceSynchronize();
  // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
  // printf("Boruvka_kernel_1 %.3f\n", time);

  ; // asst in .cu

  // cudaEventRecord(start, 0);
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));
  
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  bool noNewComp = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
  // printf("d_modified malloc and init %.3f\n", time);
 
  while(!noNewComp) {
    // cudaEventRecord(start, 0);
    noNewComp = true;
    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    int* d_minEdge;
    cudaMalloc(&d_minEdge, sizeof(int)*(V));

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge,(int)-1);
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("d_minEdge malloc and init %.3f\n", time);


    // cudaEventRecord(start, 0);
    Boruvka_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdge,d_color,d_isMSTEdge);
    cudaDeviceSynchronize();
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("Boruvka Kernel 2 %.3f\n", time);


    // cudaEventRecord(start, 0);
    int* d_minEdgeOfComp;
    cudaMalloc(&d_minEdgeOfComp, sizeof(int)*(V));

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp,(int)-1);
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("minEdgeOfComp malloc and init %.3f\n", time);

    // cudaEventRecord(start, 0);
    bool finishedMinEdge = false; // asst in .cu
    
    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    while(!finishedMinEdge) {
      finishedMinEdge = true;
      cudaMemcpyToSymbol(::finishedMinEdge, &finishedMinEdge, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::finishedMinEdge, &finishedMinEdge, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Boruvka_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdge,d_minEdgeOfComp,d_color,d_isMSTEdge);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finishedMinEdge, ::finishedMinEdge, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&finishedMinEdge, ::finishedMinEdge, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    } // END FIXED POINT
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("Boruvka Kernel 3 %.3f\n", time);

    // cudaEventRecord(start, 0);
    Boruvka_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdgeOfComp,d_color,d_nodeId,d_isMSTEdge);
    cudaDeviceSynchronize();
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("Boruvka Kernel 4 %.3f\n", time);

    // cudaEventRecord(start, 0);
    Boruvka_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdgeOfComp,d_color,d_nodeId,d_isMSTEdge);
    cudaDeviceSynchronize();
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("Boruvka Kernel 5 %.3f\n", time);

    // cudaEventRecord(start, 0);
    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Boruvka_kernel_6<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_minEdgeOfComp,d_color,d_nodeId,d_isMSTEdge);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("Boruvka Kernel 6 %.3f\n", time);


    bool finished = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    while(!finished) {

      // cudaEventRecord(start, 0);
      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
      // printf("Single Iteration Symbol copy %.3f\n", time);     

      // cudaEventRecord(start, 0);
      Boruvka_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_color,d_isMSTEdge);
      cudaDeviceSynchronize();
      // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
      // printf("Single Iteration Propagate colors %.3f\n", time);
     
      // cudaEventRecord(start, 0);
      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu


      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
      // printf("Single Iteration Symbol copy %.3f\n", time);
  
    } // END FIXED POINT

    // cudaEventRecord(start, 0);
    //cudaFree up!! all propVars in this BLOCK!
    cudaFree(d_minEdgeOfComp);
    cudaFree(d_minEdge);

    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    // cudaEventRecord(stop, 0);  cudaEventSynchronize(stop);  cudaEventElapsedTime(&time, start, stop);
    // printf("Memcpy and initKernel %.3f\n", time);  
  } // END FIXED POINT

  //TIMER STOP
  cudaEventRecord(stop1,0); cudaEventSynchronize(stop1); cudaEventElapsedTime(&milliseconds, start1, stop1);
  printf("GPU Time: %.6f ms\n", milliseconds);

  bool* h_isMSTEdge = (bool *)malloc((E)*sizeof(bool));
  cudaMemcpy(h_isMSTEdge, d_isMSTEdge, E * sizeof(bool), cudaMemcpyDeviceToHost);

  long long mst = 0;
  for(int i = 0; i < E; i++){
    if(h_isMSTEdge[i] == true) mst += h_weight[i];
  }
  printf("MST Weight: %lld\n", mst);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_modified);
  cudaFree(d_isMSTEdge);
  cudaFree(d_color);
  cudaFree(d_nodeId);
} //end FUN

int main(int argc, char** argv) {
  time_t start, end;
	time(&start);
  char* inp = argv[1];
  bool isWeighted = atoi(argv[2]) ? true : false;
  printf("Taking input from: %s\n", inp);
  graph g(inp);
  g.parseGraph(isWeighted);
  Boruvka(g);
	time(&end);
	double time_taken = double(end - start);
  cout << "Time taken by program is : " << fixed
        << time_taken << setprecision(5);
  cout << " sec " << endl;
  return 0;
}