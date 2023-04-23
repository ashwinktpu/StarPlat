// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_K-CORE_H
#define GENCPP_K-CORE_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void k_core(graph& g,int k);

__device__ int k ;


__device__ int kcore_count ; // DEVICE ASSTMENT in .h

__device__ int update_count ; // DEVICE ASSTMENT in .h

__global__ void k_core_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_indeg,int* d_indeg_prev){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  initIndex<int><<<1,1>>>(V,d_indeg,v,(int)(d_meta[nbr+1]-d_meta[nbr])); //InitIndexDevice
  initIndex<int><<<1,1>>>(V,d_indeg_prev,v,(int)d_indeg[v]); //InitIndexDevice
} // end KER FUNC
__global__ void k_core_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_indeg_prev,int* d_indeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_indeg_prev[v] >= k){ // if filter begin 
    int f = v;
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      if (d_indeg_prev[nbr] < k && d_indeg[f] >= k){ // if filter begin 
        d_indeg[f]--
      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__global__ void k_core_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_indeg_prev,int* d_indeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_indeg_prev[v] < k){ // if filter begin 
    d_indeg[v] = 0;

  } // if filter end
} // end KER FUNC
__global__ void k_core_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_indeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_indeg[v] < k && d_indeg[v] != 0){ // if filter begin 
    int f = v;
    d_indeg[v] = 0;
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      if (d_indeg[nbr] >= k){ // if filter begin 
        d_indeg[nbr]--update_count++
      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__global__ void k_core_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_indeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_indeg[v] >= k){ // if filter begin 
    kcore_count++
  } // if filter end
} // end KER FUNC

#endif
