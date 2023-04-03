// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "scc.h"

#include<bits/stdc++.h>
using namespace std;


void vHong(graph& g)

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
  int* d_modified;
  cudaMalloc(&d_modified, sizeof(int)*(V));

  int* d_outDeg;
  cudaMalloc(&d_outDeg, sizeof(int)*(V));

  int* d_inDeg;
  cudaMalloc(&d_inDeg, sizeof(int)*(V));

  bool* d_visitFw;
  cudaMalloc(&d_visitFw, sizeof(bool)*(V));

  bool* d_visitBw;
  cudaMalloc(&d_visitBw, sizeof(bool)*(V));

  bool* d_propFw;
  cudaMalloc(&d_propFw, sizeof(bool)*(V));

  bool* d_propBw;
  cudaMalloc(&d_propBw, sizeof(bool)*(V));

  bool* d_isPivot;
  cudaMalloc(&d_isPivot, sizeof(bool)*(V));

  int* d_scc;
  cudaMalloc(&d_scc, sizeof(int)*(V));

  int* d_range;
  cudaMalloc(&d_range, sizeof(int)*(V));

  int* d_pivotField;
  cudaMalloc(&d_pivotField, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(int)false);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_outDeg,(int)0);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_inDeg,(int)0);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitFw,(bool)false);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitBw,(bool)false);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propFw,(bool)false);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propBw,(bool)false);

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_isPivot,(bool)false);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_scc,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_range,(int)0);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_pivotField,(int)-1);

  vHong_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_outDeg,d_inDeg);
  cudaDeviceSynchronize();



  ; // asst in .cu

  bool fpoint1 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  while(!fpoint1) {

    fpoint1 = true;
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_range,d_isPivot);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  } // END FIXED POINT

    bool* h_scc1;
    h_scc1 = (bool *)malloc( (V)*sizeof(bool));
    cudaMemcpy(  h_scc1,   d_isPivot, sizeof(bool)*(V), cudaMemcpyDeviceToHost);
    int scc_cnt1 = 0;
    for(int i = 0; i < V; i++) {
      cout<<h_scc1[i]<<" ";
    }cout << endl;
    cout << "No.of strongly connected components in graph : " << scc_cnt1 << endl;


  vHong_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_inDeg,d_scc,d_outDeg,d_pivotField,d_range);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  vHong_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_pivotField,d_range,d_visitFw,d_visitBw,d_isPivot);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  bool fpoint2 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  while(!fpoint2) {

    fpoint2 = true;
    cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_visitBw,d_propFw,d_scc,d_propBw,d_range,d_visitFw);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  } // END FIXED POINT

  vHong_kernel_6<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_range,d_visitBw,d_visitFw,d_propBw,d_propFw);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  fpoint1 = false;
  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  while(!fpoint1) {

    fpoint1 = true;
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint1, &fpoint1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_range,d_isPivot);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint1, ::fpoint1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  } // END FIXED POINT

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_range,(int)0);

  vHong_kernel_8<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_range);
  cudaDeviceSynchronize();



  bool fpoint4 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  while(!fpoint4) {

    fpoint4 = true;
    cudaMemcpyToSymbol(::fpoint4, &fpoint4, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::fpoint4, &fpoint4, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_9<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_range);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint4, ::fpoint4, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    cudaMemcpyToSymbol(::fpoint4, &fpoint4, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_10<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_range);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint4, ::fpoint4, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint4, ::fpoint4, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  } // END FIXED POINT

  bool fpoint5 = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  while(!fpoint5) {

    fpoint5 = true;
    cudaMemcpyToSymbol(::fpoint5, &fpoint5, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_11<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_inDeg,d_scc,d_outDeg,d_pivotField,d_range);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    vHong_kernel_12<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_pivotField,d_range,d_visitFw,d_visitBw,d_isPivot);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    fpoint2 = false;
    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    while(!fpoint2) {

      fpoint2 = true;
      cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::fpoint2, &fpoint2, sizeof(bool), 0, cudaMemcpyHostToDevice);
      vHong_kernel_13<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_visitBw,d_propFw,d_scc,d_propBw,d_range,d_visitFw);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu


      cudaMemcpyFromSymbol(&fpoint2, ::fpoint2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    } // END FIXED POINT

    cudaMemcpyToSymbol(::fpoint5, &fpoint5, sizeof(bool), 0, cudaMemcpyHostToDevice);
    vHong_kernel_14<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_scc,d_range,d_visitBw,d_visitFw,d_propBw,d_propFw);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&fpoint5, ::fpoint5, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


    cudaMemcpyFromSymbol(&fpoint5, ::fpoint5, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  } // END FIXED POINT
bool* h_scc;
    h_scc = (bool *)malloc( (V)*sizeof(bool));
    cudaMemcpy(  h_scc,   d_isPivot, sizeof(bool)*(V), cudaMemcpyDeviceToHost);
    int scc_cnt = 0;
    for(int i = 0; i < V; i++) {
      cout<<h_scc[i]<<" ";
    }cout << endl;
    cout << "No.of strongly connected components in graph : " << scc_cnt << endl;


  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_range);
  cudaFree(d_propBw);
  cudaFree(d_visitFw);
  cudaFree(d_propFw);
  cudaFree(d_inDeg);
  cudaFree(d_pivotField);
  cudaFree(d_isPivot);
  cudaFree(d_outDeg);
  cudaFree(d_visitBw);
  cudaFree(d_scc);
  cudaFree(d_modified);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN





int main(int argc,char* argv[])
{
  char *file_name = argv[1];
  graph g(file_name);
  g.parseGraph();
  vHong(g);
  return 0;
}
