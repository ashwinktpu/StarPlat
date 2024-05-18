#include "libOpenCL.h"
__kernel void initA_kernel( __global int *d_A , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_A[id] = value;
  }
}
__kernel void reductionExample_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int* accum,__global int * d_A){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  int count = 0; 

  //ForAll started here
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    count = count + d_A[nbr];

  } //  end FOR NBR ITR. TMP FIX!
  atomic_add( accum, (int)count);

} // end KER FUNC
