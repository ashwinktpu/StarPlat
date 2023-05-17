// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_BP_H
#define GENCPP_BP_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void bp(graph& g,int k,int* weight);

__device__ int k ;


__device__ int iter ; // DEVICE ASSTMENT in .h

__global__ void merged_kernel_1(unsigned V, int* array_1, int val_1, int* array_2, int val_2){
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V) {
        array_1[id] = val_1;
        array_2[id] = val_2;
    }
}
__global__ void bp_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_weight,int* d_prior_prob,int* d_cur_prob){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int new_prob = 0; // DEVICE ASSTMENT in .h

  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    int e = edge;
    atomicAdd(& new_prob, (int)d_weight[e] * d_prior_prob[v]);

  } //  end FOR NBR ITR. TMP FIX!
  d_cur_prob[v] = new_prob;
} // end KER FUNC
__global__ void bp_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_cur_prob,int* d_prior_prob){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  d_prior_prob[v] = d_cur_prob[v];
} // end KER FUNC

#endif
