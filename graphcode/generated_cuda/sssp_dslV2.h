// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SSSP_DSLV2_H
#define GENCPP_SSSP_DSLV2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_SSSP(graph& g,int* dist,int src);

__device__ int src ;


__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void Compute_SSSP_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_modified,int* d_dist){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned pp = blockIdx.x * blockDim.x + threadIdx.x;
  if(pp >= V) return;
  if (d_modified[pp] == true){ // if filter begin 
    for (int edge = d_meta[pp]; edge < d_meta[pp+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e = edge;
       int dist_new = d_dist[pp] + d_weight[e];
      bool modified_new = true;
      if(d_dist[pp]!= INT_MAX && d_dist[nbr] > dist_new)
      {
        atomicMin(&d_dist[nbr],dist_new);
        d_modified_next[nbr] = modified_new;
        finished = false ;
      }

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC

#endif
