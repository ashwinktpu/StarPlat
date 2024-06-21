// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_TEST_HEAP_H
#define GENCPP_TEST_HEAP_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Test(graph& g,int src);



__device__ int maxSize ; // DEVICE ASSTMENT in .h

__device__ int siz ; // DEVICE ASSTMENT in .h


#endif
