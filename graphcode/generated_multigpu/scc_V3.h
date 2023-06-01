// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SCC_V3_H
#define GENCPP_SCC_V3_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Compute_SSSP(graph& g,int* dist,int src);



__global__ void Compute_SSSP_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_x){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
      int nbr = d_data[edge];
      d_x[0] = d_x[0] + 1;
    } //  end FOR NBR ITR. TMP FIX!
  }
} // end KER FUNC

#endif
