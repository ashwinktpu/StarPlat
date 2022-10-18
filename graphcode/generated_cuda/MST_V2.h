// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_V2_H
#define GENCPP_MST_V2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Boruvka(graph& g);



__global__ void Boruvka_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  initIndex<int><<<1,1>>>(V,d_nodeId,u,(int)u); //InitIndexDevice
  initIndex<int><<<1,1>>>(V,d_color,u,(int)u); //InitIndexDevice
} // end KER FUNC
__device__ bool noNewComp ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_2(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdge,int* d_color){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
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
} // end KER FUNC
__device__ bool finishedMinEdge ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_3(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdge,int* d_minEdgeOfComp,int* d_color){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int comp = d_color[u]; // DEVICE ASSTMENT in .h

  int minEdge = d_minEdgeOfComp[comp]; // DEVICE ASSTMENT in .h

  int e = d_minEdge[u]; // DEVICE ASSTMENT in .h

  if (e != -1){ // if filter begin 
    int dst = d_data[e]; // DEVICE ASSTMENT in .h

    if (minEdge == -1){ // if filter begin 
      d_minEdgeOfComp[comp] = e;
      finishedMinEdge = false;

    } // if filter end
    if (minEdge != -1){ // if filter begin 
      int minDst = d_data[minEdge];
      if (d_weight[e] < d_weight[minEdge] || (d_weight[e] == d_weight[minEdge] && d_color[dst] < d_color[minDst])){ // if filter begin 
        d_minEdgeOfComp[comp] = e;
        finishedMinEdge = false;

      } // if filter end

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void Boruvka_kernel_4(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_minEdgeOfComp,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
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
} // end KER FUNC
__global__ void Boruvka_kernel_5(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_minEdgeOfComp,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == d_nodeId[src]){ // if filter begin 
    int srcMinEdge = d_minEdgeOfComp[src];
    if (srcMinEdge != -1){ // if filter begin 
      d_isMSTEdge[srcMinEdge] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void Boruvka_kernel_6(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_minEdgeOfComp,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == d_nodeId[src]){ // if filter begin 
    int srcMinEdge = d_minEdgeOfComp[src];
    if (srcMinEdge != -1){ // if filter begin 
      noNewComp = false;
      int dst = d_data[srcMinEdge];
      d_color[src] = d_color[dst];

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_7(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int my_color = d_color[u]; // DEVICE ASSTMENT in .h

  int other_color = d_color[my_color]; // DEVICE ASSTMENT in .h

  if (my_color != other_color){ // if filter begin 
    finished = false;
    d_color[u] = other_color;

  } // if filter end
} // end KER FUNC

#endif
