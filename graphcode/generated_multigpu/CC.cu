// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "CC.h"

void Compute_CC(graph& g,float* CC,std::set<int>& sourceSet)

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


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLARE DEVICE AND HOST vars in params
  float* h_CC;
  h_CC= (float*)malloc(sizeof(float)*(V+1));
  float** d_CC;
  d_CC = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_CC[i], sizeof(float)*(V+1));
  }


  //BEGIN DSL PARSING 
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_CC[i],(float)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_CC+h_vertex_partition[i],d_CC[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(float),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  int numNodes = g.num_nodes( ); // asst in .cu 
  //fixed_pt_var

  int* h_dist;
  h_dist=(int*)malloc(sizeof(int)*(V+1));
  int** d_dist;
  d_dist = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_dist[i], sizeof(int)*(V+1));
  }

  bool* h_modified;
  h_modified=(bool*)malloc(sizeof(bool)*(V+1));
  bool** d_modified;
  d_modified = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified[i], sizeof(bool)*(V+1));
  }

  //FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++) 
  {
    int src = *itr;
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_dist[i],(int)INT_MAX);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_dist+h_vertex_partition[i],d_dist[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
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
    //hi2
    h_modified[src]=true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initIndex<bool><<<1,1>>>(V,d_modified[i],src,(bool)true); //InitIndexDevice
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    //hi2
    h_dist[src]=0;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initIndex<int><<<1,1>>>(V,d_dist[i],src,(int)0); //InitIndexDevice
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    bool finished = false; // asst in .cu 
    //fixed_pt_var
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
        Compute_CC_kernel1<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_dist[i],d_modified[i],d_modified_next[i],d_finished[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      //global loop var v iden v
      if(devicecount>1){
        //pull based

        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_dist+h_vertex_partition[i],d_dist[i]+h_vertex_partition[i],sizeof(int)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_dist[i],h_dist,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      bool* h_modified1;
      if(devicecount==1){
        cudaMemcpy(d_modified[0],d_modified_next[0],sizeof(bool)*(V+1),cudaMemcpyDeviceToDevice);
      }
      if(devicecount>1){
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_modified+h_vertex_partition[i],d_modified_next[i]+h_vertex_partition[i],sizeof(bool)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_modified[i],h_modified,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
        }
        for(int i=0;i<devicecount;i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
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

    int temp = 0; // asst in .cu 
    //fixed_pt_var
    int** h_temp;
    h_temp = (int**)malloc(sizeof(int*)*(devicecount+1));
    for(int i=0;i<=devicecount;i+=1){
      h_temp[i] = (int*)malloc(sizeof(int));
    }

    int** d_temp;
    d_temp = (int**)malloc(sizeof(int*)*devicecount);
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_temp[i],sizeof(int));
      initKernel<int> <<<1,1>>>(1,d_temp[i],0);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      Compute_CC_kernel2<<<numBlocksKernel, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_temp[i],d_dist[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_temp[i],d_temp[i],sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    int temp_=0;
    for(int i=0;i<devicecount;i++){
      temp_ += h_temp[i][0];
    } //end of for
    temp=temp_;
    //hi2
    h_CC[src]=1.000000 / temp;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initIndex<float><<<1,1>>>(V,d_CC[i],src,(float)1.000000 / temp); //InitIndexDevice
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  }
  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    int s = h_vertex_partition[i], e = h_vertex_partition[i+1] ; 
    cudaMemcpyAsync(      CC + s,     d_CC[i] + s, sizeof(float)*(e-s), cudaMemcpyDeviceToHost);
  }
  for (int i = 0 ; i < devicecount; i++){
    cudaSetDevice(i) ; 
    cudaDeviceSynchronize();
  }
} //end FUN
