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



__global__ void Boruvka_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_nodeId){ // BEGIN KER FUN via ADDKERNEL
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
__device__ bool no_new_comp ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_2(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,int* d_minEdge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    int minEdge = d_minEdge[src];
    int minDst = d_data[minEdge];
    if (d_color[dst] != d_color[src] && (minEdge == -1 || d_weight[e] < d_weight[minEdge] || (d_weight[e] == d_weight[minEdge] && d_color[dst] < d_color[minDst]))){ // if filter begin 
      d_minEdge[src] = e;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void Boruvka_kernel_3(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeComp,int* d_nodeId,int* d_minEdge,int* d_color){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == d_nodeId[src]){ // if filter begin 
    for (int nbr = 0; nbr < V; nbr++) {
      if (d_color[nbr] == d_color[src]){ // if filter begin 
        int minEdgeNbr = d_minEdge[nbr];
        int minEdgeComp = d_minEdgeComp[src];
        int minDst = d_data[minEdgeComp];
        int dst = d_data[minEdgeNbr];
        if (minEdgeNbr != -1){ // if filter begin 
          if (minEdgeComp == -1 || d_weight[minEdgeNbr] < d_weight[minEdgeComp] || (d_weight[minEdgeNbr] == d_weight[minEdgeComp] && d_color[dst] < d_color[minDst])){ // if filter begin 
            d_minEdgeComp[src] = minEdgeNbr;

          } // if filter end

        } // if filter end

      } // if filter end

    }
    int minEdgeComp = d_minEdgeComp[src];
    if (minEdgeComp != -1){ // if filter begin 
      d_isMSTEdge[minEdgeComp] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void Boruvka_kernel_4(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeComp,int* d_nodeId,int* d_color){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src1 = blockIdx.x * blockDim.x + threadIdx.x;
  if(src1 >= V) return;
  if (d_color[src1] == d_nodeId[src1]){ // if filter begin 
    int edge1 = d_minEdgeComp[src1];
    if (edge1 != -1){ // if filter begin 
      int dst1 = d_data[edge1];
      dst1 = d_color[dst1];
      int edge2 = d_minEdgeComp[dst1];
      if (edge2 != -1){ // if filter begin 
        int dst2 = d_data[edge2];
        dst2 = d_color[dst2];
        if (d_color[src1] == d_color[dst2] && d_color[src1] > d_color[dst1]){ // if filter begin 
          d_minEdgeComp[dst1] = -1;

        } // if filter end

      } // if filter end

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_5(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_minEdgeComp,int* d_nodeId,int* d_color,bool* d_newColorChanges,bool* d_modified){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  if (d_color[u] == d_nodeId[u]){ // if filter begin 
    int minEdgeComp = d_minEdgeComp[u];
    if (minEdgeComp != -1){ // if filter begin 
      int minDst = d_data[minEdgeComp];
      d_color[u] = d_color[minDst];
      d_modified[u] = true;
      d_newColorChanges[u] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void Boruvka_kernel_6(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_color,bool* d_modified){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int lead = d_color[u]; // DEVICE ASSTMENT in .h

  if (d_color[lead] != d_color[u]){ // if filter begin 
    d_color[u] = d_color[lead];
    d_modified[u] = true;

  } // if filter end
} // end KER FUNC

#endif
