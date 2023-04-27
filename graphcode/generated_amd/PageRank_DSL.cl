#include "libOpenCL.h"
__kernel void initpageRank_kernel( __global float *d_pageRank , float value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_pageRank[id] = value;
  }
}
__kernel void Compute_PR_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta,__global float *d_diff,__global float *d_delta,__global float *d_num_nodes,__global float* d_pageRank){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;

  float sum = 0.000000; 

  //ForAll started here
  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {
    int nbr = d_src[edge] ;
    sum = sum + d_pageRank[nbr] / (d_meta[nbr+1]-d_meta[nbr]);

  } //  end FOR NBR ITR. TMP FIX!

  float val = (1 - (*d_delta)) / (*d_num_nodes) + (*d_delta) * sum; 
  atomicAddF(d_diff, (float)val - d_pageRank[v]);
  d_pageRank[v] = val;
} // end KER FUNC
