// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_GRAPHCOLORINGNEW_H
#define GENCPP_GRAPHCOLORINGNEW_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void colorGraph(graph& g);



__device__ int numNodes ; // DEVICE ASSTMENT in .h

__device__ int fpoint1 ; // DEVICE ASSTMENT in .h

__device__ int diff ; // DEVICE ASSTMENT in .h

__device__ int diff_old ; // DEVICE ASSTMENT in .h

__device__ int cnt ; // DEVICE ASSTMENT in .h

__device__ int iter ; // DEVICE ASSTMENT in .h

__global__ void colorGraph_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_modified,long* d_color,bool* d_modified_next){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_modified[v] == false){ // if filter begin 
    int cnt = 0; // DEVICE ASSTMENT in .h

    int total = 0; // DEVICE ASSTMENT in .h

    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      total = total + 1;
      if (nbr == v){ // if filter begin 
        cnt = cnt + 1;

      } // if filter end
      else
      if (d_modified[nbr] == true){ // if filter begin 
        cnt = cnt + 1;

      } // if filter end
      else
      if (d_color[v] > d_color[nbr]){ // if filter begin 
        cnt = cnt + 1;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (cnt == total){ // if filter begin 
      d_modified_next[v] = true;
      atomicAdd(& fpoint1, (int)1);

    } // if filter end

  } // if filter end
} // end KER FUNC

#endif
