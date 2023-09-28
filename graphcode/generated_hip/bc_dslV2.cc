#include "bc_dslV2.h"


void Compute_BC(
  graph& dG,
  float* dBC,
  std::set<int>& dSourceSet
)
{

  int* dOffsetArray;
  int* dEdgelist;
  int *dWeight;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dEdgelist, sizeof(int) * (E));
  hipMalloc(&dWeight, sizeof(int) * (E));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof( int) * (V + 1), hipMemcpyHostToDevice);

