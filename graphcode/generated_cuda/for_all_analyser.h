// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_FOR_ALL_ANALYSER_H
#define GENCPP_FOR_ALL_ANALYSER_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graphcode/graph.hpp"
#include "graphcode/libcuda.cuh"
#include <cooperative_groups.h>
void for_loop(graph& g,std::set<int>& sourceSet);



__device__ int count ; // DEVICE ASSTMENT in .h

__global__ void for_loop_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_count_prop){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  atomicAdd(& count, (int)d_count_prop[v]);
} // end KER FUNC

#endif
