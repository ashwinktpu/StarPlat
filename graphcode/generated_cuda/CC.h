// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_CC_H
#define GENCPP_CC_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_CC(graph& g,float* CC);



; // DEVICE ASSTMENT in .h

__device__ int i ; // DEVICE ASSTMENT in .h

; // DEVICE ASSTMENT in .h

__global__ void Compute_CC_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_modified,int* d_dist){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned p = blockIdx.x * blockDim.x + threadIdx.x;
  if(p >= V) return;
  if (d_modified[p] == true){ // if filter begin 
    for (int edge = d_meta[p]; edge < d_meta[p+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e = edge;
       int dist_new = d_dist[p] + d_weight[e];
      bool modified_new = true;
      if(d_dist[p]!= INT_MAX && d_dist[nbr] > dist_new)
      {
        atomicMin(&d_dist[nbr],dist_new);
        d_modified_next[nbr] = modified_new;
        finished = false ;
      }

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__global__ void Compute_CC_kernel_2(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_dist,float* d_CC){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned dst = blockIdx.x * blockDim.x + threadIdx.x;
  if(dst >= V) return;
  if (d_dist[dst] != INT_MAX){ // if filter begin 
    d_CC[src] = d_CC[src] + 1.000000 / d_dist[dst];

  } // if filter end
} // end KER FUNC
__global__ void Compute_CC_kernel_3(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next){ // BEGIN KER FUN via ADDKERNEL
  float num_edges  = E;
  unsigned edg = blockIdx.x * blockDim.x + threadIdx.x;
  if(edg >= E) return;
  i = i + 1;
} // end KER FUNC

#endif
