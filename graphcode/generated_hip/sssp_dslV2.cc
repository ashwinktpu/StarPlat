#include "sssp_dslV2.h"


void Compute_SSSP(
  graph& dG,
  int* dDist,
  int* dWeight,
  int dSrc
)
{
  int V = g.num_nodes();
  int E = g.num_edges();


  int *hOffsetArray;
  int *hEdgelist;

  hOffsetArray = (int*) malloc(sizeof(int) * (V + 1));
  hEdgelist = (int*) malloc(sizeof(int) * (E));

  for(int i = 0; i <= V; i++) {
    hOffsetArray[i] = g.indexOfNodes[i];
  }
  for(int i = 0; i < E; i++) {
    hEdgelist[i] = g.edgeList[i];
  }


  int *dOffsetArray;
  int *dEdgelist;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dEdgelist, sizeof(int) * (E));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof( int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dEdgelist, hEdgelist, sizeof( int) * (E), hipMemcpyHostToDevice);


  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? 512 : V;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  int * dDist;
  hipMalloc(&ddist, sizeof(int) * (V));
  int * dWeight;
  hipMalloc(&dweight, sizeof(int) * (E));

  bool *dModified1;
  hipMalloc(&dModified1, sizeof(bool) * (V));
  bool *dModified1Next;hipMalloc(&dModified1Next, sizeof(bool) * (V));

}
