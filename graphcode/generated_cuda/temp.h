// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_TEMP_H
#define GENCPP_TEMP_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_SSSP(graph& g,int* dist,int src);

__device__ int src ;


__device__ int x ; // DEVICE ASSTMENT in .h


#endif
