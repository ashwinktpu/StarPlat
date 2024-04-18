#ifndef GENHIP_TRIANGLE_COUNTING_DSL_H
#define GENHIP_TRIANGLE_COUNTING_DSL_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

long Compute_TC(
  graph& g
);

__device__ 
bool IsAnEdge(const int, const int, const int*, const int*);


__global__
void Compute_TCKernel(int V, int E, int *dOffsetArray, int *dEdgelist, long *dTriangle_count) {

  unsigned dV = blockIdx.x * blockDim.x + threadIdx.x;

  if (dV >= V) {
    return;
  }

  for(int edge = dOffsetArray[dV]; edge < dOffsetArray[dV + 1]; edge++) {
    int dU = dEdgelist[edge];
    //printf("dU: %d\n", dU);
    if (dU < dV) {
      for(int edge = dOffsetArray[dV]; edge < dOffsetArray[dV + 1]; edge++) {
        int dW = dEdgelist[edge];
        //printf("dW: %d\n", dW);
        if (dW > dV) {
          //printf("dV%d ", dV);
          if (IsAnEdge(dU, dW, dOffsetArray, dEdgelist)) {
            atomicAdd((unsigned long long*) dTriangle_count, (unsigned long long) 1);
            //printf("INININ %d\n", *dTriangle_count);
          } 
        } 
      }
      //printf("Wd\n");
    }
    //printf("Ud\n");
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

__device__
bool IsAnEdge(const int s, const int d, const int* dOffsetArray, const int *dEdgeList) {

  int startEdge = dOffsetArray[s];
  int endEdge = dOffsetArray[s + 1] - 1;

  if (dEdgeList[startEdge] == d) {
    return true;
  }

  if (dEdgeList[endEdge] == d) {
    return true;
  }

  int mid = (startEdge + endEdge) / 2;
  if(threadIdx.x == 1)
  while (startEdge <= endEdge) {

    //printf("s: %d, d: %d, startEdge: %d, endEdge: %d, mid: %d, dStartEdge: %d, dEndEdge: %d, dMidEdge: %d\n", s, d, startEdge, endEdge, mid, dEdgeList[startEdge], dEdgeList[endEdge], dEdgeList[mid]);
    if (dEdgeList[mid] == d) {
      return true;
    }

    if (d < dEdgeList[mid]) {
      endEdge = mid - 1;
    } else {
      startEdge = mid + 1;
    }
    
    mid = (startEdge + endEdge) / 2;
  }

  return false;
}

#endif
