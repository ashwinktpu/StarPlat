// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_PAGERANKDSLV2_H
#define GENCPP_PAGERANKDSLV2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank);






__global__ void Compute_PR_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,float* d_delta,float* d_num_nodes,float* d_pageRank,float* d_pageRank_nxt){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    float sum = 0.000000; // asst in .cu 
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++){
      int nbr = d_src[edge] ;
      sum = sum + d_pageRank[nbr] / (d_meta[nbr+1]-d_meta[nbr]);
    } //  end FOR NBR ITR. TMP FIX!
    float val = (1 - d_delta[0]) / d_num_nodes[0] + d_delta[0] * sum; // asst in .cu 
    d_pageRank_nxt[v] = val;
  }
} // end KER FUNC

#endif
