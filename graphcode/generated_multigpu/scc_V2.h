// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SCC_V2_H
#define GENCPP_SCC_V2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void vHong(graph& g);



__global__ void vHong_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_outDeg,int* d_inDeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    int nxtSrc = src + 1; // asst in .cu 
    d_inDeg[src] = d_inDeg[src]+ d_rev_meta[nxtSrc] - d_rev_meta[src];
    d_outDeg[src] = d_outDeg[src]+ d_meta[nxtSrc] - d_meta[src];
  }
} // end KER FUNC

#endif
