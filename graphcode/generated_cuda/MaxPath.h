// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MAXPATH_H
#define GENCPP_MAXPATH_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void computeMaxPath(graph& g,int* dist,std::set<int>& l);



__device__ int x ; // DEVICE ASSTMENT in .h

__device__ double*  s1 ; // DEVICE ASSTMENT in .h

__device__ long*  s1 ; // DEVICE ASSTMENT in .h

__device__ std::set<int>& l ; // DEVICE ASSTMENT in .h

__global__ void computeMaxPath_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  x = x + 1; //InitIndex
} // end KER FUNC

#endif
