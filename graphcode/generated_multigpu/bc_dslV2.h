// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_BC_DSLV2_H
#define GENCPP_BC_DSLV2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet);



__global__ void fwd_pass(int n,int device,int start,int end,int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,float* d_BC) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  v = v + start;
  if(v >= n) return;
  if ( v >= start && v < end ) {
    if(d_level[v] == -1) {
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
        int w = d_data[edge];
        if(d_level[w] == *d_hops_from_source) {
          d_level[v] = *d_hops_from_source+1;
          *d_finished = false;
          atomicAdd(&d_sigma[v],  d_sigma[w]);
        }
      }
    }
  } // end if d lvl
} // kernel end

__global__ void back_pass(int n,int device,int start,int end,int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,float* d_BC) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if(d_level[v] == *d_hops_from_source-1) {
    if ( v >= start && v < end) {
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { 
        int w = d_data[edge];
        if(d_level[w] == *d_hops_from_source) {
          atomicAdd(&d_delta[v],  (d_sigma[v] / d_sigma[w]) * (1 + d_delta[w]));
        } // end IF  
      } // end FOR
      d_BC[v] = d_BC[v] + d_delta[v];
    } // end if d lvl
  } // end if start <= v && v < end
} // kernel end


#endif
