#ifndef GENCPP_PAGERANK_DSL_V2_H
#define GENCPP_PAGERANK_DSL_V2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<cuda.h>
#include"../graph.hpp"
#include"../libcuda.cuh"

Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank);

unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
unsigned int v =id;
if (id < V)
{float sum = 0.000000;
  for (int edge = gpu_rev_OA[v]; edge < gpu_rev_OA[v+1]; edge ++) 
  {int nbr = srcList[edge] ;
    sum = sum + pageRank[nbr] / (gpu_OA[nbr+1]-gpu_OA[nbr]);
  }
  float val = (1 - delta) / num_nodes + delta * sum;
  diff = diff+ val - pageRank[v];
  pageRank_nxt[v] = val;

  #endif
