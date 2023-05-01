// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_EDMONDS-KARP_CU_H
#define GENCPP_EDMONDS-KARP_CU_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void ek(graph& g,int s,int d,int* weight
);



__device__ int flow ; // DEVICE ASSTMENT in .h

__device__ int new_flow ; // DEVICE ASSTMENT in .h

__device__ int cnew_flow ; // DEVICE ASSTMENT in .h

__device__ int e_cap ; // DEVICE ASSTMENT in .h

__device__ bool terminate ; // DEVICE ASSTMENT in .h

__global__ void merged_kernel_1(unsigned V, int* array_1, int val_1, int* array_2, int val_2, bool* array_3, bool val_3){
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V) {
        array_1[id] = val_1;
        array_2[id] = val_2;
        array_3[id] = val_3;
    }
}
__global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, double* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_weight) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if(d_level[v] == *d_hops_from_source) {
    if (d_in_bfs[cur] == true && terminate == false){ // if filter begin 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int nbr = d_data[edge];
        int e = edge;
        if (d_par[nbr] == -1 && d_cap[e] > 0){ // if filter begin 
          d_par[nbr] = cur;
          cnew_flow = d_n_flow[cur];
          if (cnew_flow > d_cap[e]){ // if filter begin 
            cnew_flow = d_cap[e];

          } // if filter end
          if (nbr == d){ // if filter begin 
            new_flow = cnew_flow;
            terminate = true;

          } // if filter end
          d_in_bfs[nbr] = true;
          d_n_flow[nbr] = cnew_flow;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    } // if filter end
  } // end if d lvl
} // kernel end

__global__ void back_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, double* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_weight) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  auto grid = cooperative_groups::this_grid();
  if(d_level[v] == *d_hops_from_source-1) {
  } // end if d lvl
} // kernel end


#endif
