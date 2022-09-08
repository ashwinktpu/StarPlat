// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_V1_H
#define GENCPP_MST_V1_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Boruvka(graph& g);



__global__ void Boruvka_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_nodeId, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned dummy = blockIdx.x * blockDim.x + threadIdx.x;
  if(dummy >= V) return;
  int i = 0; // DEVICE ASSTMENT in .h

  for (int v = 0; v < V; v++) {
    d_nodeId[v] = i;
    d_color[v] = i;
    i = i + 1;
  }
} // end KER FUNC
__device__ bool noNewComp ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_2(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdge,int* d_color, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
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
  if(d_minEdge[src] != -1){
      int e = d_minEdge[src];
      printf("Min Edge for %d: %d %d %d\n", src, d_src[e], d_data[e], d_weight[e]);
  }
    
} // end KER FUNC
__global__ void Boruvka_kernel_3(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeOfComp,int* d_nodeId,int* d_minEdge,int* d_color, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == d_nodeId[src]){ // if filter begin 
    for (int nbr = 0; nbr < V; nbr++) {
      if (d_color[nbr] == d_color[src]){ // if filter begin 
        int minEdgeNbr = d_minEdge[nbr];
        int minEdgeOfComp = d_minEdgeOfComp[src];
        if (minEdgeNbr != -1 && minEdgeOfComp == -1){ // if filter begin 
          d_minEdgeOfComp[src] = minEdgeNbr;

        } // if filter end
        if (minEdgeNbr != -1 && minEdgeOfComp != -1){ // if filter begin 
          int minDst = d_data[minEdgeOfComp];
          int dst = d_data[minEdgeNbr];
          if (d_weight[minEdgeNbr] < d_weight[minEdgeOfComp] || (d_weight[minEdgeNbr] == d_weight[minEdgeOfComp] && d_color[dst] < d_color[minDst])){ // if filter begin 
            d_minEdgeOfComp[src] = minEdgeNbr;

          } // if filter end

        } // if filter end

      } // if filter end

    }
  } // if filter end
} // end KER FUNC
__global__ void Boruvka_kernel_4(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeOfComp,int* d_nodeId,int* d_color, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
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


__global__ void Boruvka_kernel_20(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeOfComp,int* d_nodeId,int* d_color, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == d_nodeId[src]){ // if filter begin 
    int minEdgeOfComp = d_minEdgeOfComp[src];
    if (minEdgeOfComp != -1){ // if filter begin 
        d_isMSTEdge[minEdgeOfComp] = true;
    } // if filter end
  }
}

__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_5(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeOfComp,int* d_nodeId,int* d_color,bool* d_newColorChanges,bool* d_modified, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  if (d_color[u] == d_nodeId[u]){ // if filter begin 
    int minEdgeOfComp = d_minEdgeOfComp[u];
    if (minEdgeOfComp != -1){ // if filter begin 
      int minDst = d_data[minEdgeOfComp];
      d_color[u] = d_color[minDst];
      d_modified[u] = true;
      d_newColorChanges[u] = true;
        finished = false;
        noNewComp = false;
    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void Boruvka_kernel_6(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color, bool* d_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int lead = d_color[u]; // DEVICE ASSTMENT in .h

  if (d_color[lead] != d_color[u]){ // if filter begin 
    d_color[u] = d_color[lead];

  } // if filter end
} // end KER FUNC

#endif