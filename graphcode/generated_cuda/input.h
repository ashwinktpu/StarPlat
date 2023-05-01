// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_INPUT_H
#define GENCPP_INPUT_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void test(graph& g);



__device__ int x ; // DEVICE ASSTMENT in .h

__global__ void test_kernel(int V, int E,int* d_prop){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  atomicAdd(& x, (int)d_prop[v]);
} // end KER FUNC

#endif
