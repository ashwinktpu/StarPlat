// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "cc_dsl.h"

void Compute_CC(graph& g,float* CC)

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
  float* d_CC;
  cudaMalloc(&d_CC, sizeof(float)*(V));


  //BEGIN DSL PARSING 
  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_CC,(float)0);

  // int V = g.num_nodes( ); // asst in .cu

  int src = 0;
  do{
    int* d_dist;
    cudaMalloc(&d_dist, sizeof(int)*(V));

    bool* d_modified;
    cudaMalloc(&d_modified, sizeof(bool)*(V));

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_dist,(int)INT_MAX);

    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

    initIndex<bool><<<1,1>>>(V,d_modified,src,(bool)true); //InitIndexDevice
    initIndex<int><<<1,1>>>(V,d_dist,src,(int)0); //InitIndexDevice
    bool finished = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!finished) {

      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Compute_CC_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_modified,d_dist);
      cudaDeviceSynchronize();




      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    float temp = 0; // asst in .cu

    cudaMemcpyToSymbol(::temp, &temp, sizeof(float), 0, cudaMemcpyHostToDevice);
    Compute_CC_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next,d_dist);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&temp, ::temp, sizeof(float), 0, cudaMemcpyDeviceToHost);



    initIndex<float><<<1,1>>>(V,d_CC,src,(float)(1 / temp)); //InitIndexDevice
    src = src + 1;

    //cudaFree up!! all propVars in this BLOCK!
    cudaFree(d_modified);
    cudaFree(d_dist);

  }while(src < V);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(      CC,     d_CC, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN

int main(int argc, char *argv[])
{
  char *filename = argv[1];
  graph g(filename);
  g.parseGraph();
  float *CC = (float *)malloc((g.num_nodes()) * sizeof(float));
  Compute_CC(g, CC);

  for (int i = 0 ; i < g.num_nodes(); i++)
  {
    std::cout << i << " " << CC[i] << std::endl;
  }
  std::cout << std::endl;
}