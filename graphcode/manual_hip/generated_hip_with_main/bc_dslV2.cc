#include "bc_dslV2.h"


void Compute_BC(
  graph& g,
  float* hBC,
  std::set<int>& sourceSet
) {

  int V = g.num_nodes();
  int E = g.num_edges();

  int *edgeLens = g.getEdgeLen();

  int *hOffsetArray;
  int *hEdgelist;
  int *hWeight;

  hOffsetArray = (int*) malloc(sizeof(int) * (V + 1));
  hEdgelist = (int*) malloc(sizeof(int) * (E));
  hWeight = (int*) malloc(sizeof(int) * (E));

  for(int i = 0; i <= V; i++) {
    hOffsetArray[i] = g.indexofNodes[i];
  }
  for(int i = 0; i < E; i++) {
    hEdgelist[i] = g.edgeList[i];
    hWeight[i] = edgeLens[i];
  }


  int *dOffsetArray;
  int *dEdgelist;
  int *dWeight;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dEdgelist, sizeof(int) * (E));
  hipMalloc(&dWeight, sizeof(int) * (E));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dEdgelist, hEdgelist, sizeof(int) * (E), hipMemcpyHostToDevice);
  hipMemcpy(dWeight, hWeight, sizeof(int) * (E), hipMemcpyHostToDevice);

  float *dBC;
  hipMalloc(&dBC, sizeof(float) * (1));
  hipMemcpy(dBC, hBC, sizeof(float) * (V), hipMemcpyHostToDevice);

  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? 512 : V;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  initArray<float><<<numBlocks, numThreads>>>(V, dBC, 0);
  float *dSigma;
  // COMMENT
  hipMalloc(&dSigma, sizeof(float*) * (V));
  float *dDelta;
  // COMMENT
  hipMalloc(&dDelta, sizeof(float*) * (V));

  for(auto itr = sourceSet.begin(); itr != sourceSet.end(); itr++) 
  {
    int src = *itr;
    initArray<float><<<numBlocks, numThreads>>>(V, dDelta, 0);
    initArray<float><<<numBlocks, numThreads>>>(V, dSigma, 0);
    initIndex<float><<<1,1>>>(V, dSigma, src, 1);

    // Additional variables added for BC. Not present in the DSL
    bool hIsAllNodesTraversed; /*Non-DSL variable*/
    bool *dIsAllNodesTraversed; /*Non-DSL variable*/
    int hLevel; /*Non-DSL variable*/ // Indicates the level of the BFS
    int *dLevel; /*Non-DSL variable*/
    // Explore the possibility of using hipHostMalloc for simple vars
    // Check if hipHostMalloc works on all platforms
    int *dD; /*Non-DSL variable*/
    hipMalloc(&dIsAllNodesTraversed, sizeof(bool) * (1));
    hipMalloc(&dLevel, sizeof(int) * (1));
    hipMalloc(&dD, sizeof(int) * (V));
    initIndex<int><<<1, 1>>>(V, dLevel, 0, 0);
    initArray<int><<<numBlocks, numThreads>>>(V, dD, -1);
    initIndex<int><<<1, 1>>>(V, dD, 0, src);

    do {

      hIsAllNodesTraversed = true;
      hipMemcpy(dIsAllNodesTraversed, &hIsAllNodesTraversed, sizeof(bool) * (1), hipMemcpyHostToDevice);
      ForwardBfsKernel<<<numBlocks, numThreads>>>(
        V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed, dBC, dSigma
      );
      hLevel++;
      hipMemcpy(&hLevel, dLevel, sizeof(int) * (1), hipMemcpyDeviceToHost);

      hipMemcpy(&hIsAllNodesTraversed, dIsAllNodesTraversed, sizeof(bool) * (1), hipMemcpyDeviceToHost);
    } while(!hIsAllNodesTraversed);
    hLevel--;
    hipMemcpy(dLevel, &hLevel, sizeof(int) * (1), hipMemcpyHostToDevice);

    while(hLevel > 0) {

      ReverseBfsKernel<<<numBlocks, numThreads>>>(
        V, dOffsetArray, dEdgelist, dD, dLevel, dIsAllNodesTraversed, dBC, dDelta, dSigma
      );
      hLevel--;
      hipMemcpy(dLevel, &hLevel, sizeof(int) * (1), hipMemcpyHostToDevice);
    }



  }

}
