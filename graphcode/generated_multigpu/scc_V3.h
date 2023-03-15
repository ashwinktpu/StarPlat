// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SCC_V3_H
#define GENCPP_SCC_V3_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Hong(graph& g);



__global__ void Hong_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,bool* d_fpoint1){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int ifIn = 0; // asst in .cu 
      int ifOut = 0; // asst in .cu 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
        int dst = d_src[edge] ;
        if (d_scc[dst] == -1){ // if filter begin 
          ifIn = 1;
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          ifOut = 1;
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      if ((ifIn == 0) || (ifOut == 0)){ // if filter begin 
        d_scc[src] = src;
        fpoint1 = false;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void Hong_kernel2(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,int* d_visitBw,int* d_visitFw,bool* d_fpoint2){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      d_visitFw[src] = src;
      d_visitBw[src] = src;
    } // if filter end
  }
} // end KER FUNC
__global__ void Hong_kernel3(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_visitFw,int* d_scc,bool* d_fpoint3){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_visitFw[dst] > d_visitFw[src]){ // if filter begin 
            d_visitFw[dst] = d_visitFw[src];
            fpoint3 = false;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
    } // if filter end
  }
} // end KER FUNC
__global__ void Hong_kernel4(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_visitBw,int* d_scc,bool* d_fpoint3){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
        int dst = d_src[edge] ;
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_visitBw[dst] > d_visitBw[src]){ // if filter begin 
            d_visitBw[dst] = d_visitBw[src];
            fpoint3 = false;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
    } // if filter end
  }
} // end KER FUNC
__global__ void Hong_kernel5(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_visitFw,int* d_scc,int* d_visitBw,bool* d_fpoint2){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( v < num_vertices) {
    v+=start;
    if (d_scc[v] == -1){ // if filter begin 
      if (d_visitFw[v] == d_visitBw[v]){ // if filter begin 
        d_scc[v] = d_visitFw[v];
        fpoint2 = false;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void Hong_kernel6(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,bool* d_fpoint1){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int ifIn = 0; // asst in .cu 
      int ifOut = 0; // asst in .cu 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
        int dst = d_src[edge] ;
        if (d_scc[dst] == -1){ // if filter begin 
          ifIn = 1;
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          ifOut = 1;
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      if ((ifIn == 0) || (ifOut == 0)){ // if filter begin 
        d_scc[src] = src;
        fpoint1 = false;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC

#endif
