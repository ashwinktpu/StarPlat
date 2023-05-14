// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "PageRankDSLV2.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)

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
  float** d_delta;
  d_delta = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_delta[i], sizeof(float));
    initKernel<float> <<<1,1>>>(1,d_delta[i],delta);
  }

  float** h_delta = (float**)malloc(sizeof(float*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_delta[i] = (float*)malloc(sizeof(float));
  }
  float* h_pageRank;
  h_pageRank= (float*)malloc(sizeof(float)*(V+1));
  float** d_pageRank;
  d_pageRank = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_pageRank[i], sizeof(float)*(V+1));
  }


  //BEGIN DSL PARSING 
  float num_nodes = (float)g.num_nodes( ); // asst in .cu 
  //fixed_pt_var
  float** h_num_nodes;
  h_num_nodes = (float**)malloc(sizeof(float*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_num_nodes[i] = (float*)malloc(sizeof(float));
  }

  float** d_num_nodes;
  d_num_nodes = (float**)malloc(sizeof(float*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_num_nodes[i],sizeof(float));
    initKernel<float> <<<1,1>>>(1,d_num_nodes[i],g.num_nodes( ));
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }


  float* h_pageRank_nxt;
  h_pageRank_nxt=(float*)malloc(sizeof(float)*(V+1));
  float** d_pageRank_nxt;
  d_pageRank_nxt = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_pageRank_nxt[i], sizeof(float)*(V+1));
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank[i],(float)1 / num_nodes);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_pageRank+h_vertex_partition[i],d_pageRank[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(float),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_pageRank_nxt[i],(float)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_pageRank_nxt+h_vertex_partition[i],d_pageRank_nxt[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(float),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  int iterCount = 0; // asst in .cu 
  //fixed_pt_var

  float diff; // asst in .cu 
  //fixed_pt_var
  float** h_diff;
  h_diff = (float**)malloc(sizeof(float*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_diff[i] = (float*)malloc(sizeof(float));
  }

  float** d_diff;
  d_diff = (float**)malloc(sizeof(float*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_diff[i],sizeof(float));
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }


  do{
    diff = 0.000000;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      //printed here

      initKernel<float> <<<1,1>>>(1,d_diff[i],(float)diff);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Compute_PR_kernel1<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_diff[i],d_delta[i],d_num_nodes[i],d_pageRank[i],d_pageRank_nxt[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    //fixed_pt_var

    //fixed_pt_var

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_diff[i],d_diff[i],sizeof(float),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    float diff_=0;
    for(int i=0;i<devicecount;i++){
      diff_ += h_diff[i][0];
    } //end of for
    diff=diff_;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_diff[i],d_diff[i],sizeof(float),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    float diff_=0;
    for(int i=0;i<devicecount;i++){
      diff_ += h_diff[i][0];
    } //end of for
    diff=diff_;
    if(devicecount>1){
      //v v
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_pageRank_nxt+h_vertex_partition[i],d_pageRank_nxt[i]+h_vertex_partition[i],sizeof(float)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_pageRank_nxt[i],h_pageRank_nxt,sizeof(float)*(V+1),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpy(d_pageRank[i],d_pageRank_nxt[i],sizeof(float)*(V+1),cudaMemcpyDeviceToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    iterCount++;
  }while((diff > beta) && (iterCount < maxIter));
  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(pageRank, d_pageRank, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
