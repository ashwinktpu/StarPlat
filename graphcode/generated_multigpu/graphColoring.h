// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_GRAPHCOLORING_H
#define GENCPP_GRAPHCOLORING_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void colorGraph(graph& g);



__global__ void colorGraph_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,bool* d_modified,int* d_fpoint1,long* d_color,bool* d_modified_next){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    if (d_modified[v] == false){ // if filter begin 
      int cnt = 0; // asst in .cu 
      int total = 0; // asst in .cu 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
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
        atomicAdd(&d_fpoint1[0], (int)1);
      } // if filter end
    } // if filter end
  }
} // end KER FUNC

#endif
