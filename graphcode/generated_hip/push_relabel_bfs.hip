#include "push_relabel_bfs.h"


void do_max_flow(
  graph&  g,int  source,int  sink,int  kernel_parameter
)
{
  int V = g.num_nodes();
  int E = g.num_edges();

  int *edgeLens = g.getEdgeLen();

  int *hOffsetArray;
  int *hEdgelist;
  int *hSrcList;
  int *hWeight;
  int *hRevOffsetArray;

  hOffsetArray = (int*) malloc(sizeof(int) * (V + 1));
  hEdgelist = (int*) malloc(sizeof(int) * (E));
  hSrcList = (int*) malloc(sizeof(int) * (E));
  hWeight = (int*) malloc(sizeof(int) * (E));
  hRevOffsetArray = (int*) malloc(sizeof(int) * (V + 1));

  for(int i = 0; i <= V; i++) {
    hOffsetArray[i] = g.indexofNodes[i];
    hRevOffsetArray[i] = g.rev_indexofNodes[i];
  }
  for(int i = 0; i < E; i++) {
    hEdgelist[i] = g.edgeList[i];
    hSrcList[i] = g.srcList[i];
    hWeight[i] = edgeLens[i];
  }


  int *dOffsetArray;
  int *dEdgelist;
  int *dSrcList;
  int *dWeight;
  int *dRevOffsetArray;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dEdgelist, sizeof(int) * (E));
  hipMalloc(&dSrcList, sizeof(int) * (E));
  hipMalloc(&dWeight, sizeof(int) * (E));
  hipMalloc(&dRevOffsetArray, sizeof(int) * (V + 1));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dEdgelist, hEdgelist, sizeof(int) * (E), hipMemcpyHostToDevice);
  hipMemcpy(dSrcList, hSrcList, sizeof(int) * (E), hipMemcpyHostToDevice);
  hipMemcpy(dWeight, hWeight, sizeof(int) * (E), hipMemcpyHostToDevice);
  hipMemcpy(dRevOffsetArray, hRevOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);


  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? V : 512;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  hipEvent_t start, stop;
  hipEventCreate(&start);
  hipEventCreate(&stop);
  float milliseconds = 0;
  hipEventRecord(start,0);


  int *dLabel;
  hipMalloc(&dLabel, sizeof(int) * (V));
  initKernel<int><<<numBlocks, numThreads>>>(V, dLabel, 0);
  initIndex<int><<<1,1>>>(V,dLabel,source,(int)); //InitIndexDevice
  int *dExcess;
  hipMalloc(&dExcess, sizeof(int) * (V));
  initKernel<int><<<numBlocks, numThreads>>>(V, dExcess, 0);
  int *dResidual_capacity;
  hipMalloc(&dResidual_capacity, sizeof(int) * (E));
  int excesstotal = 0;  int *dVisit;
  hipMalloc(&dVisit, sizeof(int) * (V));
  hipMemcpyToSymbol(::source, &source, sizeof(int), 0, hipMemcpyHostToDevice);
  hipMemcpyToSymbol(::excesstotal, &excesstotal, sizeof(int), 0, hipMemcpyHostToDevice);
  do_max_flow_kernel0<<<numBlocks, threadsPerBlock>>>(V, E, dOffsetArray, dEdgelist, dResidual_capacity, dExcess);
  hipDeviceSynchronize();
  hipMemcpyFromSymbol(&source, ::source, sizeof(int), 0, hipMemcpyDeviceToHost);
  hipMemcpyFromSymbol(&excesstotal, ::excesstotal, sizeof(int), 0, hipMemcpyDeviceToHost);


  bool flag = true;  do{
    hipMemcpyToSymbol(::source, &source, sizeof(int), 0, hipMemcpyHostToDevice);
    hipMemcpyToSymbol(::sink, &sink, sizeof(int), 0, hipMemcpyHostToDevice);
    hipMemcpyToSymbol(::kernel_parameter, &kernel_parameter, sizeof(int), 0, hipMemcpyHostToDevice);
    do_max_flow_kernel1<<<numBlocks, threadsPerBlock>>>(V, E, dOffsetArray, dEdgelist, dExcess, dLabel, dResidual_capacity);
    hipDeviceSynchronize();
    hipMemcpyFromSymbol(&source, ::source, sizeof(int), 0, hipMemcpyDeviceToHost);
    hipMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, hipMemcpyDeviceToHost);
    hipMemcpyFromSymbol(&kernel_parameter, ::kernel_parameter, sizeof(int), 0, hipMemcpyDeviceToHost);








    do_max_flow_kernel2<<<numBlocks, threadsPerBlock>>>(V, E, dOffsetArray, dEdgelist, dResidual_capacity, dExcess, dLabel);
    hipDeviceSynchronize();


    initKernel<int><<<numBlocks, numThreads>>>(V, dVisit, 0);
    dLabel[sink] = 0;
    dVisit[sink] = 1;

    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    bool* dFinished;
    hipMalloc(&dFinished,sizeof(bool) *(1));
    bool *dVisitBfs;
    hipMalloc(&dVisitBfs, sizeof(bool) * (V));
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, dVisitBfs, false);
    do {
      finished = true;
      hipMemcpy(dFinished, &finished, sizeof(bool) * (1), hipMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass<<<numBlocks,threadsPerBlock>>>(V, dOffsetArray, dEdgelist, dRevOffsetArray, dSrcList, dWeight, dFinished, dVisitBfs, dVisit, dResidual_capacity, dLabel); ///DONE from varList



      hipDeviceSynchronize();

      hipMemcpy(&finished, dFinished, sizeof(bool) * (1), hipMemcpyDeviceToHost);
    }while(!finished);
    hipMemcpyToSymbol(::excesstotal, &excesstotal, sizeof(int), 0, hipMemcpyHostToDevice);
    do_max_flow_kernel3<<<numBlocks, threadsPerBlock>>>(V, E, dVisit, dExcess, dLabel);
    hipDeviceSynchronize();
    hipMemcpyFromSymbol(&excesstotal, ::excesstotal, sizeof(int), 0, hipMemcpyDeviceToHost);

    if (excess[source] + excess[sink] == excesstotal) {
      flag = false;


    } 


  }while(flag);

  //hipFree up!! all propVars in this BLOCK!
  hipFree(dVisit);
  hipFree(dExcess);
  hipFree(dLabel);


  hipEventRecord(stop,0);
  hipEventSynchronize(stop);
  hipEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
}
