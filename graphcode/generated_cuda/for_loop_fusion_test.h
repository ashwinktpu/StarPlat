// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_FOR_LOOP_FUSION_TEST_H
#define GENCPP_FOR_LOOP_FUSION_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>
void for_loop_fusion(graph& g,std::set<int>& sourceSet);



__global__ void merged_kernel_1(unsigned V, int* array_1, int val_1, int* array_2, int val_2){
    unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
    if (id < V) {
        array_1[id] = val_1;
        array_2[id] = val_2;
    }
}
__device__ int count1 ; // DEVICE ASSTMENT in .h

__device__ int count2 ; // DEVICE ASSTMENT in .h

__global__ void for_loop_fusion_kernel(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  count1count2} // end KER FUNC

#endif
