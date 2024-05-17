#include "sssp_dslV3.h"

using std::cout;
using std::endl;

void Compute_SSSP(
  graph& g,
  int* hDist,
  int* hWeight,
  int src
) {

  int V = g.num_nodes();
  int E = g.num_edges();

  int *hOffsetArray;
  int *hEdgelist;

  hOffsetArray = (int*) malloc(sizeof(int) * (V + 1));
  hEdgelist = (int*) malloc(sizeof(int) * (E));

  for(int i = 0; i <= V; i++) {
    hOffsetArray[i] = g.indexofNodes[i];
  }
  for(int i = 0; i < E; i++) {
    hEdgelist[i] = g.edgeList[i];
  }


  int *dOffsetArray;
  int *dEdgelist;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dEdgelist, sizeof(int) * (E));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dEdgelist, hEdgelist, sizeof(int) * (E), hipMemcpyHostToDevice);

  int *dDist;
  hipMalloc(&dDist, sizeof(int) * (V));
  hipMemcpy(dDist, hDist, sizeof(int) * (V), hipMemcpyHostToDevice);
  int *dWeight;
  hipMalloc(&dWeight, sizeof(int) * (E));
  hipMemcpy(dWeight, hWeight, sizeof(int) * (E), hipMemcpyHostToDevice);

  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? 512 : V;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  bool *dModified;
  // COMMENT
  hipMalloc(&dModified, sizeof(bool) * (V));
  bool *dModified_nxt;
  // COMMENT
  hipMalloc(&dModified_nxt, sizeof(bool) * (V));
  initArray<int><<<numBlocks, numThreads>>>(V, dDist, INT_MAX);
  initArray<bool><<<numBlocks, numThreads>>>(V, dModified, false);
  initArray<bool><<<numBlocks, numThreads>>>(V, dModified_nxt, false);
  initIndex<bool><<<1,1>>>(V, dModified, true, src);
  initIndex<int><<<1,1>>>(V, dDist, 0, src);
  bool hFinished = false;
  bool *dFinished;
  hipMalloc(&dFinished, sizeof(bool));
  int k = 0;
  hipDeviceSynchronize();

  while(!hFinished) {

    hFinished = true;
    hipMemcpy(dFinished, &hFinished, sizeof(bool) * (1), hipMemcpyHostToDevice);
    Compute_SSSPKernel<<<numBlocks, numThreads>>>(V, E, dOffsetArray, dEdgelist, dModified, dWeight, dDist, dModified_nxt, dFinished);
    hipDeviceSynchronize();


    hipMemcpy(dModified, dModified_nxt, sizeof(bool) * (V), hipMemcpyDeviceToDevice);
    initArray<bool><<<numBlocks, numThreads>>>(V, dModified_nxt, false);

    hipMemcpy(&hFinished, dFinished, sizeof(bool) * (1), hipMemcpyDeviceToHost);
    k++;
  }
  hipMemcpy(hDist, dDist, sizeof(int) * (V), hipMemcpyDeviceToHost);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: ./program <input_file>" << std::endl;
    return 1;
  }

  std::string inputFileName = argv[1];

  graph g(argv[1]);
  g.parseGraph();

  int V = g.num_nodes();
  int E = g.num_edges();
  int* hDist = new int[V];
  int* hWeight = new int[E];

  cout << "Number of nodes: " << V << endl;
  cout << "Number of edges: " << E << endl;

  int src = 0;

  // Compute SSSP
  Compute_SSSP(g, hDist, g.getEdgeLen(), src);

  for (int i = 0; i < V; i++) {
    std::cout << "Distance from source to node " << i << ": " << hDist[i] << std::endl;
  }

  // Clean up memory
  delete[] hDist;
  delete[] hWeight;

  return 0;
}