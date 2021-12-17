#ifndef GENCPP_BC_DSL_V2_H
#define GENCPP_BC_DSL_V2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<cuda.h>
#include"../graph.hpp"
#include"../libcuda.cuh"

__global__ void void Compute_BC_kernel(graph& g,float* BC,std::set<int>& sourceSet)
{

  graph& g,float* BC,std::set<int>& sourceSetstd::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++)
  for (int edge = gpu_OA[id]; edge < gpu_OA[id+1]; edge ++) 
  {int w = g.edgeList[edge] ;
    sigma[w] = sigma[w] + sigma[v];
    for (int edge = gpu_OA[id]; edge < gpu_OA[id+1]; edge ++) 
    {int w = g.edgeList[edge] ;
      delta[v] = delta[v] + (sigma[v] / sigma[w]) * (1 + delta[w]);

      #endif
