#ifndef GENHIP_PAGERANKDSLV3_H
#define GENHIP_PAGERANKDSLV3_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void ComputePageRank(
  graph& g,
  float hBeta,
  float hDelta,
  int hMaxIter,
  float* hPageRank
);

__device__ 
bool IsAnEdge(const int, const int, const int*, const int*);


__global__
void PrintArrayKernel(float* array, int size) {
    printf("Array: ");
    for (int i = 0; i < size; i++) {
        printf("%f ", array[i]);
    }
    printf("\n");
}

__global__
void ComputePageRankKernel(int V, int E, int *dOffsetArray, int *dSrcList, int *dRevOffsetArray, float *dDiff, float *dDelta, float *dNumNodes, float* dPageRank, float* dPageRankNext) {

  unsigned dV = blockIdx.x * blockDim.x + threadIdx.x;

  if (dV >= V) {
    return;
  }

  float dSum = 0;
  for(int edge = dRevOffsetArray[dV]; edge < dRevOffsetArray[dV + 1]; edge++) {
    int dNbr = dSrcList[edge];
    dSum = dSum + dPageRank[dNbr] / (dOffsetArray[dNbr + 1] - dOffsetArray[dNbr]);
  }
  float dNewPageRank = (1 - (*dDelta)) / (*dNumNodes) + (*dDelta) * dSum;
  if (dNewPageRank - dPageRank[dV] >= 0) {
    atomicAdd((float*) dDiff, (float) dNewPageRank - dPageRank[dV]);
  } else {
    atomicAdd((float*) dDiff, (float) dPageRank[dV] - dNewPageRank);
  }
  dPageRankNext[dV] = dNewPageRank;
  printf("dV: %d, dSum: %f dNewPageRank: %f, dPageRank[%d]: %f, dDiff: %f\n", dV, dSum, dNewPageRank, dV, dPageRank[dV], *dDiff);

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
void initIndex(const unsigned V, T* dArray, int index, T value) {
  if(index < V) {
    dArray[index] = value;
  }
}


#endif
