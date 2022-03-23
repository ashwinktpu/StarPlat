#ifndef GENCPP_SSSP_V2_H
#define GENCPP_SSSP_V2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_SSSP(graph& g,int* dist,int src);

__global__ void fwd_pass(int V, int E, int* d_meta,int* d_data,int* d_weight
   ,graph& g,int* dist,int src){unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge ++)
  {int g = gpu_d_data[edge];
    int e = edge;
     int dist_new = d_dist[v] + d_weight[e];
    bool modified_new = true;
    if(dist[id]!= MAX_VAL && d_dist[nbr] > dist_new)
    {
      atomicMin(&d_dist[nbr],dist_new);
      modified_nxt[nbr] = modified_new;
      finished[0] = false ;
    }
  }
}
} // end if d lvl

#endif
