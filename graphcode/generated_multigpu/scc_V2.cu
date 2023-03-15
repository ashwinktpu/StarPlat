// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "scc_V2.h"

void vHong(graph& g)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int devicecount;
  cudaGetDeviceCount(&devicecount);
  int* h_vertex_partition;
  int *h_edges;//data
  int *h_weight;
  int *h_offset;//meta
  int* h_src;
  int *h_rev_meta;
  int h_vertex_per_device;

  h_edges = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_offset = (int *)malloc( (V+1)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_vertex_partition = (int*)malloc((devicecount+1)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));
  h_vertex_per_device = V/devicecount;

  for(int i=0; i<= V; i++) {
    h_offset[i] = g.indexofNodes[i];
  }

  int index = 0;
  h_vertex_partition[0]=0;
  h_vertex_partition[devicecount]=V;
  for(int i=1;i<devicecount;i++){
    if(i<=(V%devicecount)){
       index+=(h_vertex_per_device+1);
    }
    else{
       index+=h_vertex_per_device;
    }
    h_vertex_partition[i]=index;
  }
  for(int i=0; i< E; i++){
    h_edges[i]= g.edgeList[i];
    h_src[i]=g.srcList[i];
    h_weight[i] = edgeLen[i];
  }


  int** d_offset;
  int** d_edges;
  int** d_weight;
  int** d_src;
  int** d_rev_meta;
  d_offset = (int**) malloc(devicecount*sizeof(int*));
  d_edges = (int**) malloc(devicecount*sizeof(int*));
  d_weight = (int**) malloc(devicecount*sizeof(int*));
  d_src = (int**) malloc(devicecount*sizeof(int*));
  d_rev_meta = (int**) malloc(devicecount*sizeof(int*));

  int perdevicevertices;
  int lastleftvertices;
  perdevicevertices = V / devicecount ;
  lastleftvertices = V % devicecount;
  for(int i=0;i<devicecount;i++)	
  {
    cudaSetDevice(i);
    cudaMalloc(&d_offset[i], (V+1)*sizeof(int) );
    cudaMalloc(&d_edges[i], (E)*sizeof(int) );
    cudaMalloc(&d_weight[i], (E)*sizeof(int) );
    cudaMalloc(&d_src[i], (E)*sizeof(int) );
    cudaMalloc(&d_rev_meta[i], (V+1)*sizeof(int) );
  }
  for(int i=0;i<devicecount;i++)	
  {
    cudaSetDevice(i);
    cudaMemcpyAsync(d_offset[i], h_offset, (V+1)*sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpyAsync(d_edges[i], h_edges, (E)*sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpyAsync(d_weight[i], h_weight, (E)*sizeof(int),cudaMemcpyHostToDevice );
    cudaMemcpyAsync(d_src[i], h_src, (E)*sizeof(int),cudaMemcpyHostToDevice );
    cudaMemcpyAsync(d_rev_meta[i], h_rev_meta, (V+1)*sizeof(int),cudaMemcpyHostToDevice );
  }
  for(int i=0;i<devicecount;i++)	
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  const unsigned threadsPerBlock = 1024;
  unsigned numThreads   = (V < threadsPerBlock)? V: 1024;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLARE DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int** d_modified;
  d_modified = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified[i], sizeof(int)*(V+1));
  }

  int** d_outDeg;
  d_outDeg = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_outDeg[i], sizeof(int)*(V+1));
  }

  int** d_inDeg;
  d_inDeg = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_inDeg[i], sizeof(int)*(V+1));
  }

  bool** d_visitFw;
  d_visitFw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_visitFw[i], sizeof(bool)*(V+1));
  }

  bool** d_visitBw;
  d_visitBw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_visitBw[i], sizeof(bool)*(V+1));
  }

  bool** d_propFw;
  d_propFw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_propFw[i], sizeof(bool)*(V+1));
  }

  bool** d_propBw;
  d_propBw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_propBw[i], sizeof(bool)*(V+1));
  }

  bool** d_isPivot;
  d_isPivot = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_isPivot[i], sizeof(bool)*(V+1));
  }

  int** d_scc;
  d_scc = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_scc[i], sizeof(int)*(V+1));
  }

  int** d_range;
  d_range = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_range[i], sizeof(int)*(V+1));
  }

  int** d_pivotField;
  d_pivotField = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_pivotField[i], sizeof(int)*(V+1));
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_modified[i],(int)false);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_outDeg[i],(int)0);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_inDeg[i],(int)0);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitFw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitBw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propFw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propBw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_isPivot[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_scc[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_range[i],(int)0);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_pivotField[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    vHong_kernel1<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_outDeg[i],d_inDeg[i]);
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }




  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
