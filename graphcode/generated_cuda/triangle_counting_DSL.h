// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
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



// __device__ long triangle_count ; // DEVICE ASSTMENT in .h

// __global__ void Compute_TC_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next){ // BEGIN KER FUN via ADDKERNEL
//   float num_nodes  = V;
//   unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
//   if(v >= V) return;
//   for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
//     int u = d_data[edge];
//     if (u < v){ // if filter begin 
//       for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
//         int w = d_data[edge];
//         if (w > v){ // if filter begin 
//           if (findNeighborSorted(u, w, d_meta, d_data)){ // if filter begin 
//             atomicAdd((unsigned long long*)& triangle_count, (unsigned long long)1);

//           } // if filter end

//         } // if filter end

//       } //  end FOR NBR ITR. TMP FIX!

//     } // if filter end

//   } //  end FOR NBR ITR. TMP FIX!
// } // end KER FUNC

__device__ long triangle_count; // DEVICE ASSTMENT in .h

__global__ void Compute_TC_kernel(int V, int E, int *d_meta, int *d_data, int *d_src, int *d_weight, int *d_rev_meta, bool *d_modified_next)
{
  float num_nodes = V;
  const int tile_size = 256;
  __shared__ int tile_data[tile_size];
  __shared__ int tile_meta[tile_size + 1];

  for (int tile_base = 0; tile_base < V; tile_base += tile_size)
  {
    int tile_end = min(tile_base + tile_size, V);
    int tile_len = tile_end - tile_base;

    // Load tile data and meta
    if (threadIdx.x < tile_len)
    {
      int node = tile_base + threadIdx.x;
      tile_meta[threadIdx.x] = d_meta[node];
      tile_meta[threadIdx.x + 1] = d_meta[node + 1];
      for (int i = tile_meta[threadIdx.x]; i < tile_meta[threadIdx.x + 1]; ++i)
      {
        tile_data[i] = d_data[i];
      }
    }
    __syncthreads();

    // Compute triangle counts for each vertex in the tile
    for (int i = threadIdx.x; i < tile_len; ++i)
    {
      int v = tile_base + i;
      for (int edge = tile_meta[i]; edge < tile_meta[i + 1]; edge++)
      {
        int u = tile_data[edge];
        if (u < v)
        { // if filter begin
          for (int j = 0; j < tile_len; ++j)
          {
            int w = tile_base + j;
            if (w > v)
            { // if filter begin
              for (int edge = tile_meta[j]; edge < tile_meta[j + 1]; edge++)
              {
                int n = tile_data[edge];
                if (n == u)
                {
                  atomicAdd((unsigned long long *)&triangle_count, (unsigned long long)1);
                }
              }
            } // if filter end
          }
        } // if filter end
      }   //  end FOR NBR ITR
    }
    __syncthreads();
  }
} // end KER FUNC

#endif


