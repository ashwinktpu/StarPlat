#include "libOpenCL.h"
__kernel void initdist_kernel( __global int *d_dist , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_dist[id] = value;
  }
}
__kernel void initmodified_kernel( __global int *d_modified , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_modified[id] = value;
  }
}

__kernel void initIndexmodified_kernel(__global int *d_modified, int src , int val){
  d_modified[src] = val ; 
}

__kernel void initIndexdist_kernel(__global int *d_dist, int src , int val){
  d_dist[src] = val ; 
}
__kernel void initd_modified_next_kernel(__global int *d_modified_next, int val ,  int v){
  unsigned int id = get_global_id(0);
  if(id<v)
  {
    d_modified_next[id] = val;
  }
}
__kernel void Compute_SSSP_new_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int *finished,__global int * d_modified_next,__global int * d_modified,__global int * d_weight,__global int * d_dist){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  if (d_modified[v] == true){ // if filter begin 

    //ForAll started here
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e = edge; if (d_dist[nbr] > d_dist[v] + d_weight[e]){ // if filter begin 
        d_dist[nbr] = d_dist[v] + d_weight[e];
        d_modified[nbr] = true;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
