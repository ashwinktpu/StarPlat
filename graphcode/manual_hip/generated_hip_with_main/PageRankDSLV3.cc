#include "PageRankDSLV3.h"


void ComputePageRank(
  graph& g,
  float hBeta,
  float hDelta,
  int hMaxIter,
  float* hPageRank
) {

  int V = g.num_nodes();
  int E = g.num_edges();

  int *hOffsetArray;
  int *hSrcList;
  int *hRevOffsetArray;

  hOffsetArray = (int*) malloc(sizeof(int) * (V + 1));
  hSrcList = (int*) malloc(sizeof(int) * (E));
  hRevOffsetArray = (int*) malloc(sizeof(int) * (V + 1));

  for(int i = 0; i <= V; i++) {
    hOffsetArray[i] = g.indexofNodes[i];
    hRevOffsetArray[i] = g.rev_indexofNodes[i];
  }
  for(int i = 0; i < E; i++) {
    hSrcList[i] = g.srcList[i];
  }


  int *dOffsetArray;
  int *dSrcList;
  int *dRevOffsetArray;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dSrcList, sizeof(int) * (E));
  hipMalloc(&dRevOffsetArray, sizeof(int) * (V + 1));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dSrcList, hSrcList, sizeof(int) * (E), hipMemcpyHostToDevice);
  hipMemcpy(dRevOffsetArray, hRevOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);

  float *dBeta;
  hipMalloc(&dBeta, sizeof(float) * (1));
  hipMemcpy(dBeta, &hBeta, sizeof(float) * (1), hipMemcpyHostToDevice);
  float *dDelta;
  hipMalloc(&dDelta, sizeof(float) * (1));
  hipMemcpy(dDelta, &hDelta, sizeof(float) * (1), hipMemcpyHostToDevice);
  int *dMaxIter;
  hipMalloc(&dMaxIter, sizeof(int) * (1));
  hipMemcpy(dMaxIter, &hMaxIter, sizeof(int) * (1), hipMemcpyHostToDevice);
  float *dPageRank;
  hipMalloc(&dPageRank, sizeof(float) * (V));
  hipMemcpy(dPageRank, hPageRank, sizeof(float) * (V), hipMemcpyHostToDevice);

  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? 512 : V;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  float hNumNodes = g.num_nodes();
  float *dNumNodes;
  hipMalloc(&dNumNodes, sizeof(float));
  float *dPageRankNext;
  // COMMENT
  hipMalloc(&dPageRankNext, sizeof(float*) * (V));
  initArray<float><<<numBlocks, numThreads>>>(V, dPageRank, 1 / hNumNodes);
  initArray<float><<<numBlocks, numThreads>>>(V, dPageRankNext, 0);
  int hIterCount = 0;
  int *dIterCount;
  hipMalloc(&dIterCount, sizeof(int));
  float hDiff;
  float *dDiff;
  hipMalloc(&dDiff, sizeof(float));
  do {
    hDiff = 0;
    hipMemcpy(dDiff, &hDiff, sizeof(float) * (1), hipMemcpyHostToDevice);
    hipMemcpy(dIterCount, &hIterCount, sizeof(int) * (1), hipMemcpyHostToDevice);
    hipMemcpy(dDelta, &hDelta, sizeof(float) * (1), hipMemcpyHostToDevice);
    hipMemcpy(dNumNodes, &hNumNodes, sizeof(float) * (1), hipMemcpyHostToDevice);
    ComputePageRankKernel<<<numBlocks, numThreads>>>(V, E, dOffsetArray, dSrcList, dRevOffsetArray, dDiff, dDelta, dNumNodes, dPageRank, dPageRankNext);
    hipDeviceSynchronize();
    hipMemcpy(&hDiff, dDiff, sizeof(float) * (1), hipMemcpyDeviceToHost);
    hipMemcpy(&hDelta, dDelta, sizeof(float) * (1), hipMemcpyDeviceToHost);
    hipMemcpy(&hNumNodes, dNumNodes, sizeof(float) * (1), hipMemcpyDeviceToHost);

    hipMemcpy(dPageRank, dPageRankNext, sizeof(float) * (V), hipMemcpyDeviceToDevice);
    hIterCount++;
    PrintArrayKernel<<<1, 1>>>(dPageRank, V);
    hipDeviceSynchronize();
    std::cout << "iter: " << hIterCount << " hDiff: " << hDiff << " hBeta: " << hBeta << std::endl;

  } while((hDiff > hBeta) && (hIterCount < hMaxIter));

  hipMemcpy(hPageRank, dPageRank, sizeof(float) * (V), hipMemcpyDeviceToHost);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: ./program <filename>" << std::endl;
    return 1;
  }

  char* filepath = argv[1];
  graph g(filepath);
  g.parseGraph();

  float beta = 0.000000000000001;
  float delta = 0.85;
  int maxIter = 100;

  int V = g.num_nodes();
  float* hPageRank = (float*)malloc(sizeof(float) * V);

  ComputePageRank(g, beta, delta, maxIter, hPageRank);

  // Print the page rank values
  for (int i = 0; i < V; i++) {
    std::cout << "Page Rank value for node " << i << ": " << hPageRank[i] << std::endl;
  }

  return 0;
}