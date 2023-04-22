// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SSSP_DSLV3_H
#define GENCPP_SSSP_DSLV3_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_SSSP(graph& g,int* dist,int* weight,int src
);



__global__ void merged_kernel_1(unsigned V, int* array_1, int val_1, bool* array_2, bool val_2, bool* array_3, bool val_3, bool* array_4,int index_4, bool val_4, int* array_5,int index_5, int val_5){
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V) {
        array_1[id] = val_1;
        array_2[id] = val_2;
        array_3[id] = val_3;
    }
    if(index_4==id){
        array_4[index_4] = val_4;
    }
    if(index_5==id){
        array_5[index_5] = val_5;
    }
}
; // DEVICE ASSTMENT in .h

__global__ void Compute_SSSP_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_modified,int* d_weight,int* d_dist,bool* d_modified_nxt){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_modified[v] == true){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e = edge;
       int dist_new = d_dist[v] + d_weight[e];
      bool modified_nxt_new = true;
      if(d_dist[v]!= INT_MAX && d_dist[nbr] > dist_new)
      {
        atomicMin(&d_dist[nbr],dist_new);
        d_modified_nxt[nbr] = modified_nxt_new;
      }

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC

#endif
