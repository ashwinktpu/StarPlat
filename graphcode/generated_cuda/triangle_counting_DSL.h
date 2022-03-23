#ifndef GENCPP_TRIANGLE_COUNTING_DSL_H
#define GENCPP_TRIANGLE_COUNTING_DSL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"
#include "libcuda.cuh"
#include <cooperative_groups.h>

void Compute_TC(graph& g);

__global__ void Compute_TC_kernel(int V, int E, int* d_meta,int* d_data,int* d_weight ,graph& g){unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge ++)
  {int u = d_data[edge];
    if (u < v ) {for (int edge = d_meta[v]; edge < d_meta[v+1]; edge ++)
      {int w = d_data[edge];
        if (w > v ) {if (g.check_if_nbr(u, w) ) {triangle_count = triangle_count+ 1;
           } } }} // end if d lvl

        #endif
