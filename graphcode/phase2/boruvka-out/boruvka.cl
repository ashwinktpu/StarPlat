#include "libOpenCL.h"
__kernel void initcheapest_kernel( __global int *d_cheapest , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_cheapest[id] = value;
  }
}
__kernel void initd_cheapest_next_kernel(__global int *d_cheapest_next, int val ,  int v){
  unsigned int id = get_global_id(0);
  if(id<v)
  {
    d_cheapest_next[id] = val;
  }
}
__kernel void MST_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int *finished,__global int * d_cheapest_next,__global int * d_cheapest){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  d_cheapest[v] = 3;
} // end KER FUNC
