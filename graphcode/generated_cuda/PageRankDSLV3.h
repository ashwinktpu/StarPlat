// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_PAGERANKDSLV3_H
#define GENCPP_PAGERANKDSLV3_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void ComputePageRank(graph& g,float beta,float delta,int maxIter,
  float* pageRank);

__device__ float beta ;
__device__ float delta ;
__device__ int maxIter ;


__device__ float numNodes ; // DEVICE ASSTMENT in .h

__device__ int iterCount ; // DEVICE ASSTMENT in .h

__device__ float diff ; // DEVICE ASSTMENT in .h

__global__ void ComputePageRank_kernel(int V, int E, int* d_meta, int* d_src, int *d_rev_meta,float* d_pageRank){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  float sum = 0.000000; // DEVICE ASSTMENT in .h

  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {
    int nbr = d_src[edge] ;
    sum = sum + d_pageRank[nbr] / (d_meta[nbr+1]-d_meta[nbr]);

  } //  end FOR NBR ITR. TMP FIX!
  float newPageRank = (1 - delta) / numNodes + delta * sum; // DEVICE ASSTMENT in .h

  diff = diff + newPageRank - d_pageRank[v];
  d_pageRank[v] = newPageRank;
} // end KER FUNC

#endif
