// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_CC_H
#define GENCPP_CC_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Compute_CC(graph& g,float* CC,std::set<int>& sourceSet);



__global__ void Compute_CC_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_dist,bool* d_modified,bool* d_modified_next,bool* d_finished){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++){
      int nbr = d_src[edge] ;
      if (d_modified[nbr] == true){ // if filter begin 
        int e = edge;
         int dist_new = d_dist[nbr] + d_weight[e];
        bool modified_new = true;
        if(d_dist[nbr]!= INT_MAX && d_dist[v] > dist_new)
        {
          atomicMin(&d_dist[v],dist_new);
          d_modified_next[v] = true ;
          d_finished[0] = false ;
        }
      } // if filter end
    } //  end FOR NBR ITR. TMP FIX!
  }
} // end KER FUNC
__global__ void Compute_CC_kernel2(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_temp,int* d_dist){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    if (d_dist[v] != INT_MAX){ // if filter begin 
      atomicAdd(&d_temp[0], (int)d_dist[v]);
    } // if filter end
  }
} // end KER FUNC

#endif
