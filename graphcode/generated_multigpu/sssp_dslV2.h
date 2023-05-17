// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SSSP_DSLV2_H
#define GENCPP_SSSP_DSLV2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Compute_SSSP(graph& g,int* dist,int src);



__global__ void Compute_SSSP_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_dist,bool* d_modified1,bool* d_modified1_next,bool* d_finished){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
      int nbr = d_data[edge];
      if (d_modified1[nbr] == true){ // if filter begin 
        int e = edge;
         int dist_new = d_dist[nbr] + d_weight[e];
        bool modified1_new = true;
        if(d_dist[nbr]!= INT_MAX && d_dist[v] > dist_new)
        {
          atomicMin(&d_dist[v],dist_new);
          d_modified1_next[v] = true ;
          d_finished[0] = false ;
        }
      } // if filter end
    } //  end FOR NBR ITR. TMP FIX!
  }
} // end KER FUNC

#endif
