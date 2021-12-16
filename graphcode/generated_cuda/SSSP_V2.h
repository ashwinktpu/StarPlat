#ifndef GENCPP_SSSP_V2_H
#define GENCPP_SSSP_V2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<cuda.h>
#include"../graph.hpp"
#include"../libcuda.cuh"

__global__ void void Compute_SSSP_kernel(graph& g,int* dist,int src);

graph& g,int* dist,int srcunsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
unsigned int v =id;
if (id < V)
{if (modified[v] == true )for (int edge = gpu_OA[id]; edge < gpu_OA[id+1]; edge ++) 
  {int nbr = g.edgeList[edge] ;
    int e = edge;
     int dist_new = dist[v] + weight[e];
    bool modified_new = true;
    if(dist[id]!= MAX_VAL && dist[nbr] > dist_new)
    {
      atomicMin(&dist[nbr],dist_new);
      modified_nxt[nbr] = modified_new;
      finished[0] = false ;
    }
  }

  #endif
