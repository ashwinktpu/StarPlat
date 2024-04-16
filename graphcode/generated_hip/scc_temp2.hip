#include "scc_temp2.h"


void scc_fw(
  graph&  g)
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


  int *dInDeg;
  hipMalloc(&dInDeg, sizeof(int) * (V));
  int *dOutDeg;
  hipMalloc(&dOutDeg, sizeof(int) * (V));
  bool *dVisit;
  hipMalloc(&dVisit, sizeof(bool) * (V));
  bool *dVisitFw;
  hipMalloc(&dVisitFw, sizeof(bool) * (V));
  bool *dVisitBw;
  hipMalloc(&dVisitBw, sizeof(bool) * (V));
  int *dVisitLevelFw;
  hipMalloc(&dVisitLevelFw, sizeof(int) * (V));
  int *dVisitLevelBw;
  hipMalloc(&dVisitLevelBw, sizeof(int) * (V));
  initKernel<int><<<numBlocks, numThreads>>>(V, dInDeg, 0);
  initKernel<int><<<numBlocks, numThreads>>>(V, dOutDeg, 0);
  initKernel<bool><<<numBlocks, numThreads>>>(V, dVisit, false);
  initKernel<bool><<<numBlocks, numThreads>>>(V, dVisitFw, false);
  initKernel<bool><<<numBlocks, numThreads>>>(V, dVisitBw, false);
  bool *dModified;
  hipMalloc(&dModified, sizeof(bool) * (V));
  bool *dModifiedNext;hipMalloc(&dModifiedNext, sizeof(bool) * (V));
  initKernel<bool><<<numBlocks, numThreads>>>(V, dModified, false);
  bool noNewComp = false;
  int sccCount = 0;
  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, dModifiedNext, false);
  int k=0; // #fixpt-Iterations
  while(!noNewComp) {
    noNewComp = true;
    hipMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, hipMemcpyHostToDevice);
    bool noNewNode = false;
    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, dModifiedNext, false);
    int k=0; // #fixpt-Iterations
    while(!noNewNode) {
      noNewNode = true;
      hipMemcpyToSymbol(::noNewNode, &noNewNode, sizeof(bool), 0, hipMemcpyHostToDevice);
      hipMemcpyToSymbol(::sccCount, &sccCount, sizeof(int), 0, hipMemcpyHostToDevice);
      hipMemcpyToSymbol(::noNewNode, &noNewNode, sizeof(bool), 0, hipMemcpyHostToDevice);
      scc_fw_kernel0<<<numBlocks, threadsPerBlock>>>(V, E, dOffsetArray, dEdgelist, dSrcList,dRevOffsetArray, dVisit, dOutDeg, dInDeg);
      hipDeviceSynchronize();
      hipMemcpyFromSymbol(&sccCount, ::sccCount, sizeof(int), 0, hipMemcpyDeviceToHost);
      hipMemcpyFromSymbol(&noNewNode, ::noNewNode, sizeof(bool), 0, hipMemcpyDeviceToHost);








      hipMemcpyFromSymbol(&noNewNode, ::noNewNode, sizeof(bool), 0, hipMemcpyDeviceToHost);
      hipMemcpy(dModified, dModifiedNext, sizeof(bool)*V, hipMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, dModifiedNext, false);
      k++;
    } // END FIXED POINT

    int nodeToBeVisited = -1;
    int maxDegree = -1;
    hipMemcpyToSymbol(::maxDegree, &maxDegree, sizeof(int), 0, hipMemcpyHostToDevice);
    scc_fw_kernel1<<<numBlocks, threadsPerBlock>>>(V, E, dVisit, dInDeg, dOutDeg);
    hipDeviceSynchronize();
    hipMemcpyFromSymbol(&maxDegree, ::maxDegree, sizeof(int), 0, hipMemcpyDeviceToHost);

    hipMemcpyToSymbol(::maxDegree, &maxDegree, sizeof(int), 0, hipMemcpyHostToDevice);
    hipMemcpyToSymbol(::nodeToBeVisited, &nodeToBeVisited, sizeof(int), 0, hipMemcpyHostToDevice);
    scc_fw_kernel2<<<numBlocks, threadsPerBlock>>>(V, E, dVisit, dInDeg, dOutDeg);
    hipDeviceSynchronize();
    hipMemcpyFromSymbol(&maxDegree, ::maxDegree, sizeof(int), 0, hipMemcpyDeviceToHost);
    hipMemcpyFromSymbol(&nodeToBeVisited, ::nodeToBeVisited, sizeof(int), 0, hipMemcpyDeviceToHost);

    initKernel<int><<<numBlocks, numThreads>>>(V, dVisitLevelFw, 0);
    initKernel<int><<<numBlocks, numThreads>>>(V, dVisitLevelBw, 0);
    hipMemcpyToSymbol(::nodeToBeVisited, &nodeToBeVisited, sizeof(int), 0, hipMemcpyHostToDevice);
    scc_fw_kernel3<<<numBlocks, threadsPerBlock>>>(V, E, dVisitLevelBw, dVisitLevelFw);
    hipDeviceSynchronize();
    hipMemcpyFromSymbol(&nodeToBeVisited, ::nodeToBeVisited, sizeof(int), 0, hipMemcpyDeviceToHost);


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
      fwd_pass<<<numBlocks,threadsPerBlock>>>(V, dOffsetArray, dEdgelist, dWeight, dFinished, dVisitBfs, dVisitLevelFw, dVisit, dVisitFw); ///DONE from varList



      hipDeviceSynchronize();

      hipMemcpy(&finished, dFinished, sizeof(bool) * (1), hipMemcpyDeviceToHost);
    }while(!finished);

    //EXTRA vars for ITBFS AND REVBFS
    bool finished2;
    bool* dFinished2;
    hipMalloc(&dFinished2,sizeof(bool) *(1));
    bool *dVisitBfs2;
    hipMalloc(&dVisitBfs2, sizeof(bool) * (V));
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, dVisitBfs2, false);
    do {
      finished2 = true;
      hipMemcpy(dFinished2, &finished2, sizeof(bool) * (1), hipMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass2<<<numBlocks,threadsPerBlock>>>(V, dRevOffsetArray, dSrcList, dWeight, dFinished2, dVisitBfs2, dVisitLevelBw, dVisit, dVisitFw, dVisitBw); ///DONE from varList



      hipDeviceSynchronize();

      hipMemcpy(&finished2, dFinished2, sizeof(bool) * (1), hipMemcpyDeviceToHost);
    }while(!finished2);
    hipMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, hipMemcpyHostToDevice);
    scc_fw_kernel4<<<numBlocks, threadsPerBlock>>>(V, E, dVisit, dVisitFw, dVisitBw);
    hipDeviceSynchronize();
    hipMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, hipMemcpyDeviceToHost);


    if (noNewComp == false) {
      sccCount = sccCount + 1;
    } 


    hipMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, hipMemcpyDeviceToHost);
    hipMemcpy(dModified, dModifiedNext, sizeof(bool)*V, hipMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, dModifiedNext, false);
    k++;
  } // END FIXED POINT


  //hipFree up!! all propVars in this BLOCK!
  hipFree(dModified);
  hipFree(dVisitLevelBw);
  hipFree(dVisitLevelFw);
  hipFree(dVisitBw);
  hipFree(dVisitFw);
  hipFree(dVisit);
  hipFree(dOutDeg);
  hipFree(dInDeg);


  hipEventRecord(stop,0);
  hipEventSynchronize(stop);
  hipEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
}
