#include "libOpenCL.h"
__kernel void initdist_kernel( __global int *d_dist , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_dist[id] = value;
  }
}
__kernel void Compute_SSSP_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int * d_dist){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;

  __kernel void initIndexdist_kernel(__global int *d_dist, int v , int val){
    d_dist[v] = val ; 
  }
} // end KER FUNC
