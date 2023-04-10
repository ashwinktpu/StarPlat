// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_DSL_H
#define GENCPP_MST_DSL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Boruvka(graph& g);



__global__ void Boruvka_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_color,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( u < num_vertices) {
    u+=start;
    //hi2
    d_nodeId[u] = u; //InitIndex
    //hi2
    d_color[u] = u; //InitIndex
  }
} // end KER FUNC

#endif
