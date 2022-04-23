// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "bc_dsl_v2.h"

void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet)

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
  float* d_BC;
  cudaMalloc(&d_BC, sizeof(float)*(V));


  //BEGIN DSL PARSING 
  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_BC,(float)0);

  //FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++) 
  {
    int src = *itr;
    double* d_sigma;
    cudaMalloc(&d_sigma, sizeof(double)*(V));

    float* d_delta;
    cudaMalloc(&d_delta, sizeof(float)*(V));

    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_delta,(float)0);

    initKernel<double> <<<numBlocks,threadsPerBlock>>>(V,d_sigma,(double)0);

    initIndex<double><<<1,1>>>(V,d_sigma,src,(double)1); //InitIndexDevice

    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    int hops_from_source=0;
    bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
    int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));  cudaMemset(d_hops_from_source,0,sizeof(int));
    int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

    //EXTRA vars INITIALIZATION
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_level,-1);
    initIndex<int><<<1,1>>>(V,d_level,src, 0);

    // long k =0 ;// For DEBUG
    do {
      finished = true;
      cudaMemcpy(d_finished, &finished, sizeof(bool)*(1), cudaMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,d_BC); ///DONE from varList

      incrementDeviceVar<<<1,1>>>(d_hops_from_source);
      cudaDeviceSynchronize(); //MUST - rupesh
      ++hops_from_source; // updating the level to process in the next iteration
      // k++; //DEBUG

      cudaMemcpy(&finished, d_finished, sizeof(bool)*(1), cudaMemcpyDeviceToHost);
    }while(!finished);

    hops_from_source--;
    cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);

    //BACKWARD PASS
    while(hops_from_source > 1) {

      //KERNEL Launch
      back_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished
        ,d_BC); ///DONE from varList

      hops_from_source--;
      cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);
    }
    //accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);

    //cudaFree up!! all propVars in this BLOCK!
    cudaFree(d_delta);
    cudaFree(d_sigma);

  }

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(      BC,     d_BC, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
