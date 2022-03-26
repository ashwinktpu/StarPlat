#ifndef GENCPP_TRIANGLE_COUNTING_DSL_H
#define GENCPP_TRIANGLE_COUNTING_DSL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_TC(graph& g);

__device__ long triangle_count = 0; // DEVICE ASSTMENT in .h

__global__ void Compute_TC_kernel(int V, int E, int* d_meta, int* d_data, int* d_weight ,graph& g){ // BEGIN FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int u = d_data[edge];
    if (u < v){ // if filter begin 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int w = d_data[edge];
        if (w > v){ // if filter begin 
          if (findNeighborSorted(u, w, d_meta, d_data)){ // if filter begin 
            atomicAdd(& triangle_count, (long)1);
          } // if filter end
        } // if filter end
      } // FOR END
    } // if filter end
  } // FOR END
} // end FUNC

#endif
