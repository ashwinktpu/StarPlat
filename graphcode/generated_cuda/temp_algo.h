// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_TEMP_ALGO_H
#define GENCPP_TEMP_ALGO_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void count_nodes(graph& g);



__device__ int y ; // DEVICE ASSTMENT in .h

__global__ void count_nodes_kernel(int V, int E,int* d_prop){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  initIndex<int><<<1,1>>>(V,d_prop,v,(int)y); //InitIndexDevice
} // end KER FUNC

#endif
