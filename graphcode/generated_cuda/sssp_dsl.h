#ifndef GENCPP_SSSP_DSL_H
#define GENCPP_SSSP_DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<cuda.h>
#include"../graph.hpp"
#include"../libcuda.cuh"

Compute_SSSP(graph& g,int src);

void Compute_SSSP (int * OA, int *edgeList)
{
  unsigned V = g.num_nodes();
  unsigned E = g.num_edges();

  int* gpu_OA;
  int* gpu_edgeList;
  int* gpu_edgeList;

  cudaMalloc(&d_gpu_OA, sizeof(int)*(1+V));
  cudaMalloc(&d_gpu_edgeList, sizeof(int)*(E));
  cudaMalloc(&d_gpu_edgeList, sizeof(int)*(E));

  if( V <= 1024)
  {
    block_size = V;
    num_blocks = 1;
  }
  else
  {
    block_size = 1024;
    num_blocks = ceil(((float)V) / block_size);
  }
  cudaMemcpy(&d_gpu_OA,OA, sizeof(int)*(1+V), cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  Compute_SSSP_kernel<<<num_blocks, block_size>>>(gpu_OA, gpu_edgeList, V, E ;
    cudaDeviceSynchronize();
    int countprintf("TC = %d",count);
  }


  #endif
