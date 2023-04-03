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
  h_vertex_partition[devicecount]=V+1;
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
  int** h_modified;
  h_modified = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_modified[i]=(int*)malloc(sizeof(int)*(V+1));
  }
  int** d_modified;
  d_modified = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified[i], sizeof(int)*(V+1));
  }

  int* h_modified_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_modified_temp1;
  cudaMalloc(&d_modified_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_modified_temp2;
  cudaMalloc(&d_modified_temp2,(V+1)*(devicecount)*sizeof(int));
  int** h_outDeg;
  h_outDeg = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_outDeg[i]=(int*)malloc(sizeof(int)*(V+1));
  }
  int** d_outDeg;
  d_outDeg = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_outDeg[i], sizeof(int)*(V+1));
  }

  int* h_outDeg_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_outDeg_temp1;
  cudaMalloc(&d_outDeg_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_outDeg_temp2;
  cudaMalloc(&d_outDeg_temp2,(V+1)*(devicecount)*sizeof(int));
  int** h_inDeg;
  h_inDeg = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_inDeg[i]=(int*)malloc(sizeof(int)*(V+1));
  }
  int** d_inDeg;
  d_inDeg = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_inDeg[i], sizeof(int)*(V+1));
  }

  int* h_inDeg_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_inDeg_temp1;
  cudaMalloc(&d_inDeg_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_inDeg_temp2;
  cudaMalloc(&d_inDeg_temp2,(V+1)*(devicecount)*sizeof(int));
  bool** h_visitFw;
  h_visitFw = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_visitFw[i]=(bool*)malloc(sizeof(bool)*(V+1));
  }
  bool** d_visitFw;
  d_visitFw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_visitFw[i], sizeof(bool)*(V+1));
  }

  bool* h_visitFw_temp1 = (bool*)malloc((V+1)*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_visitFw_temp1;
  cudaMalloc(&d_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool));
  bool* d_visitFw_temp2;
  cudaMalloc(&d_visitFw_temp2,(V+1)*(devicecount)*sizeof(bool));
  bool** h_visitBw;
  h_visitBw = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_visitBw[i]=(bool*)malloc(sizeof(bool)*(V+1));
  }
  bool** d_visitBw;
  d_visitBw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_visitBw[i], sizeof(bool)*(V+1));
  }

  bool* h_visitBw_temp1 = (bool*)malloc((V+1)*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_visitBw_temp1;
  cudaMalloc(&d_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool));
  bool* d_visitBw_temp2;
  cudaMalloc(&d_visitBw_temp2,(V+1)*(devicecount)*sizeof(bool));
  bool** h_propFw;
  h_propFw = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_propFw[i]=(bool*)malloc(sizeof(bool)*(V+1));
  }
  bool** d_propFw;
  d_propFw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_propFw[i], sizeof(bool)*(V+1));
  }

  bool* h_propFw_temp1 = (bool*)malloc((V+1)*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_propFw_temp1;
  cudaMalloc(&d_propFw_temp1,(V+1)*(devicecount)*sizeof(bool));
  bool* d_propFw_temp2;
  cudaMalloc(&d_propFw_temp2,(V+1)*(devicecount)*sizeof(bool));
  bool** h_propBw;
  h_propBw = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_propBw[i]=(bool*)malloc(sizeof(bool)*(V+1));
  }
  bool** d_propBw;
  d_propBw = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_propBw[i], sizeof(bool)*(V+1));
  }

  bool* h_propBw_temp1 = (bool*)malloc((V+1)*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_propBw_temp1;
  cudaMalloc(&d_propBw_temp1,(V+1)*(devicecount)*sizeof(bool));
  bool* d_propBw_temp2;
  cudaMalloc(&d_propBw_temp2,(V+1)*(devicecount)*sizeof(bool));
  bool** h_isPivot;
  h_isPivot = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_isPivot[i]=(bool*)malloc(sizeof(bool)*(V+1));
  }
  bool** d_isPivot;
  d_isPivot = (bool**)malloc(sizeof(bool*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_isPivot[i], sizeof(bool)*(V+1));
  }

  bool* h_isPivot_temp1 = (bool*)malloc((V+1)*(devicecount)*sizeof(bool));
  cudaSetDevice(0);
  bool* d_isPivot_temp1;
  cudaMalloc(&d_isPivot_temp1,(V+1)*(devicecount)*sizeof(bool));
  bool* d_isPivot_temp2;
  cudaMalloc(&d_isPivot_temp2,(V+1)*(devicecount)*sizeof(bool));
  int** h_scc;
  h_scc = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_scc[i]=(int*)malloc(sizeof(int)*(V+1));
  }
  int** d_scc;
  d_scc = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_scc[i], sizeof(int)*(V+1));
  }

  int* h_scc_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_scc_temp1;
  cudaMalloc(&d_scc_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_scc_temp2;
  cudaMalloc(&d_scc_temp2,(V+1)*(devicecount)*sizeof(int));
  int** h_range;
  h_range = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_range[i]=(int*)malloc(sizeof(int)*(V+1));
  }
  int** d_range;
  d_range = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_range[i], sizeof(int)*(V+1));
  }

  int* h_range_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_range_temp1;
  cudaMalloc(&d_range_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_range_temp2;
  cudaMalloc(&d_range_temp2,(V+1)*(devicecount)*sizeof(int));
  int** h_pivotField;
  h_pivotField = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_pivotField[i]=(int*)malloc(sizeof(int)*(V+1));
  }
  int** d_pivotField;
  d_pivotField = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_pivotField[i], sizeof(int)*(V+1));
  }

  int* h_pivotField_temp1 = (int*)malloc((V+1)*(devicecount)*sizeof(int));
  cudaSetDevice(0);
  int* d_pivotField_temp1;
  cudaMalloc(&d_pivotField_temp1,(V+1)*(devicecount)*sizeof(int));
  int* d_pivotField_temp2;
  cudaMalloc(&d_pivotField_temp2,(V+1)*(devicecount)*sizeof(int));
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_modified[i],(int)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_modified[i],d_modified[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_outDeg[i],(int)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_outDeg[i],d_outDeg[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_inDeg[i],(int)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_inDeg[i],d_inDeg[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitFw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_visitFw[i],d_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_visitBw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_visitBw[i],d_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propFw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_propFw[i],d_propFw[i],(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_propBw[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_propBw[i],d_propBw[i],(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_isPivot[i],(bool)false);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_isPivot[i],d_isPivot[i],(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_scc[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_scc[i],d_scc[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_range[i],(int)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_range[i],d_range[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_pivotField[i],(int)-1);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_pivotField[i],d_pivotField[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
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

  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_inDeg_temp1+i*(V+1),d_inDeg[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_inDeg_temp1,h_inDeg_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_inDeg_temp2+i*(V+1),h_inDeg[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_inDeg_temp1,d_inDeg_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_inDeg[i],d_inDeg_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_inDeg[i],h_inDeg[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_outDeg_temp1+i*(V+1),d_outDeg[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_outDeg_temp1,h_outDeg_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_outDeg_temp2+i*(V+1),h_outDeg[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_outDeg_temp1,d_outDeg_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_outDeg[i],d_outDeg_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_outDeg[i],h_outDeg[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  bool fpoint1 = false; // asst in .cu 
  bool** h_fpoint1;
  h_fpoint1 = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_fpoint1[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_fpoint1;
  d_fpoint1 = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_fpoint1[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_fpoint1[i],false);
  }


  int** d_modified_next;
  d_modified_next = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_modified_next[i], sizeof(int)*(V+1));
  }


  // FIXED POINT variables
  //BEGIN FIXED POINT
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
  }

  while(!fpoint1) {

    fpoint1 = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_fpoint1[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel2<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_range[i],d_scc[i],d_fpoint1[i],d_isPivot[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_scc_temp1+i*(V+1),d_scc[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_scc_temp1,h_scc_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_scc_temp2+i*(V+1),h_scc[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_scc_temp1,d_scc_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_scc[i],d_scc_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_scc[i],h_scc[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_isPivot_temp1+i*(V+1),d_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_isPivot_temp1,h_isPivot_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_isPivot_temp2+i*(V+1),h_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_isPivot_temp1,d_isPivot_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_isPivot[i],d_isPivot_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_isPivot[i],h_isPivot[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_fpoint1[i], d_fpoint1[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      fpoint1&=h_fpoint1[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_fpoint1[i],true);
    }

    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  } // END FIXED POINT

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    vHong_kernel3<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_outDeg[i],d_pivotField[i],d_inDeg[i],d_range[i]);
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }




  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_pivotField_temp1+i*(V+1),d_pivotField[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_pivotField_temp1,h_pivotField_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_pivotField_temp2+i*(V+1),h_pivotField[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_pivotField_temp1,d_pivotField_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_pivotField[i],d_pivotField_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_pivotField[i],h_pivotField[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    vHong_kernel4<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_pivotField[i],d_range[i],d_isPivot[i],d_visitBw[i],d_visitFw[i]);
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }


  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_visitFw_temp1+i*(V+1),d_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_visitFw_temp1,h_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_visitFw_temp2+i*(V+1),h_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitFw_temp1,d_visitFw_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_visitFw[i],d_visitFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_visitFw[i],h_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_visitBw_temp1+i*(V+1),d_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_visitBw_temp1,h_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_visitBw_temp2+i*(V+1),h_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitBw_temp1,d_visitBw_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_visitBw[i],d_visitBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_visitBw[i],h_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_isPivot_temp1+i*(V+1),d_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_isPivot_temp1,h_isPivot_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_isPivot_temp2+i*(V+1),h_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_isPivot_temp1,d_isPivot_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_isPivot[i],d_isPivot_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_isPivot[i],h_isPivot[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  bool fpoint2 = false; // asst in .cu 
  bool** h_fpoint2;
  h_fpoint2 = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_fpoint2[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_fpoint2;
  d_fpoint2 = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_fpoint2[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_fpoint2[i],false);
  }


  // FIXED POINT variables
  //BEGIN FIXED POINT
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
  }

  while(!fpoint2) {

    fpoint2 = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_fpoint2[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel5<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_propBw[i],d_visitBw[i],d_propFw[i],d_visitFw[i],d_scc[i],d_range[i],d_fpoint2[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_visitFw_temp1+i*(V+1),d_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_visitFw_temp1,h_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_visitFw_temp2+i*(V+1),h_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitFw_temp1,d_visitFw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_visitFw[i],d_visitFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_visitFw[i],h_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_propFw_temp1+i*(V+1),d_propFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_propFw_temp1,h_propFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_propFw_temp2+i*(V+1),h_propFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propFw_temp1,d_propFw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_propFw[i],d_propFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_propFw[i],h_propFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_visitBw_temp1+i*(V+1),d_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_visitBw_temp1,h_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_visitBw_temp2+i*(V+1),h_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitBw_temp1,d_visitBw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_visitBw[i],d_visitBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_visitBw[i],h_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_propBw_temp1+i*(V+1),d_propBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_propBw_temp1,h_propBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_propBw_temp2+i*(V+1),h_propBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propBw_temp1,d_propBw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_propBw[i],d_propBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_propBw[i],h_propBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_fpoint2[i], d_fpoint2[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      fpoint2&=h_fpoint2[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_fpoint2[i],true);
    }

    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  } // END FIXED POINT

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    vHong_kernel6<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_visitFw[i],d_visitBw[i],d_range[i],d_propBw[i],d_propFw[i]);
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }


  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_range_temp1+i*(V+1),d_range[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_range_temp1,h_range_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_range_temp2+i*(V+1),h_range[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_range_temp1,d_range_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_range[i],d_range_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_range[i],h_range[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_visitFw_temp1+i*(V+1),d_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_visitFw_temp1,h_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_visitFw_temp2+i*(V+1),h_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitFw_temp1,d_visitFw_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_visitFw[i],d_visitFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_visitFw[i],h_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_visitBw_temp1+i*(V+1),d_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_visitBw_temp1,h_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_visitBw_temp2+i*(V+1),h_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitBw_temp1,d_visitBw_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_visitBw[i],d_visitBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_visitBw[i],h_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_propFw_temp1+i*(V+1),d_propFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_propFw_temp1,h_propFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_propFw_temp2+i*(V+1),h_propFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propFw_temp1,d_propFw_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_propFw[i],d_propFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_propFw[i],h_propFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_propBw_temp1+i*(V+1),d_propBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_propBw_temp1,h_propBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_propBw_temp2+i*(V+1),h_propBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propBw_temp1,d_propBw_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_propBw[i],d_propBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_propBw[i],h_propBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_scc_temp1+i*(V+1),d_scc[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_scc_temp1,h_scc_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_scc_temp2+i*(V+1),h_scc[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_scc_temp1,d_scc_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_scc[i],d_scc_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_scc[i],h_scc[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  fpoint1 = false;
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    //printed here

    initKernel<bool> <<<1,1>>>(1,d_fpoint1[i],(bool)false);
  }
  // FIXED POINT variables
  //BEGIN FIXED POINT
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
  }

  while(!fpoint1) {

    fpoint1 = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_fpoint1[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel7<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_range[i],d_scc[i],d_fpoint1[i],d_isPivot[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_scc_temp1+i*(V+1),d_scc[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_scc_temp1,h_scc_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_scc_temp2+i*(V+1),h_scc[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_scc_temp1,d_scc_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_scc[i],d_scc_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_scc[i],h_scc[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_isPivot_temp1+i*(V+1),d_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_isPivot_temp1,h_isPivot_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_isPivot_temp2+i*(V+1),h_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_isPivot_temp1,d_isPivot_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_isPivot[i],d_isPivot_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_isPivot[i],h_isPivot[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_fpoint1[i], d_fpoint1[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      fpoint1&=h_fpoint1[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_fpoint1[i],true);
    }

    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  } // END FIXED POINT

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_range[i],(int)0);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_range[i],d_range[i],(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    vHong_kernel8<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_range[i]);
  }

  for(int i=0;i<devicecount;i++)
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(h_range_temp1+i*(V+1),d_range[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
  }
  cudaSetDevice(0);
  cudaMemcpy(d_range_temp1,h_range_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(d_range_temp2+i*(V+1),h_range[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  }
  Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_range_temp1,d_range_temp2,V,devicecount);
  for(int i=0;i<devicecount;i++){
    cudaMemcpy(h_range[i],d_range_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaMemcpyAsync(d_range[i],h_range[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
  }
  for(int i=0;i<devicecount;i++){
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }
  bool fpoint4 = false; // asst in .cu 
  bool** h_fpoint4;
  h_fpoint4 = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_fpoint4[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_fpoint4;
  d_fpoint4 = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_fpoint4[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_fpoint4[i],false);
  }


  // FIXED POINT variables
  //BEGIN FIXED POINT
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
  }

  while(!fpoint4) {

    fpoint4 = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_fpoint4[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel9<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_range[i],d_scc[i],d_fpoint4[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_range_temp1+i*(V+1),d_range[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_range_temp1,h_range_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_range_temp2+i*(V+1),h_range[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_range_temp1,d_range_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_range[i],d_range_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_range[i],h_range[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel10<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_range[i],d_fpoint4[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }

    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_range_temp1+i*(V+1),d_range[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_range_temp1,h_range_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_range_temp2+i*(V+1),h_range[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_range_temp1,d_range_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_range[i],d_range_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_range[i],h_range[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_fpoint4[i], d_fpoint4[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      fpoint4&=h_fpoint4[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_fpoint4[i],true);
    }

    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
  } // END FIXED POINT

  bool fpoint5 = false; // asst in .cu 
  bool** h_fpoint5;
  h_fpoint5 = (bool**)malloc(sizeof(bool*)*(devicecount+1));
  for(int i=0;i<=devicecount;i+=1){
    h_fpoint5[i] = (bool*)malloc(sizeof(bool));
  }

  bool** d_fpoint5;
  d_fpoint5 = (bool**)malloc(sizeof(bool*)*devicecount);
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    cudaMalloc(&d_fpoint5[i],sizeof(bool));
    initKernel<bool> <<<1,1>>>(1,d_fpoint5[i],false);
  }


  // FIXED POINT variables
  //BEGIN FIXED POINT
  for(int i = 0 ; i < devicecount ; i++){
    cudaSetDevice(i);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
  }

  while(!fpoint5) {

    fpoint5 = true;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      initKernel<bool><<<1,1>>>(1,d_fpoint5[i],(bool)true);
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel11<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_outDeg[i],d_pivotField[i],d_inDeg[i],d_range[i],d_fpoint5[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }




    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_pivotField_temp1+i*(V+1),d_pivotField[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_pivotField_temp1,h_pivotField_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_pivotField_temp2+i*(V+1),h_pivotField[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_pivotField_temp1,d_pivotField_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_pivotField[i],d_pivotField_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_pivotField[i],h_pivotField[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel12<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_pivotField[i],d_range[i],d_isPivot[i],d_visitBw[i],d_visitFw[i],d_fpoint5[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }


    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_visitFw_temp1+i*(V+1),d_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_visitFw_temp1,h_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_visitFw_temp2+i*(V+1),h_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitFw_temp1,d_visitFw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_visitFw[i],d_visitFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_visitFw[i],h_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_visitBw_temp1+i*(V+1),d_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_visitBw_temp1,h_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_visitBw_temp2+i*(V+1),h_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitBw_temp1,d_visitBw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_visitBw[i],d_visitBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_visitBw[i],h_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_isPivot_temp1+i*(V+1),d_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_isPivot_temp1,h_isPivot_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_isPivot_temp2+i*(V+1),h_isPivot[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_isPivot_temp1,d_isPivot_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_isPivot[i],d_isPivot_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_isPivot[i],h_isPivot[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    fpoint2 = false;
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      //printed here

      initKernel<bool> <<<1,1>>>(1,d_fpoint2[i],(bool)false);
    }
    // FIXED POINT variables
    //BEGIN FIXED POINT
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<int> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next[i], false);
    }

    while(!fpoint2) {

      fpoint2 = true;
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        initKernel<bool><<<1,1>>>(1,d_fpoint2[i],(bool)true);
      }
      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        vHong_kernel13<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_propBw[i],d_visitBw[i],d_propFw[i],d_visitFw[i],d_scc[i],d_range[i],d_fpoint2[i]);
      }

      for(int i=0;i<devicecount;i++)
      {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }

      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_visitFw_temp1+i*(V+1),d_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
      }
      cudaSetDevice(0);
      cudaMemcpy(d_visitFw_temp1,h_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(d_visitFw_temp2+i*(V+1),h_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
      }
      Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitFw_temp1,d_visitFw_temp2,V,devicecount);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(h_visitFw[i],d_visitFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_visitFw[i],h_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_propFw_temp1+i*(V+1),d_propFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
      }
      cudaSetDevice(0);
      cudaMemcpy(d_propFw_temp1,h_propFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(d_propFw_temp2+i*(V+1),h_propFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
      }
      Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propFw_temp1,d_propFw_temp2,V,devicecount);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(h_propFw[i],d_propFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_propFw[i],h_propFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_visitBw_temp1+i*(V+1),d_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
      }
      cudaSetDevice(0);
      cudaMemcpy(d_visitBw_temp1,h_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(d_visitBw_temp2+i*(V+1),h_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
      }
      Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitBw_temp1,d_visitBw_temp2,V,devicecount);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(h_visitBw[i],d_visitBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_visitBw[i],h_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_propBw_temp1+i*(V+1),d_propBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();
      }
      cudaSetDevice(0);
      cudaMemcpy(d_propBw_temp1,h_propBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(d_propBw_temp2+i*(V+1),h_propBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
      }
      Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propBw_temp1,d_propBw_temp2,V,devicecount);
      for(int i=0;i<devicecount;i++){
        cudaMemcpy(h_propBw[i],d_propBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(d_propBw[i],h_propBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaMemcpyAsync(h_fpoint2[i], d_fpoint2[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
      }
      for(int i=0;i<devicecount;i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
      for(int i=0;i<devicecount;i++){
        fpoint2&=h_fpoint2[i][0];
      }
      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        initKernel<bool> <<<1,1>>>(1,d_fpoint2[i],true);
      }

      for(int i = 0 ; i < devicecount ; i++){
        cudaSetDevice(i);
        cudaDeviceSynchronize();
      }
    } // END FIXED POINT

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      vHong_kernel14<<<numBlocks, threadsPerBlock>>>(h_vertex_partition[i],h_vertex_partition[i+1],V,E,d_offset[i],d_edges[i],d_weight[i],d_src[i],d_rev_meta[i],d_scc[i],d_visitFw[i],d_visitBw[i],d_range[i],d_propFw[i],d_propBw[i],d_fpoint5[i]);
    }

    for(int i=0;i<devicecount;i++)
    {
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }



    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_range_temp1+i*(V+1),d_range[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_range_temp1,h_range_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_range_temp2+i*(V+1),h_range[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_range_temp1,d_range_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_range[i],d_range_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_range[i],h_range[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_visitFw_temp1+i*(V+1),d_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_visitFw_temp1,h_visitFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_visitFw_temp2+i*(V+1),h_visitFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitFw_temp1,d_visitFw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_visitFw[i],d_visitFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_visitFw[i],h_visitFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_visitBw_temp1+i*(V+1),d_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_visitBw_temp1,h_visitBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_visitBw_temp2+i*(V+1),h_visitBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_visitBw_temp1,d_visitBw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_visitBw[i],d_visitBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_visitBw[i],h_visitBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_propFw_temp1+i*(V+1),d_propFw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_propFw_temp1,h_propFw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_propFw_temp2+i*(V+1),h_propFw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propFw_temp1,d_propFw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_propFw[i],d_propFw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_propFw[i],h_propFw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_propBw_temp1+i*(V+1),d_propBw[i],sizeof(bool)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_propBw_temp1,h_propBw_temp1,(V+1)*(devicecount)*sizeof(bool),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_propBw_temp2+i*(V+1),h_propBw[i],sizeof(bool)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<bool><<<numBlocks,threadsPerBlock>>>(d_propBw_temp1,d_propBw_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_propBw[i],d_propBw_temp1+i*(V+1),(V+1)*sizeof(bool),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_propBw[i],h_propBw[i],(V+1)*sizeof(bool),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_scc_temp1+i*(V+1),d_scc[i],sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    }
    cudaSetDevice(0);
    cudaMemcpy(d_scc_temp1,h_scc_temp1,(V+1)*(devicecount)*sizeof(int),cudaMemcpyHostToDevice);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(d_scc_temp2+i*(V+1),h_scc[i],sizeof(int)*(V+1),cudaMemcpyHostToDevice);
    }
    Compute_correct<int><<<numBlocks,threadsPerBlock>>>(d_scc_temp1,d_scc_temp2,V,devicecount);
    for(int i=0;i<devicecount;i++){
      cudaMemcpy(h_scc[i],d_scc_temp1+i*(V+1),(V+1)*sizeof(int),cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(d_scc[i],h_scc[i],(V+1)*sizeof(int),cudaMemcpyHostToDevice);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaMemcpyAsync(h_fpoint5[i], d_fpoint5[i], sizeof(bool)*1, cudaMemcpyDeviceToHost);
    }
    for(int i=0;i<devicecount;i++){
      cudaSetDevice(i);
      cudaDeviceSynchronize();
    }
    for(int i=0;i<devicecount;i++){
      fpoint5&=h_fpoint5[i][0];
    }
    for(int i = 0 ; i < devicecount ; i++){
      cudaSetDevice(i);
      initKernel<bool> <<<1,1>>>(1,d_fpoint5[i],true);
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
