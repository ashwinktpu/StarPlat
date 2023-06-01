// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "mst_dsl1.h"

void Boruvka(graph& g)

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

  //BEGIN DSL PARSING 
  int* h_nodeId;
  h_nodeId=(int*)malloc(sizeof(int)*V+1);
  int** d_nodeId;
  d_nodeId = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_nodeId[i], sizeof(int)*(V+1));
  }

  int* h_color;
  h_color=(int*)malloc(sizeof(int)*V+1);
  int** d_color;
  d_color = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_color[i], sizeof(int)*(V+1));
  }

  bool* h_isMSTEdge;
  h_isMSTEdge=(bool*)malloc(sizeof(bool)*E);
  bool** d_isMSTEdge;
  d_isMSTEdge = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_isMSTEdge[i], sizeof(bool)*E);
  }

  bool* h_isMSTEdge_temp1 = (bool*)malloc(E*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_isMSTEdge_temp1;
  cudaMalloc(&d_isMSTEdge_temp1,E*(devicecount)*sizeof(bool));
  bool* d_isMSTEdge_temp2;
  cudaMalloc(&d_isMSTEdge_temp2,E*(devicecount)*sizeof(bool));


  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_nodeId[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_nodeId+h_vertex_partition[i],d_nodeId[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_color[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_color+h_vertex_partition[i],d_color[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(E,d_isMSTEdge[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  cudaMemcpyAsync(h_isMSTEdge,d_isMSTEdge[0],E*sizeof(bool),cudaMemcpyDeviceToHost);
  cudaDeviceSynchronize();
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    Boruvka_kernel1<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_color[i],d_nodeId[i]);
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  if(devicecount>1){
    //u u
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_nodeId+h_vertex_partition[i],d_nodeId[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_nodeId[i],h_nodeId,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  }
  if(devicecount>1){
    //u u
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_color+h_vertex_partition[i],d_color[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_color[i],h_color,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  }
  bool* h_modified;
  h_modified=(bool*)malloc(sizeof(bool)*V+1);
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
  int* h_minEdgeOfComp;
  h_minEdgeOfComp=(int*)malloc(sizeof(int)*V+1);
  int** d_minEdgeOfComp;
  d_minEdgeOfComp = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_minEdgeOfComp[i], sizeof(int)*(V+1));
  }

  int* h_minEdgeOfComp_temp1 = (int*)malloc(V+1*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_minEdgeOfComp_temp1;
  cudaMalloc(&d_minEdgeOfComp_temp1,V+1*(devicecount)*sizeof(int));
  int* d_minEdgeOfComp_temp2;
  cudaMalloc(&d_minEdgeOfComp_temp2,V+1*(devicecount)*sizeof(int));


  int* h_minEdge;
  h_minEdge=(int*)malloc(sizeof(int)*V+1);
  int** d_minEdge;
  d_minEdge = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_minEdge[i], sizeof(int)*(V+1));
  }

  bool noNewComp = false; // asst in .cu 
  bool** h_noNewComp;
  h_noNewComp = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_noNewComp[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_noNewComp;
  d_noNewComp = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_noNewComp[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_noNewComp[i],false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
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
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  while(!noNewComp) {

    noNewComp = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_noNewComp[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge[i],(int)-1);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_minEdge+h_vertex_partition[i],d_minEdge[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Boruvka_kernel2<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_minEdge[i],d_color[i],d_noNewComp[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    if(devicecount>1){
      //src src
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_minEdge+h_vertex_partition[i],d_minEdge[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_minEdge[i],h_minEdge,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    if(devicecount>1){
      //src src
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_minEdge+h_vertex_partition[i],d_minEdge[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_minEdge[i],h_minEdge,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp[i],(int)-1);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_minEdgeOfComp+h_vertex_partition[i],d_minEdgeOfComp[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    bool finishedMinEdge = false; // asst in .cu 
    bool** h_finishedMinEdge;
    h_finishedMinEdge = (bool**)malloc(sizeof(bool*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_finishedMinEdge[i] = (bool*)malloc(sizeof(bool));
    }

    bool** d_finishedMinEdge;
    d_finishedMinEdge = (bool**)malloc(sizeof(bool*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_finishedMinEdge[i],sizeof(bool));
      initKernel<bool> <<<1,1>>>(1,d_finishedMinEdge[i],false);
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

    while(!finishedMinEdge) {

      finishedMinEdge = true;
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        initKernel<bool><<<1,1>>>(1,d_finishedMinEdge[i],(bool)true);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        Boruvka_kernel3<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_minEdge[i],d_minEdgeOfComp[i],d_color[i],d_finishedMinEdge[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }




      if(devicecount>1){
        //u comp
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_minEdgeOfComp_temp1+i*V+1,d_minEdgeOfComp[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_minEdgeOfComp_temp1,h_minEdgeOfComp_temp1,V+1*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_minEdgeOfComp_temp2+i*V+1,h_minEdgeOfComp,sizeof(int)*V+1,cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_minEdgeOfComp_temp1,d_minEdgeOfComp_temp2,V+1,devicecount);
        cudaMemcpy(h_minEdgeOfComp,d_minEdgeOfComp_temp1,V+1*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_minEdgeOfComp[i],h_minEdgeOfComp,V+1*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      if(devicecount>1){
        //u comp
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_minEdgeOfComp_temp1+i*V+1,d_minEdgeOfComp[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        cudaSetDevice(0);
        cudaMemcpy(d_minEdgeOfComp_temp1,h_minEdgeOfComp_temp1,V+1*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
        for(int i=0;i<devicecount;i++){
          cudaMemcpy(d_minEdgeOfComp_temp2+i*V+1,h_minEdgeOfComp,sizeof(int)*V+1,cudaMemcpyHostToDevice);
        }
        Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_minEdgeOfComp_temp1,d_minEdgeOfComp_temp2,V+1,devicecount);
        cudaMemcpy(h_minEdgeOfComp,d_minEdgeOfComp_temp1,V+1*sizeof(int),cudaMemcpyDeviceToHost);
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_minEdgeOfComp[i],h_minEdgeOfComp,V+1*sizeof(int),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_finishedMinEdge[i], d_finishedMinEdge[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        finishedMinEdge&=h_finishedMinEdge[i][0];
      }
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        initKernel<bool> <<<1,1>>>(1,d_finishedMinEdge[i],true);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    } // END FIXED POINT

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Boruvka_kernel4<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_color[i],d_nodeId[i],d_minEdgeOfComp[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    if(devicecount>1){
      //src dstLead
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_minEdgeOfComp_temp1+i*V+1,d_minEdgeOfComp[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      cudaSetDevice(0);
      cudaMemcpy(d_minEdgeOfComp_temp1,h_minEdgeOfComp_temp1,V+1*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(d_minEdgeOfComp_temp2+i*V+1,h_minEdgeOfComp,sizeof(int)*V+1,cudaMemcpyHostToDevice);
      }
      Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_minEdgeOfComp_temp1,d_minEdgeOfComp_temp2,V+1,devicecount);
      cudaMemcpy(h_minEdgeOfComp,d_minEdgeOfComp_temp1,V+1*sizeof(int),cudaMemcpyDeviceToHost);
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_minEdgeOfComp[i],h_minEdgeOfComp,V+1*sizeof(int),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Boruvka_kernel5<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_color[i],d_nodeId[i],d_minEdgeOfComp[i],d_isMSTEdge[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    if(devicecount>1){
      //src srcMinEdge
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_isMSTEdge_temp1+i*E,d_isMSTEdge[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      cudaSetDevice(0);
      cudaMemcpy(d_isMSTEdge_temp1,h_isMSTEdge_temp1,E*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(d_isMSTEdge_temp2+i*E,h_isMSTEdge,sizeof(bool)*E,cudaMemcpyHostToDevice);
      }
      Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_isMSTEdge_temp1,d_isMSTEdge_temp2,E,devicecount);
      cudaMemcpy(h_isMSTEdge,d_isMSTEdge_temp1,E*sizeof(bool),cudaMemcpyDeviceToHost);
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_isMSTEdge[i],h_isMSTEdge,E*sizeof(bool),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Boruvka_kernel6<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_color[i],d_nodeId[i],d_minEdgeOfComp[i],d_noNewComp[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    if(devicecount>1){
      //src src
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_color+h_vertex_partition[i],d_color[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_color[i],h_color,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    bool finished = false; // asst in .cu 
    bool** h_finished;
    h_finished = (bool**)malloc(sizeof(bool*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_finished[i] = (bool*)malloc(sizeof(bool));
    }

    bool** d_finished;
    d_finished = (bool**)malloc(sizeof(bool*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_finished[i],sizeof(bool));
      initKernel<bool> <<<1,1>>>(1,d_finished[i],false);
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

    while(!finished) {

      finished = true;
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        initKernel<bool><<<1,1>>>(1,d_finished[i],(bool)true);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        Boruvka_kernel7<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_color[i],d_finished[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }


      if(devicecount>1){
        //u u
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_color+h_vertex_partition[i],d_color[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_color[i],h_color,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
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
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        initKernel<bool> <<<1,1>>>(1,d_finished[i],true);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    } // END FIXED POINT

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_noNewComp[i], d_noNewComp[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      noNewComp&=h_noNewComp[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_noNewComp[i],true);
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
