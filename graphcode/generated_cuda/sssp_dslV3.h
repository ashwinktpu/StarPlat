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



__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void Compute_SSSP_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_dist,bool* d_modified){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {
    int nbr = d_src[edge] ;
    if (d_modified[nbr] == true){ // if filter begin 
      int e = edge;
       int dist_new = d_dist[nbr] + d_weight[e];
      bool modified_new = true;
      if(d_dist[nbr]!= INT_MAX && d_dist[v] > dist_new)
      {
        atomicMin(&d_dist[v],dist_new);
        d_modified_next[v] = modified_new;
        finished = false ;
      }

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC

#endif
