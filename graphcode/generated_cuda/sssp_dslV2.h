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



__global__ void merged_kernel_1(unsigned V, int* array_1, int val_1, bool* array_2, bool val_2, bool* array_3,int index_3, bool val_3, int* array_4,int index_4, int val_4){
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V) {
        array_1[id] = val_1;
        array_2[id] = val_2;
    }
    if(index_3==id){
        array_3[index_3] = val_3;
    }
    if(index_4==id){
        array_4[index_4] = val_4;
    }
}
__device__ bool finished ; // DEVICE ASSTMENT in .h

; // DEVICE ASSTMENT in .h

; // DEVICE ASSTMENT in .h

__global__ void merged_kernel_2(unsigned V,bool array_1,int index_1,bool val_1,int array_2,int index_2,int val_2){
    if(index_1 < V){
        array_1[index_1] = val_1
    }
    if(index_2 < V){
        array_2[index_2] = val_2
    }
}
__global__ void Compute_SSSP_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_modified,int* d_dist){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_modified[v] == true){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e = edge;
       int dist_new = d_dist[v] + d_weight[e];
      bool modified_new = true;
      if(d_dist[v]!= INT_MAX && d_dist[nbr] > dist_new)
      {
        atomicMin(&d_dist[nbr],dist_new);
        d_modified_next[nbr] = modified_new;
        finished = false ;
      }

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC

#endif
