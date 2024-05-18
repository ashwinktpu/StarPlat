// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "bc_dslV2.h"

void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet)

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
  float* h_BC;
  h_BC= (float*)malloc(sizeof(float)*(V+1));
  float** d_BC;
  d_BC = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_BC[i], sizeof(float)*(V+1));
  }


  //BEGIN DSL PARSING 
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_BC[i],(float)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_BC+h_vertex_partition[i],d_BC[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(float),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i+=1){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  float* h_sigma;
  h_sigma=(float*)malloc(sizeof(float)*(V+1));
  float** d_sigma;
  d_sigma = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_sigma[i], sizeof(float)*(V+1));
  }

  float* h_delta;
  h_delta=(float*)malloc(sizeof(float)*(V+1));
  float** d_delta;
  d_delta = (float**)malloc(sizeof(float*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_delta[i], sizeof(float)*(V+1));
  }

  //FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++) 
  {
    int src = *itr;
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_delta[i],(float)0);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_delta+h_vertex_partition[i],d_delta[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(float),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_sigma[i],(float)0);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_sigma+h_vertex_partition[i],d_sigma[i]+h_vertex_partition[i],(h_vertex_partition[i+1]-h_vertex_partition[i])*sizeof(float),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    //hi2
    h_sigma[src]=1;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initIndex<float><<<1,1>>>(V,d_sigma[i],src,(float)1); //InitIndexDevice
    }
    for(int i=0;i<devicecount;i+=1){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    bool* h_finished = (bool*)malloc(devicecount * sizeof(bool));
    int hops_from_source = 0 ; 

    bool **d_finished;
    d_finished = (bool**)malloc(sizeof(bool*)* devicecount) ; 
     for (int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_finished[i],sizeof(bool) *(1));
    }

    int **d_hops_from_source;
    d_hops_from_source = (int**)malloc(sizeof(int*)*devicecount); 
    for (int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_hops_from_source[i],sizeof(int) *(1));
    }

    int** d_level ; 
    d_level = (int**)malloc(sizeof(int*)*devicecount) ; 
     for (int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMalloc(&d_level[i],sizeof(int) *(V+1));
    }

    int** h_level_temp = (int**)malloc(sizeof(int*) * devicecount);
    for (int i = 0 ; i < devicecount ; i++){
      h_level_temp[i] = (int*)malloc(sizeof(int) * (V+1));
    }

    int* h_level = (int*)malloc(sizeof(int) * (V+1));

    //EXTRA vars INITIALIZATION
    for (int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<int><<<numBlocks,threadsPerBlock>>>(V,d_level[i],-1);
      initIndex<int><<<1,1>>>(V,d_level[i],src, 0);
      cudaDeviceSynchronize();
      cudaMemcpy(d_hops_from_source[i], &hops_from_source, sizeof(int), cudaMemcpyHostToDevice);
    }

    // long k =0 ;// For DEBUG
    h_sigma[src] = 1;

    for (int i =0  ; i < V ; i++){
      h_level[i] = -1;
    }
    do {
      finished = true;
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_finished[i], &finished, sizeof(bool), cudaMemcpyHostToDevice);
      }
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }

      //Kernel LAUNCH
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        fwd_pass<<<numBlocksKernel,threadsPerBlock>>>(V, i, h_vertex_partition[i], h_vertex_partition[i+1], d_offset[i], d_edges[i], d_weight[i], d_delta[i], d_sigma[i], d_level[i], d_hops_from_source[i], d_finished[i],d_BC[i]); ///DONE from varList
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for (int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        incrementDeviceVar<<<1,1>>>(d_hops_from_source[i]);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      ++hops_from_source; // updating the level to process in the next iteration
      // k++; //DEBUG

      if (devicecount > 1){
        for (int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          int s = h_vertex_partition[i], e = h_vertex_partition[i+1] ;
          cudaMemcpyAsync(h_sigma+s, d_sigma[i]+s, sizeof(float) * (e-s), cudaMemcpyDeviceToHost);
        }

        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }


      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(&h_finished[i], d_finished[i], sizeof(bool), cudaMemcpyDeviceToHost);
      }

      for (int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }

      for(int i = 0 ; i < devicecount ; i++){
        finished = finished && h_finished[i];
      }

      if (devicecount > 1){
        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(h_level_temp[i], d_level[i], sizeof(int) * (V), cudaMemcpyDeviceToHost);
        }

        for (int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }

        for(int i = 0 ; i < V ; i++){
          for(int j = 0 ; j < devicecount ; j++){
            if(h_level_temp[j][i] > h_level[i]){
              h_level[i] = h_level_temp[j][i];
              break ; 
            }
          }
        }

        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_level[i], h_level, sizeof(int) * (V), cudaMemcpyHostToDevice);
          cudaMemcpyAsync(d_sigma[i], h_sigma, sizeof(float) * (V), cudaMemcpyHostToDevice);
        }

        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }

      }
    }while(!finished);

    hops_from_source--;
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_hops_from_source[i], &hops_from_source, sizeof(int), cudaMemcpyHostToDevice) ; 
    }

    for (int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    //BACKWARD PASS
    while(hops_from_source > 1) {

      //KERNEL Launch
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        back_pass<<<numBlocksKernel,threadsPerBlock>>>(V, i, h_vertex_partition[i], h_vertex_partition[i+1], d_offset[i], d_edges[i], d_weight[i], d_delta[i], d_sigma[i], d_level[i], d_hops_from_source[i], d_finished[i]
          ,d_BC[i]);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      hops_from_source--;
      for (int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        decrementDeviceVar<<<1,1>>>(d_hops_from_source[i]);
      }

      for (int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i) ; 
        cudaDeviceSynchronize() ; 
      }
      if (devicecount > 1) {
        for (int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          int s = h_vertex_partition[i], e = h_vertex_partition[i+1];
          cudaMemcpyAsync(h_delta+s, d_delta[i]+s, sizeof(float) * (e-s), cudaMemcpyDeviceToHost);
        }
        for (int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }

        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaMemcpyAsync(d_delta[i], h_delta, sizeof(float) * (V), cudaMemcpyHostToDevice);
        }

        for(int i = 0 ; i < devicecount ; i++){
          cudaSetDevice(i);
          cudaDeviceSynchronize();
        }
      }

    }
    //accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);
  }
  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    int s = h_vertex_partition[i], e = h_vertex_partition[i+1] ; 
    cudaMemcpyAsync(      BC + s,     d_BC[i] + s, sizeof(float)*(e-s), cudaMemcpyDeviceToHost);
  }
  for (int i = 0 ; i < devicecount; i++){
    cudaSetDevice(i) ; 
    cudaDeviceSynchronize();
  }
} //end FUN
