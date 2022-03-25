#ifndef GENCPP_PAGERANK_DSL_V2_H
#define GENCPP_PAGERANK_DSL_V2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"
#include "libcuda.cuh"
#include <cooperative_groups.h>

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank);

__device__ d_num_nodes;
float* d_num_nodes 
=g.num_nodes( ); // DEVICE ASSTMENT
__device__ d_iterCount;
int* d_iterCount 
=0; // DEVICE ASSTMENT
__device__ d_diff;
float* d_diff 
__global__ void Compute_PR_kernel(int V, int E, int* d_meta, int* d_data, int* d_weight ,graph& g,float beta,float delta,int maxIter,
  float* d_pageRank){
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  __device__ d_sum;
  float* d_sum 
  =0.000000; // DEVICE ASSTMENT
  for (int edge = gpu_rev_OA[v]; edge < gpu_rev_OA[v+1]; edge ++)
  {int nbr = srcList[edge] ;
    sum = sum + d_pageRank[nbr] / (gpu_OA[nbr+1]-gpu_OA[nbr]);
    __device__ d_val;
    float* d_val 
    =(1 - delta) / num_nodes + delta * sum; // DEVICE ASSTMENT
    diff = diff+ val - d_pageRank[v];
    d_pageRank_nxt[v] = val;
  } // end if d lvl

  #endif
