// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_PATIL_H
#define GENCPP_PATIL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void mf(graph& g);



__global__ void mf_kernel(int V, int E,int* d_indeg,int* d_indeg_prev){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  initIndex<int><<<1,1>>>(V,d_indeg,u,(int)); //InitIndexDevice
  initIndex<int><<<1,1>>>(V,d_indeg_prev,u,(int)d_indeg[u]); //InitIndexDevice
} // end KER FUNC
__global__ void mf_kernel(int V, int E, int* d_meta, int* d_data,int* d_indeg,int* d_indeg_prev){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_indeg_prev[u] > k){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int v = d_data[edge];
      if (d_indeg_prev[v] <= k && d_indeg[u] > k){ // if filter begin 
        d_indeg[u]--
      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__global__ void mf_kernel(int V, int E,int* d_indeg_prev,int* d_indeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_indeg_prev[u] > k){ // if filter begin 
    initIndex<int><<<1,1>>>(V,d_indeg,u,(int)0); //InitIndexDevice

  } // if filter end
} // end KER FUNC

#endif
