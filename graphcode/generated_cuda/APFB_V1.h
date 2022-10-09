// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_APFB_V1_H
#define GENCPP_APFB_V1_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void APFB(graph& g,int nc);

__device__ int nc ;


__device__ bool noNewPaths ; // DEVICE ASSTMENT in .h

__device__ int L0 ; // DEVICE ASSTMENT in .h

__device__ int NOT_VISITED ; // DEVICE ASSTMENT in .h

__global__ void APFB_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_cmatch,int* d_bfsArray){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned c = blockIdx.x * blockDim.x + threadIdx.x;
  if(c >= V) return;
  if (c < nc){ // if filter begin 
    if (d_cmatch[c] == -1){ // if filter begin 
      d_bfsArray[c] = L0;

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ int bfsLevel ; // DEVICE ASSTMENT in .h

__device__ bool noNewVertices ; // DEVICE ASSTMENT in .h

__global__ void APFB_kernel_2(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_rmatch,int* d_bfsArray,int* d_predeccesor){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned col_vertex = blockIdx.x * blockDim.x + threadIdx.x;
  if(col_vertex >= V) return;
  if (col_vertex < nc){ // if filter begin 
    if (d_bfsArray[col_vertex] == bfsLevel){ // if filter begin 
      for (int edge = d_meta[col_vertex]; edge < d_meta[col_vertex+1]; edge++) { // FOR NBR ITR 
        int neigh_row = d_data[edge];
        int col_match = d_rmatch[neigh_row]; // DEVICE ASSTMENT in .h

        if (col_match > -1){ // if filter begin 
          if (d_bfsArray[col_match] == NOT_VISITED){ // if filter begin 
            noNewVertices = false;
            d_bfsArray[col_match] = bfsLevel + 1;
            d_predeccesor[neigh_row] = col_vertex;

          } // if filter end

        } // if filter end
        if (col_match == -1){ // if filter begin 
          d_rmatch[neigh_row] = -2;
          d_predeccesor[neigh_row] = col_vertex;
          noNewPaths = false;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void APFB_kernel_3(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_rmatch,bool* d_compress){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned r = blockIdx.x * blockDim.x + threadIdx.x;
  if(r >= V) return;
  if (r >= nc && d_rmatch[r] == -2){ // if filter begin 
    d_compress[r] = true;

  } // if filter end
} // end KER FUNC
__device__ bool compressed ; // DEVICE ASSTMENT in .h

__global__ void APFB_kernel_4(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_predeccesor,bool* d_compress,int* d_cmatch,bool* d_compress_next,int* d_rmatch){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned row_vertex = blockIdx.x * blockDim.x + threadIdx.x;
  if(row_vertex >= V) return;
  if (d_compress[row_vertex] == true){ // if filter begin 
    if (row_vertex >= nc){ // if filter begin 
      int matched_col = d_predeccesor[row_vertex]; // DEVICE ASSTMENT in .h

      int matched_row = d_cmatch[matched_col]; // DEVICE ASSTMENT in .h

      bool isValid = true; // DEVICE ASSTMENT in .h

      if (matched_row != -1){ // if filter begin 
        if (d_predeccesor[matched_row] == matched_col){ // if filter begin 
          isValid = false;

        } // if filter end

      } // if filter end
      if (isValid){ // if filter begin 
        d_cmatch[matched_col] = row_vertex;
        d_rmatch[row_vertex] = matched_col;
        if (matched_row != -1){ // if filter begin 
          d_compress_next[matched_row] = true;
          compressed = false;

        } // if filter end

      } // if filter end

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void APFB_kernel_5(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_compress_next,bool* d_compress){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned row_vertex = blockIdx.x * blockDim.x + threadIdx.x;
  if(row_vertex >= V) return;
  if (row_vertex >= nc){ // if filter begin 
    d_compress[row_vertex] = d_compress_next[row_vertex];
    d_compress_next[row_vertex] = false;

  } // if filter end
} // end KER FUNC
__global__ void APFB_kernel_6(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_rmatch,int* d_cmatch){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned r = blockIdx.x * blockDim.x + threadIdx.x;
  if(r >= V) return;
  if (r >= nc){ // if filter begin 
    int c = d_rmatch[r]; // DEVICE ASSTMENT in .h

    if (c > -1){ // if filter begin 
      if (d_cmatch[c] != r){ // if filter begin 
        d_rmatch[r] = -1;

      } // if filter end

    } // if filter end
    if (c == -2){ // if filter begin 
      d_rmatch[r] = -1;

    } // if filter end

  } // if filter end
} // end KER FUNC

#endif
