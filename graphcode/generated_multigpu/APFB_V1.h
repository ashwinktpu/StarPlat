// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_APFB_V1_H
#define GENCPP_APFB_V1_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void APFB(graph& g,int nc);




__global__ void APFB_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_nc,int* d_cmatch,int* d_L0,int* d_bfsArray,bool* d_noNewPaths){ // BEGIN KER FUN via ADDKERNEL
  unsigned c = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( c < num_vertices) {
    c+=start;
    if (c < d_nc[0]){ // if filter begin 
      if (d_cmatch[c] == -1){ // if filter begin 
        d_bfsArray[c] = d_L0[0];
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void APFB_kernel2(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_bfsArray,int* d_nc,int* d_bfsLevel,int* d_rmatch,int* d_NOT_VISITED,bool* d_noNewVertices,int* d_predeccesor,bool* d_noNewPaths){ // BEGIN KER FUN via ADDKERNEL
  unsigned col_vertex = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( col_vertex < num_vertices) {
    col_vertex+=start;
    if (col_vertex < d_nc[0]){ // if filter begin 
      if (d_bfsArray[col_vertex] == d_bfsLevel[0]){ // if filter begin 
        for (int edge = d_meta[col_vertex]; edge < d_meta[col_vertex+1]; edge++) { 
          int neigh_row = d_data[edge];
          if (neigh_row >= d_nc[0]){ // if filter begin 
            int col_match = d_rmatch[neigh_row]; // asst in .cu 
            if (col_match > -1){ // if filter begin 
              if (d_bfsArray[col_match] == d_NOT_VISITED[0]){ // if filter begin 
                d_noNewVertices[0] = false;
                d_bfsArray[col_match] = d_bfsLevel[0] + 1;
                d_predeccesor[neigh_row] = col_vertex;
              } // if filter end
            } // if filter end
            if (col_match == -1){ // if filter begin 
              d_rmatch[neigh_row] = -2;
              d_predeccesor[neigh_row] = col_vertex;
              d_noNewPaths[0] = false;
            } // if filter end
          } // if filter end
        } //  end FOR NBR ITR. TMP FIX!
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void APFB_kernel3(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_nc,int* d_rmatch,bool* d_compress){ // BEGIN KER FUN via ADDKERNEL
  unsigned r = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( r < num_vertices) {
    r+=start;
    if (r >= d_nc[0] && d_rmatch[r] == -2){ // if filter begin 
      d_compress[r] = true;
    } // if filter end
  }
} // end KER FUNC
__global__ void APFB_kernel4(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,bool* d_compress,int* d_nc,int* d_cmatch,int* d_predeccesor,bool* d_compressed,bool* d_compress_next,int* d_rmatch){ // BEGIN KER FUN via ADDKERNEL
  unsigned row_vertex = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( row_vertex < num_vertices) {
    row_vertex+=start;
    if (d_compress[row_vertex] == true){ // if filter begin 
      if (row_vertex >= d_nc[0]){ // if filter begin 
        int matched_col = d_predeccesor[row_vertex]; // asst in .cu 
        int matched_row = d_cmatch[matched_col]; // asst in .cu 
        bool isValid = true; // asst in .cu 
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
            d_compressed[0] = false;
          } // if filter end
        } // if filter end
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void APFB_kernel5(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_nc,bool* d_compress_next,bool* d_compress,bool* d_compressed){ // BEGIN KER FUN via ADDKERNEL
  unsigned row_vertex = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( row_vertex < num_vertices) {
    row_vertex+=start;
    if (row_vertex >= d_nc[0]){ // if filter begin 
      d_compress[row_vertex] = d_compress_next[row_vertex];
      d_compress_next[row_vertex] = false;
    } // if filter end
  }
} // end KER FUNC
__global__ void APFB_kernel6(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_nc,int* d_cmatch,int* d_rmatch){ // BEGIN KER FUN via ADDKERNEL
  unsigned r = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( r < num_vertices) {
    r+=start;
    if (r >= d_nc[0]){ // if filter begin 
      int c = d_rmatch[r]; // asst in .cu 
      if (c >= 0){ // if filter begin 
        if (d_cmatch[c] != r){ // if filter begin 
          d_rmatch[r] = -1;
        } // if filter end
      } // if filter end
      if (c == -2){ // if filter begin 
        d_rmatch[r] = -1;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC

#endif
