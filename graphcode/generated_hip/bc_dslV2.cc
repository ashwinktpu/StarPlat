#include "bc_dslV2.h"


void Compute_BC(
  graph& dG,
  float* dBC,
  std::set<int>& dSourceSet
)
{
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
    hOffsetArray[i] = g.indexOfNodes[i];
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

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof( int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dEdgelist, hEdgelist, sizeof( int) * (E), hipMemcpyHostToDevice);
  hipMemcpy(dWeight, hWeight, sizeof( int) * (E), hipMemcpyHostToDevice);


  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? 512 : V;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  float * dBC;
  hipMalloc(&dBC, sizeof(float) * (V));


}
