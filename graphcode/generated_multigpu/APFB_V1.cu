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

   FILE* fptr = fopen("num_devices.txt","r"); 
  int devicecount;
   if(fptr == NULL){ 
     cudaGetDeviceCount(&devicecount); 
     } 
   else{ 
     fscanf(fptr," %d ",&devicecount); 
     fclose(fptr); 
  }
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
    h_rev_meta[i] = g.rev_indexofNodes[i];
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
  unsigned numBlocksKernel    = (V+threadsPerBlock-1)/threadsPerBlock;
  unsigned numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;

  if(devicecount>1){
    numBlocksKernel = numBlocksKernel/devicecount+1;
  }


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLARE DEVICE AND HOST vars in params
  int** d_nc;
  d_nc = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_nc[i], sizeof(int));
    initKernel<int> <<<1,1>>>(1,d_nc[i],nc);
  }

  int** h_nc = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_nc[i] = (int*)malloc(sizeof(int));
  }

  //BEGIN DSL PARSING 
  bool* h_modified;
  h_modified=(bool*)malloc(sizeof(bool)*(V+1));
  bool** d_modified;
  d_modified = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified[i], sizeof(bool)*(V+1));
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_modified+h_vertex_partition[i],d_modified[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  int* h_rmatch;
  h_rmatch=(int*)malloc(sizeof(int)*(V+1));
  int** d_rmatch;
  d_rmatch = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_rmatch[i], sizeof(int)*(V+1));
  }

  int* h_rmatch_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_rmatch_temp1;
  cudaMalloc(&d_rmatch_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_rmatch_temp2;
  cudaMalloc(&d_rmatch_temp2,(V+1)*(devicecount)*sizeof(int));


  int* h_cmatch;
  h_cmatch=(int*)malloc(sizeof(int)*(V+1));
  int** d_cmatch;
  d_cmatch = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_cmatch[i], sizeof(int)*(V+1));
  }

  int* h_cmatch_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_cmatch_temp1;
  cudaMalloc(&d_cmatch_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_cmatch_temp2;
  cudaMalloc(&d_cmatch_temp2,(V+1)*(devicecount)*sizeof(int));


  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_rmatch[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_rmatch+h_vertex_partition[i],d_rmatch[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_cmatch[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_cmatch+h_vertex_partition[i],d_cmatch[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  bool noNewPaths = false; // asst in .cu 
  bool** h_noNewPaths;
  h_noNewPaths = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_noNewPaths[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_noNewPaths;
  d_noNewPaths = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_noNewPaths[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_noNewPaths[i],false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }


  int* h_bfsArray;
  h_bfsArray=(int*)malloc(sizeof(int)*(V+1));
  int** d_bfsArray;
  d_bfsArray = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_bfsArray[i], sizeof(int)*(V+1));
  }

  int* h_bfsArray_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_bfsArray_temp1;
  cudaMalloc(&d_bfsArray_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_bfsArray_temp2;
  cudaMalloc(&d_bfsArray_temp2,(V+1)*(devicecount)*sizeof(int));


  int* h_predeccesor;
  h_predeccesor=(int*)malloc(sizeof(int)*(V+1));
  int** d_predeccesor;
  d_predeccesor = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_predeccesor[i], sizeof(int)*(V+1));
  }

  int* h_predeccesor_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_predeccesor_temp1;
  cudaMalloc(&d_predeccesor_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_predeccesor_temp2;
  cudaMalloc(&d_predeccesor_temp2,(V+1)*(devicecount)*sizeof(int));


  bool* h_compress;
  h_compress=(bool*)malloc(sizeof(bool)*(V+1));
  bool** d_compress;
  d_compress = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_compress[i], sizeof(bool)*(V+1));
  }

  bool* h_compress_next;
  h_compress_next=(bool*)malloc(sizeof(bool)*(V+1));
  bool** d_compress_next;
  d_compress_next = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_compress_next[i], sizeof(bool)*(V+1));
  }

  bool* h_compress_next_temp1 = (bool*)malloc((V+1)*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_compress_next_temp1;
  cudaMalloc(&d_compress_next_temp1,(V+1)*(devicecount)*sizeof(bool));
  bool* d_compress_next_temp2;
  cudaMalloc(&d_compress_next_temp2,(V+1)*(devicecount)*sizeof(bool));


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
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  while(!noNewPaths) {

    noNewPaths = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_noNewPaths[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    int L0 = 0; // asst in .cu 
    int** h_L0;
    h_L0 = (int**)malloc(sizeof(int*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_L0[i] = (int*)malloc(sizeof(int));
    }

    int** d_L0;
    d_L0 = (int**)malloc(sizeof(int*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_L0[i],sizeof(int));
      initKernel<int> <<<1,1>>>(1,d_L0[i],0);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    int NOT_VISITED = L0 - 1; // asst in .cu 
    int** h_NOT_VISITED;
    h_NOT_VISITED = (int**)malloc(sizeof(int*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_NOT_VISITED[i] = (int*)malloc(sizeof(int));
    }

    int** d_NOT_VISITED;
    d_NOT_VISITED = (int**)malloc(sizeof(int*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_NOT_VISITED[i],sizeof(int));
      initKernel<int> <<<1,1>>>(1,d_NOT_VISITED[i],L0 - 1);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_bfsArray[i],(int)NOT_VISITED);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_bfsArray+h_vertex_partition[i],d_bfsArray[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      APFB_kernel1<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_nc[i],d_cmatch[i],d_L0[i],d_bfsArray[i],d_noNewPaths[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    if(devicecount>1){
      //c c
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_bfsArray+h_vertex_partition[i],d_bfsArray[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_bfsArray[i],h_bfsArray,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_predeccesor[i],(int)-1);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_predeccesor+h_vertex_partition[i],d_predeccesor[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    int bfsLevel = L0; // asst in .cu 
    int** h_bfsLevel;
    h_bfsLevel = (int**)malloc(sizeof(int*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_bfsLevel[i] = (int*)malloc(sizeof(int));
    }

    int** d_bfsLevel;
    d_bfsLevel = (int**)malloc(sizeof(int*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_bfsLevel[i],sizeof(int));
      initKernel<int> <<<1,1>>>(1,d_bfsLevel[i],L0);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    bool noNewVertices = false; // asst in .cu 
    bool** h_noNewVertices;
    h_noNewVertices = (bool**)malloc(sizeof(bool*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_noNewVertices[i] = (bool*)malloc(sizeof(bool));
    }

    bool** d_noNewVertices;
    d_noNewVertices = (bool**)malloc(sizeof(bool*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_noNewVertices[i],sizeof(bool));
      initKernel<bool> <<<1,1>>>(1,d_noNewVertices[i],false);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    // FIXED POINT variables
    //BEGIN FIXED POINT
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    while(!noNewVertices) {

      noNewVertices = true;
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        initKernel<bool><<<1,1>>>(1,d_noNewVertices[i],(bool)true);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        APFB_kernel2<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_bfsArray[i],d_nc[i],d_bfsLevel[i],d_rmatch[i],d_NOT_VISITED[i],d_noNewVertices[i],d_predeccesor[i],d_noNewPaths[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }

      if(devicecount>1){
        //col_vertex col_match
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_bfsArray_temp1+i*(V+1),d_bfsArray[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_bfsArray_temp1,h_bfsArray_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_bfsArray_temp2+i*(V+1),h_bfsArray,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_bfsArray_temp1,d_bfsArray_temp2,V,devicecount);
        cudaMemcpy(h_bfsArray,d_bfsArray_temp1,(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_bfsArray[i],h_bfsArray,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //col_vertex neigh_row
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_predeccesor_temp1+i*(V+1),d_predeccesor[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_predeccesor_temp1,h_predeccesor_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_predeccesor_temp2+i*(V+1),h_predeccesor,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_predeccesor_temp1,d_predeccesor_temp2,V,devicecount);
        cudaMemcpy(h_predeccesor,d_predeccesor_temp1,(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_predeccesor[i],h_predeccesor,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //col_vertex neigh_row
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_rmatch_temp1+i*(V+1),d_rmatch[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_rmatch_temp1,h_rmatch_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_rmatch_temp2+i*(V+1),h_rmatch,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_rmatch_temp1,d_rmatch_temp2,V,devicecount);
        cudaMemcpy(h_rmatch,d_rmatch_temp1,(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_rmatch[i],h_rmatch,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //col_vertex neigh_row
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_predeccesor_temp1+i*(V+1),d_predeccesor[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_predeccesor_temp1,h_predeccesor_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_predeccesor_temp2+i*(V+1),h_predeccesor,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_predeccesor_temp1,d_predeccesor_temp2,V,devicecount);
        cudaMemcpy(h_predeccesor,d_predeccesor_temp1,(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_predeccesor[i],h_predeccesor,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      bfsLevel = bfsLevel + 1;
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        //printed here

        initKernel<int> <<<1,1>>>(1,d_bfsLevel[i],(int)bfsLevel);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_noNewVertices[i], d_noNewVertices[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        noNewVertices&=h_noNewVertices[i][0];
      }
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        initKernel<bool> <<<1,1>>>(1,d_noNewVertices[i],true);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    } // END FIXED POINT

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_compress[i],(bool)false);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_compress+h_vertex_partition[i],d_compress[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_compress_next[i],(bool)false);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_compress_next+h_vertex_partition[i],d_compress_next[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      APFB_kernel3<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_nc[i],d_rmatch[i],d_compress[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    if(devicecount>1){
      //r r
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_compress+h_vertex_partition[i],d_compress[i]+h_vertex_partition[i],sizeof(bool)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_compress[i],h_compress,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    bool compressed = false; // asst in .cu 
    bool** h_compressed;
    h_compressed = (bool**)malloc(sizeof(bool*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_compressed[i] = (bool*)malloc(sizeof(bool));
    }

    bool** d_compressed;
    d_compressed = (bool**)malloc(sizeof(bool*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_compressed[i],sizeof(bool));
      initKernel<bool> <<<1,1>>>(1,d_compressed[i],false);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    // FIXED POINT variables
    //BEGIN FIXED POINT
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    while(!compressed) {

      compressed = true;
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        initKernel<bool><<<1,1>>>(1,d_compressed[i],(bool)true);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        APFB_kernel4<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_compress[i],d_nc[i],d_cmatch[i],d_predeccesor[i],d_compressed[i],d_compress_next[i],d_rmatch[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }



      if(devicecount>1){
        //row_vertex matched_col
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_cmatch_temp1+i*(V+1),d_cmatch[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_cmatch_temp1,h_cmatch_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_cmatch_temp2+i*(V+1),h_cmatch,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_cmatch_temp1,d_cmatch_temp2,V,devicecount);
        cudaMemcpy(h_cmatch,d_cmatch_temp1,(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_cmatch[i],h_cmatch,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //row_vertex row_vertex
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_rmatch+h_vertex_partition[i],d_rmatch[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_rmatch[i],h_rmatch,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //row_vertex matched_row
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_compress_next_temp1+i*(V+1),d_compress_next[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_compress_next_temp1,h_compress_next_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_compress_next_temp2+i*(V+1),h_compress_next,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
        }
        Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_compress_next_temp1,d_compress_next_temp2,V,devicecount);
        cudaMemcpy(h_compress_next,d_compress_next_temp1,(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_compress_next[i],h_compress_next,(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        APFB_kernel5<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_nc[i],d_compress_next[i],d_compress[i],d_compressed[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      if(devicecount>1){
        //row_vertex row_vertex
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_compress+h_vertex_partition[i],d_compress[i]+h_vertex_partition[i],sizeof(bool)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_compress[i],h_compress,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //row_vertex row_vertex
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_compress_next+h_vertex_partition[i],d_compress_next[i]+h_vertex_partition[i],sizeof(bool)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_compress_next[i],h_compress_next,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_compressed[i], d_compressed[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        compressed&=h_compressed[i][0];
      }
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        initKernel<bool> <<<1,1>>>(1,d_compressed[i],true);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    } // END FIXED POINT

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      APFB_kernel6<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_nc[i],d_cmatch[i],d_rmatch[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    if(devicecount>1){
      //r r
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_rmatch+h_vertex_partition[i],d_rmatch[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_rmatch[i],h_rmatch,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    if(devicecount>1){
      //r r
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_rmatch+h_vertex_partition[i],d_rmatch[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_rmatch[i],h_rmatch,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_noNewPaths[i], d_noNewPaths[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      noNewPaths&=h_noNewPaths[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_noNewPaths[i],true);
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

} //end FUN
