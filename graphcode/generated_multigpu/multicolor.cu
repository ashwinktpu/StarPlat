#include "multicolor.h"

void colorGraph(graph& g){
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
  printf(" device count %d\n",devicecount);
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
  
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  unsigned long ** d_color ;
  d_color = (unsigned long**)malloc(sizeof(unsigned long*)*devicecount);
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMalloc(&d_color[i],sizeof(unsigned long)*(V+1));
  }
  unsigned int ** d_color1 ;
  d_color1 = (unsigned int**)malloc(sizeof(unsigned int*)*devicecount);
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMalloc(&d_color1[i],sizeof(unsigned int)*(V+1));
  }
  unsigned int ** d_color2 ;
  d_color2 = (unsigned int**)malloc(sizeof(unsigned int*)*devicecount);
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMalloc(&d_color2[i],sizeof(unsigned int)*(V+1));
  }
  


  // unsigned long** h_color;
  // h_color = (unsigned long**)malloc(sizeof(unsigned long*)*devicecount);
  // for(int i=0;i<devicecount;i+=1){
  //   h_color[i]=(unsigned long*)malloc(sizeof(unsigned long)*(V+1));
  // }

  bool** d_modified;
  d_modified = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMalloc(&d_modified[i],sizeof(bool)*(V+1));
  }
  bool** d_modified_next;
  d_modified_next = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMalloc(&d_modified_next[i],sizeof(bool)*(V+1));
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    initKernel<unsigned long><<<numBlocks,threadsPerBlock>>>(V,d_color[i],(unsigned long)0.000);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    initKernel<bool><<<numBlocks,threadsPerBlock>>>(V,d_modified[i],(bool)false);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    initKernel<bool><<<numBlocks,threadsPerBlock>>>(V,d_modified_next[i],(bool)false);
    cudaDeviceSynchronize();
  }

  int fpoint1 = 0; // asst in .cu 
  int** h_fpoint1;
  h_fpoint1 = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_fpoint1[i] = (int*)malloc(sizeof(int));
  }

  int** d_fpoint1;
  d_fpoint1 = (int**)malloc(sizeof(int*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_fpoint1[i],sizeof(int));
    initKernel<int> <<<1,1>>>(1,d_fpoint1[i],(int)0);
  }

  int iter = 0;
  do{
    printf("iter val %d\n",iter);
    // printf("iter value %d\n",iter);
    iter+=1;
    int x = rand();
    int y = rand();
    for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        curandGenerator_t gen;
        curandCreateGenerator(&gen,CURAND_RNG_PSEUDO_PHILOX4_32_10);
        curandSetPseudoRandomGeneratorSeed(gen,x);       
        curandGenerate(gen,d_color1[i],(V+1));
        curandSetPseudoRandomGeneratorSeed(gen,y);
        curandGenerate(gen,d_color2[i],(V+1));
        initialize<<<numBlocks,numThreads>>>(V,d_color[i],d_color1[i],d_color2[i]);
        // cudaMemcpyAsync(h_color[i],d_color[i],(V+1)*sizeof(unsigned long),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
    }
    // printf("=========================================\n");
    // for(int i=0;i<devicecount;i+=1){
    //   for(int j=0;j<=V;j+=1){
    //     printf("%0.15lf ",h_color[i][j]);
    //   }
    //   printf("\n");
    // }

    //   printf("=========================================\n");
    for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        compute_colors<<<numBlocks,numThreads>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_src[i],d_rev_meta[i],d_color[i],d_modified[i],d_modified_next[i],d_fpoint1[i]);
        cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i+=1){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_fpoint1[i],d_fpoint1[i],sizeof(int),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize(); 
    }
    fpoint1 = 0;
    for(int i=0;i<devicecount;i+=1){
      fpoint1+=h_fpoint1[i][0];
    }
    // printf("%d\n",fpoint1);
    // for(int i=0;i<devicecount;i+=1){
    //   cudaSetDevice(i);
    //   initKernel<int> <<<1,1>>>(1,d_fpoint1[i],(int)0);
    //   cudaDeviceSynchronize();
    // }
    bool* h_modified_next;
    h_modified_next =(bool*)malloc((V+1)*sizeof(bool));
    for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_modified_next+h_vertex_partition[i],d_modified_next[i]+h_vertex_partition[i],sizeof(bool)*(h_vertex_partition[i+1]-h_vertex_partition[i]),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_modified[i],h_modified_next,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
        cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_modified_next[i],h_modified_next,sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
        cudaDeviceSynchronize();
    }
    // for(int i=0;i<=V;i+=1){
      // printf("%d ",h_modified_next[i]);
    // }
    // printf("\n");
    // printf("%d ended\n",iter);

  }while(fpoint1<V);
  printf("num colors %d\n",iter);
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
}

int main(int argc,char* argv[])
{
  char *file_name = argv[1];
  graph g(file_name);
  g.parseGraph();
  colorGraph(g);
  return 0;
}
