// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_CC_H
#define GENCPP_CC_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_CC(graph& g,float* CC);



__device__ int V ; // DEVICE ASSTMENT in .h

__device__ bool finished ; // DEVICE ASSTMENT in .h


#endif
