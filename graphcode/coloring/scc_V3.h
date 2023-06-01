// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SCC_V3_H
#define GENCPP_SCC_V3_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"
#include "libcuda.cuh"
#include <cooperative_groups.h>

void vHong(graph& g);



__global__ void vHong_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_outDeg,int* d_inDeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    int nxtSrc = src + 1; // asst in .cu 
    d_inDeg[src] = d_inDeg[src]+ d_rev_meta[nxtSrc] - d_rev_meta[src];
    d_outDeg[src] = d_outDeg[src]+ d_meta[nxtSrc] - d_meta[src];
  }
} // end KER FUNC
__global__ void vHong_kernel2(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_range,int* d_scc,bool* d_fpoint1,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int havePar = 0; // asst in .cu 
      int haveChild = 0; // asst in .cu 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
        int par = d_src[edge] ;
        if (d_scc[par] == -1){ // if filter begin 
          if (d_range[par] == d_range[src]){ // if filter begin 
            havePar = 1;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_range[dst] == d_range[src]){ // if filter begin 
            haveChild = 1;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
        d_scc[src] = src;
        d_isPivot[src] = true;
        d_fpoint1[0] = false;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel3(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,int* d_outDeg,int* d_pivotField,int* d_inDeg,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int color = d_range[src]; // asst in .cu 
      int index = color - (color / V) * V; // asst in .cu 
      int oldSrcValue = -1; // asst in .cu 
      int oldSrc = d_pivotField[index]; // asst in .cu 
      if (oldSrc >= 0){ // if filter begin 
        oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];
      } // if filter end
      if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){ // if filter begin 
        d_pivotField[index] = src;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel4(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,int* d_pivotField,int* d_range,bool* d_isPivot,bool* d_visitBw,bool* d_visitFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int color = d_range[src]; // asst in .cu 
      int index = color - (color / V) * V; // asst in .cu 
      if (d_pivotField[index] == src){ // if filter begin 
        d_visitFw[src] = true;
        d_visitBw[src] = true;
        d_isPivot[src] = true;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel5(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,bool* d_propBw,bool* d_visitBw,bool* d_propFw,bool* d_visitFw,int* d_scc,int* d_range,bool* d_fpoint2){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int myrange = d_range[src]; // asst in .cu 
      if (d_propFw[src] == false && d_visitFw[src] == true){ // if filter begin 
        for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
          int dst = d_data[edge];
          if (d_scc[dst] == -1){ // if filter begin 
            if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
              d_visitFw[dst] = true;
              d_fpoint2[0] = false;
            } // if filter end
          } // if filter end
        } //  end FOR NBR ITR. TMP FIX!
        d_propFw[src] = true;
      } // if filter end
      if (d_propBw[src] == false && d_visitBw[src] == true){ // if filter begin 
        for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
          int par = d_src[edge] ;
          if (d_scc[par] == -1){ // if filter begin 
            if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
              d_visitBw[par] = true;
              d_fpoint2[0] = false;
            } // if filter end
          } // if filter end
        } //  end FOR NBR ITR. TMP FIX!
        d_propBw[src] = true;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel6(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,bool* d_visitFw,bool* d_visitBw,int* d_range,bool* d_propBw,bool* d_propFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      if (d_visitFw[src] == false || d_visitBw[src] == false){ // if filter begin 
        int ext1 = 0; // asst in .cu 
        int ext2 = 0; // asst in .cu 
        if (d_visitFw[src] == false){ // if filter begin 
          ext1 = 1;
        } // if filter end
        if (d_visitBw[src] == false){ // if filter begin 
          ext2 = 1;
        } // if filter end
        d_range[src] = 3 * d_range[src] + ext1 + ext2;
        d_visitFw[src] = false;
        d_visitBw[src] = false;
        d_propFw[src] = false;
        d_propBw[src] = false;
      } // if filter end
      if (d_visitFw[src] == true && d_visitBw[src] == true){ // if filter begin 
        d_scc[src] = src;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel7(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_range,int* d_scc,bool* d_fpoint1,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int havePar = 0; // asst in .cu 
      int haveChild = 0; // asst in .cu 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
        int par = d_src[edge] ;
        if (d_scc[par] == -1){ // if filter begin 
          if (d_range[par] == d_range[src]){ // if filter begin 
            havePar = 1;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_range[dst] == d_range[src]){ // if filter begin 
            haveChild = 1;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
      if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
        d_scc[src] = src;
        d_isPivot[src] = true;
        d_fpoint1[0] = false;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel8(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    d_range[src] = d_range[src]+ src;
  }
} // end KER FUNC
__global__ void vHong_kernel9(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_range,int* d_scc,bool* d_fpoint4){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_range[dst] < d_range[src]){ // if filter begin 
            d_range[src] = d_range[dst];
            d_fpoint4[0] = false;
          } // if filter end
        } // if filter end
      } //  end FOR NBR ITR. TMP FIX!
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel10(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,int* d_range,bool* d_fpoint4){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int myrange = d_range[src]; // asst in .cu 
      if ((myrange != src) && (myrange != d_range[myrange])){ // if filter begin 
        d_range[src] = d_range[myrange];
        d_fpoint4[0] = false;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel11(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,int* d_outDeg,int* d_pivotField,int* d_inDeg,int* d_range,bool* d_fpoint5){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int color = d_range[src]; // asst in .cu 
      int index = color - (color / V) * V; // asst in .cu 
      int oldSrcValue = -1; // asst in .cu 
      int oldSrc = d_pivotField[index]; // asst in .cu 
      if (oldSrc >= 0){ // if filter begin 
        oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];
      } // if filter end
      if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){ // if filter begin 
        d_pivotField[index] = src;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel12(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,int* d_pivotField,int* d_range,bool* d_isPivot,bool* d_visitBw,bool* d_visitFw,bool* d_fpoint5){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int color = d_range[src]; // asst in .cu 
      int index = color - (color / V) * V; // asst in .cu 
      if (d_pivotField[index] == src){ // if filter begin 
        d_visitFw[src] = true;
        d_visitBw[src] = true;
        d_isPivot[src] = true;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel13(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,bool* d_propBw,bool* d_visitBw,bool* d_propFw,bool* d_visitFw,int* d_scc,int* d_range,bool* d_fpoint2){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      int myrange = d_range[src]; // asst in .cu 
      if (d_propFw[src] == false && d_visitFw[src] == true){ // if filter begin 
        for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
          int dst = d_data[edge];
          if (d_scc[dst] == -1){ // if filter begin 
            if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
              d_visitFw[dst] = true;
              d_fpoint2[0] = false;
            } // if filter end
          } // if filter end
        } //  end FOR NBR ITR. TMP FIX!
        d_propFw[src] = true;
      } // if filter end
      if (d_propBw[src] == false && d_visitBw[src] == true){ // if filter begin 
        for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++){
          int par = d_src[edge] ;
          if (d_scc[par] == -1){ // if filter begin 
            if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
              d_visitBw[par] = true;
              d_fpoint2[0] = false;
            } // if filter end
          } // if filter end
        } //  end FOR NBR ITR. TMP FIX!
        d_propBw[src] = true;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void vHong_kernel14(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_scc,bool* d_visitFw,bool* d_visitBw,int* d_range,bool* d_propFw,bool* d_propBw,bool* d_fpoint5){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_scc[src] == -1){ // if filter begin 
      if (d_visitFw[src] == false || d_visitBw[src] == false){ // if filter begin 
        d_fpoint5[0] = false;
        int ext1 = 0; // asst in .cu 
        int ext2 = 0; // asst in .cu 
        if (d_visitFw[src] == false){ // if filter begin 
          ext1 = 1;
        } // if filter end
        if (d_visitBw[src] == false){ // if filter begin 
          ext2 = 1;
        } // if filter end
        int newRange = 3 * d_range[src]; // asst in .cu 
        newRange = newRange - (newRange / V) * V;
        d_range[src] = newRange + ext1 + ext2;
        d_visitFw[src] = false;
        d_visitBw[src] = false;
        d_propFw[src] = false;
        d_propBw[src] = false;
      } // if filter end
      if (d_visitFw[src] == true && d_visitBw[src] == true){ // if filter begin 
        d_scc[src] = src;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC

#endif
