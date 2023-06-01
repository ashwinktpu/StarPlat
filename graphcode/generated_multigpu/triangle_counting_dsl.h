// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_TRIANGLE_COUNTING_DSL_H
#define GENCPP_TRIANGLE_COUNTING_DSL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Compute_TC(graph& g);



__global__ void Compute_TC_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,long* d_triangle_count){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
      int u = d_data[edge];
      if (u < v){ // if filter begin 
        for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
          int w = d_data[edge];
          if (w > v){ // if filter begin 
            if (findNeighborSorted(u, w, d_meta, d_data)){ // if filter begin 
              atomicAdd((unsigned long long*) &d_triangle_count[0], (unsigned long long)1);
            } // if filter end
          } // if filter end
        } //  end FOR NBR ITR. TMP FIX!
      } // if filter end
    } //  end FOR NBR ITR. TMP FIX!
  }
} // end KER FUNC

#endif
