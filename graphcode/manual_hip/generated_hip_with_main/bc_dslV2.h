#ifndef GENHIP_BC_DSLV2_H
#define GENHIP_BC_DSLV2_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void Compute_BC(
  graph& g,
  float* hBC,
  std::set<int>& sourceSet
);

__device__ 
bool IsAnEdge(const int, const int, const int*, const int*);

__global__ 
void ForwardBfsKernel(
  int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed, float *dBC, float *dSigma
) {

  unsigned dV = threadIdx.x + blockIdx.x * blockDim.x;

  if(dV >= V) {
    return;
  }

  if(dD[dV] == *dLevel) {

    for(int edge = dOffsetArray[dV]; edge < dOffsetArray[dV + 1]; edge++) {
      int dW = dEdgelist[edge];
      if (dD[dW] == -1) {
        dD[dW] = *dLevel + 1;
        *dIsAllNodesTraversed = false;
      }

      if (dD[dW] == *dLevel + 1) {
        atomicAdd(&dSigma[dV], dSigma[dW]);
      }
    }
  }
}

__global__ 
void ReverseBfsKernel(
  int V, int *dOffsetArray, int *dEdgelist, int *dD, int *dLevel, bool *dIsAllNodesTraversed, float *dBC, float *dDelta, float *dSigma
) {

  unsigned dV = threadIdx.x + blockIdx.x * blockDim.x;

  if(dV >= V) {
    return;
  }

  auto grid = cooperative_groups::this_grid();

  if(dD[dV] == *dLevel - 1) {
    for(int edge = dOffsetArray[dV]; edge < dOffsetArray[dV + 1]; edge++) {
      int dW = dEdgelist[edge];
      if (dD[dW] == *dLevel) {
        atomicAdd(&dDelta[dV], (dSigma[dV] / dSigma[dW]) * (1 + dDelta[dW]));
      }
    }

    grid.sync();
    dBC[dV] = dBC[dV] + dDelta[dV];
  }
}

template <typename T>
__global__
void initArray(const unsigned V, T *dArray, T value) {
  unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if(idx < V) {
    dArray[idx] = value;
  }
}

template <typename T>
__global__
void initIndex(const unsigned V, T* dArray, T value, int index) {
  if(index < V) {
    dArray[index] = value;
  }
}


#endif
