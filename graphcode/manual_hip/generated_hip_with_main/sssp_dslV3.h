#ifndef GENHIP_SSSP_DSLV3_H
#define GENHIP_SSSP_DSLV3_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void Compute_SSSP(
  graph& g,
  int* hDist,
  int* hWeight,
  int src
);

__device__ 
bool IsAnEdge(const int, const int, const int*, const int*);


__global__
void Compute_SSSPKernel(int V, int E, int *dOffsetArray, int *dEdgelist, bool* dModified, int* dWeight, int* dDist, bool* dModified_nxt, bool *dFinished) {

  unsigned dV = blockIdx.x * blockDim.x + threadIdx.x;

  if (dV >= V) {
    return;
  }

  if (dModified[dV] == true) {
    for(int edge = dOffsetArray[dV]; edge < dOffsetArray[dV + 1]; edge++) {
      int dNbr = dEdgelist[edge];
      int dE = edge;
      int dUpdatedDist = dDist[dV] + dWeight[dE];
      bool dUpdatedModified_nxt = true;
      if(dDist[dV] != INT_MAX && dDist[dNbr] > dUpdatedDist) {
        atomicMin(&dDist[dNbr], dUpdatedDist);
        dModified_nxt[dNbr] = dUpdatedModified_nxt;
        *dFinished = false;
      }
    }
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
