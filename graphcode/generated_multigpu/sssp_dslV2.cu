// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "sssp_dslV2.h"

void Compute_SSSP(graph& g,int* dist,int src)

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
  int** h_dist;
  h_dist = (int**)malloc(sizeof(int*)*(devicecount));
  for(int i=0;i<devicecount;i++){
    h_dist[i] = (int*)malloc(sizeof(int)*(V+1));
  }
  int** d_dist;
  d_dist = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_dist[i], sizeof(int)*(V+1));
  }


  //BEGIN DSL PARSING 
  bool** d_modified;
  d_modified = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified[i], sizeof(bool)*(V+1));
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_dist[i],(int)INT_MAX);
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    initIndex<bool><<<1,1>>>(V,d_modified[i],src,(bool)true); //InitIndexDevice
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    initIndex<int><<<1,1>>>(V,d_dist[i],src,(int)0); //InitIndexDevice
  }
  bool finished = false; // asst in .cu 
  bool** h_finished;
  h_finished = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<devicecount;i+=1){
    h_finished[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_finished;
  d_finished = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_finished[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_finished[i],true);
  }


  bool** d_modified_next;
  d_modified_next = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified_next[i], sizeof(bool)*(V+1));
  }


  // FIXED POINT variables
  //BEGIN FIXED POINT
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
  }

  int k=0; // #fixpt-Iterations
  bool** h_modified;
  h_modified = (bool**)malloc(sizeof(bool*)*(devicecount+1)); 
  for (int i = 0 ; i < devicecount ; i++){
    h_modified[i] = (bool*)malloc(sizeof(bool)*(V+1));
  }

  while(!finished) {

    finished = true;
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Compute_SSSP_kernel1<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_modified[i],d_modified_next[i],d_dist[i],d_finished[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }



    //ronaldo
    cudaSetDevice(0);
    int* d_dist_temp;
    int* d_dist_temp1;
    cudaMalloc(&d_dist_temp , (V+1)*sizeof(int));
    cudaMalloc(&d_dist_temp1,(V+1)*(devicecount)*sizeof(int));
    initKernel<int><<<numBlocks,threadsPerBlock>>>(V+1,d_dist_temp,(int)INT_MAX);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_dist_temp1+i*(V+1),h_dist[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_Min<<<numBlocks,numThreads>>>(d_dist_temp1,d_dist_temp,V,devicecount);
    cudaMemcpy(h_dist[devicecount],d_dist_temp,sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_dist[i],h_dist[devicecount],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_modified[i],d_modified_next[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    bool* d_modified_temp;
    bool* d_modified_temp1;
    cudaMalloc(&d_modified_temp,(V+1)*sizeof(bool));
    cudaMalloc(&d_modified_temp1,(devicecount)*(V+1)*sizeof(bool));
    initKernel<bool><<<numBlocks,threadsPerBlock>>>(V+1,d_modified_temp,false);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_modified_temp1+i*(V+1),h_modified[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_Or<<<numBlocks,threadsPerBlock>>>(d_modified_temp1,d_modified_temp,V,devicecount);
    cudaMemcpy(h_modified[devicecount],d_modified_temp,sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_modified[i],h_modified[devicecount],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_finished[i], d_finished[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      finished&=h_finished[i][0];
    }
    k++;
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_finished[i],true);
    }

    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  } // END FIXED POINT

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(dist,h_dist[devicecount], sizeof(int)*(V+1), cudaMemcpyDeviceToHost);
} //end FUN
