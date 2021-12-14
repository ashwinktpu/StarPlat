#ifndef GENCPP_TRIANGLE_COUNTING_DSL_H
#define GENCPP_TRIANGLE_COUNTING_DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<cuda.h>
#include"../graph.hpp"
#include"../libcuda.cuh"

Compute_TC(graph& g);

unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
unsigned int v =id;
if (id < V)
{for (int edge = gpu_OA[id]; edge < gpu_OA[id+1]; edge ++) 
  {int u = g.edgeList[edge] ;
    if (u < v )for (int edge = gpu_OA[id]; edge < gpu_OA[id+1]; edge ++) 
    {int w = g.edgeList[edge] ;
      if (w > v )if (g.check_if_nbr(u, w) )triangle_count = triangle_count+ 1;
    }
  }

  #endif
