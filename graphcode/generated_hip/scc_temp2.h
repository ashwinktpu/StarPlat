#ifndef GENHIP_SCC_TEMP2_H
#define GENHIP_SCC_TEMP2_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void scc_fw(
  graph&  g);


__device__ bool noNewComp; // DEVICE ASSTMENT in .h
__device__ int sccCount; // DEVICE ASSTMENT in .h
__device__ bool noNewNode; // DEVICE ASSTMENT in .h
__global__ void scc_fw_kernel0(int V, int E, int* dOffsetArray, int* dEdgelist, int* dSrcList, int *dRevOffsetArray, bool* dVisit, int* dOutDeg, int* dInDeg){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (dVisit[v] == false) {
    int outNbrCount = 0;
    // DEVICE ASSTMENT in .h
    int inNbrCount = 0;
    // DEVICE ASSTMENT in .h
    for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR 
      int dst = dEdgelist[edge];
      if (dVisit[dst] == false) {
        outNbrCount = outNbrCount + 1;

      } 

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++)
    {
      int dst = dSrcList[edge] ;
      if (dVisit[dst] == false) {
        inNbrCount = inNbrCount + 1;

      } 

    } //  end FOR NBR ITR. TMP FIX!
    if (inNbrCount == 0 || outNbrCount == 0) {
      dVisit[v] = true;
      atomicAdd(&sccCount, 1);
      sccCount = sccCount;
      noNewNode = false;

    } 
    dInDeg[v] = inNbrCount;
    dOutDeg[v] = outNbrCount;

  } 
} // end KER FUNC

__device__ int nodeToBeVisited; // DEVICE ASSTMENT in .h
__device__ int maxDegree; // DEVICE ASSTMENT in .h
__global__ void scc_fw_kernel1(int V, int E, bool* dVisit, int* dInDeg, int* dOutDeg){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (dVisit[v] == false) {
    int neighbourCount = dInDeg[v] + dOutDeg[v];
    // DEVICE ASSTMENT in .h
    if (maxDegree < neighbourCount) {
      maxDegree = neighbourCount;
    } 

  } 
} // end KER FUNC

__global__ void scc_fw_kernel2(int V, int E, bool* dVisit, int* dInDeg, int* dOutDeg){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (dVisit[v] == false) {
    int curNodeDegree = dInDeg[v] + dOutDeg[v];
    // DEVICE ASSTMENT in .h
    if (curNodeDegree == maxDegree) {
      nodeToBeVisited = v;
    } 

  } 
} // end KER FUNC

__global__ void scc_fw_kernel3(int V, int E, int* dVisitLevelBw, int* dVisitLevelFw){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == nodeToBeVisited) {
    dVisitLevelFw[v] = 1;
    dVisitLevelBw[v] = 1;

  } 
} // end KER FUNC

__global__ void fwd_pass(int n, int* dOffsetArray, int* dEdgelist, int* dWeight, bool* dFinished, bool* dVisitBfs, int* dVisitLevelFw, bool* dVisit, bool* dVisitFw) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if (dVisitLevelFw[v] == 1 && dVisit[v] == false) {
    dVisitFw[v] = true;
    for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR 
      int w = dEdgelist[edge];
      if (dVisitFw[w] == false && dVisitLevelFw[w] == 0) {
        dVisitLevelFw[w] = 1;

      } 

    } //  end FOR NBR ITR. TMP FIX!
    dVisitLevelFw[v] = 2;

  }
  else
    return;
  dVisitBfs[v] = true;
  for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR 
    int w = dEdgelist[edge];
    if (dVisitBfs[w] == false) {
      *dFinished = false;
      dVisitBfs[w] = true;
    }
  }
} // kernel end

__global__ void fwd_pass2(int n, int* dRevOffsetArray, int* dSrcList, int* dWeight, bool* dFinished2, bool* dVisitBfs2, int* dVisitLevelBw, bool* dVisit, bool* dVisitFw, bool* dVisitBw) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  //Before for loop
  //Generating a statement
  if (dVisitLevelBw[v] == 1 && dVisit[v] == false && dVisitFw[v] == true) {
    dVisitBw[v] = true;
    for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++)
    {
      int w = dSrcList[edge] ;
      if (dVisitBw[w] == false && dVisitLevelBw[w] == 0) {
        dVisitLevelBw[w] = 1;

      } 

    } //  end FOR NBR ITR. TMP FIX!
    dVisitLevelBw[v] = 2;

  }
  else
    return;
  //After for loop
  dVisitBfs2[v] = true;
  for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++) { // FOR NBR REV ITR 
    int w = dSrcList[edge];
    if (dVisitBfs2[w] == false) {
      *dFinished2 = false;
      dVisitBfs2[w] = true;
    }
  }
} // kernel end

__global__ void scc_fw_kernel4(int V, int E, bool* dVisit, bool* dVisitFw, bool* dVisitBw){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (dVisit[v] == false && dVisitFw[v] && dVisitBw[v]) {
    dVisit[v] = true;
    noNewComp = false;

  }  else {
    dVisitFw[v] = false;
    dVisitBw[v] = false;

  }
} // end KER FUNC

template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T init_value) {  // intializes one 1D array with init val
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (id < V) {
    init_array[id] = init_value;
  }
}

#endif
