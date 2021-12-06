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

  if()
  {


    #endif
