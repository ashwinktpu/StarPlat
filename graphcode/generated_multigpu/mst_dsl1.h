// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_DSL1_H
#define GENCPP_MST_DSL1_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <curand.h>
#include <cooperative_groups.h>

void Boruvka(graph& g);



__global__ void Boruvka_kernel1(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_color,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( u < num_vertices) {
    u+=start;
    //hi2
    d_nodeId[u] = u; //InitIndex
    //hi2
    d_color[u] = u; //InitIndex
  }
} // end KER FUNC
__global__ void Boruvka_kernel2(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_minEdge,int* d_color,bool* d_noNewComp){ // BEGIN KER FUN via ADDKERNEL
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { 
      int dst = d_data[edge];
      if (d_color[src] != d_color[dst]){ // if filter begin 
        int e = edge;
        int minEdge = d_minEdge[src];
        if (minEdge == -1){ // if filter begin 
          d_minEdge[src] = e;
        } // if filter end
        if (minEdge != -1){ // if filter begin 
          int minDst = d_data[minEdge];
          if (d_weight[e] < d_weight[minEdge] || (d_weight[e] == d_weight[minEdge] && d_color[dst] < d_color[minDst])){ // if filter begin 
            d_minEdge[src] = e;
          } // if filter end
        } // if filter end
      } // if filter end
    } //  end FOR NBR ITR. TMP FIX!
  }
} // end KER FUNC
__global__ void Boruvka_kernel3(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_minEdge,int* d_minEdgeOfComp,int* d_color,bool* d_finishedMinEdge){ // BEGIN KER FUN via ADDKERNEL
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( u < num_vertices) {
    u+=start;
    int comp = d_color[u]; // asst in .cu 
    int minEdge = d_minEdgeOfComp[comp]; // asst in .cu 
    int e = d_minEdge[u]; // asst in .cu 
    if (e != -1){ // if filter begin 
      int dst = d_data[e]; // asst in .cu 
      if (minEdge == -1){ // if filter begin 
        d_minEdgeOfComp[comp] = e;
        d_finishedMinEdge[0] = false;
      } // if filter end
      if (minEdge != -1){ // if filter begin 
        int minDst = d_data[minEdge];
        if (d_weight[e] < d_weight[minEdge] || (d_weight[e] == d_weight[minEdge] && d_color[dst] < d_color[minDst])){ // if filter begin 
          d_minEdgeOfComp[comp] = e;
          d_finishedMinEdge[0] = false;
        } // if filter end
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void Boruvka_kernel4(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_minEdgeOfComp,int* d_color,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_color[src] == d_nodeId[src]){ // if filter begin 
      int srcMinEdge = d_minEdgeOfComp[src];
      if (srcMinEdge != -1){ // if filter begin 
        int dst = d_data[srcMinEdge];
        int dstLead = d_color[dst];
        int dstMinEdge = d_minEdgeOfComp[dstLead];
        if (dstMinEdge != -1){ // if filter begin 
          int dstOfDst = d_data[dstMinEdge];
          int dstOfDstLead = d_color[dstOfDst];
          if (d_color[src] == d_color[dstOfDstLead] && d_color[src] > d_color[dstLead]){ // if filter begin 
            d_minEdgeOfComp[dstLead] = -1;
          } // if filter end
        } // if filter end
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void Boruvka_kernel5(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_minEdgeOfComp,int* d_color,int* d_nodeId,bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_color[src] == d_nodeId[src]){ // if filter begin 
      int srcMinEdge = d_minEdgeOfComp[src];
      if (srcMinEdge != -1){ // if filter begin 
        d_isMSTEdge[srcMinEdge] = true;
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void Boruvka_kernel6(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_minEdgeOfComp,int* d_color,int* d_nodeId,bool* d_noNewComp){ // BEGIN KER FUN via ADDKERNEL
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( src < num_vertices) {
    src+=start;
    if (d_color[src] == d_nodeId[src]){ // if filter begin 
      int srcMinEdge = d_minEdgeOfComp[src];
      if (srcMinEdge != -1){ // if filter begin 
        d_noNewComp[0] = false;
        int dst = d_data[srcMinEdge];
        d_color[src] = d_color[dst];
      } // if filter end
    } // if filter end
  }
} // end KER FUNC
__global__ void Boruvka_kernel7(int start,int end,int V, int E, int* d_meta, int* d_data, int* d_weight,int* d_src,int* d_rev_meta,int* d_color,bool* d_finished){ // BEGIN KER FUN via ADDKERNEL
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  int num_vertices = end-start;
  if( u < num_vertices) {
    u+=start;
    int my_color = d_color[u]; // asst in .cu 
    int other_color = d_color[my_color]; // asst in .cu 
    if (my_color != other_color){ // if filter begin 
      d_finished[0] = false;
      d_color[u] = other_color;
    } // if filter end
  }
} // end KER FUNC

#endif
