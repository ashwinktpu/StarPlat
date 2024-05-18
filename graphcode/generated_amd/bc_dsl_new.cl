#include "libOpenCL.h"
__kernel void initBC_kernel( __global double *d_BC , double value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_BC[id] = value;
  }
}
__kernel void initdelta_kernel( __global double *d_delta , double value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_delta[id] = value;
  }
}
__kernel void initsigma_kernel( __global double *d_sigma , double value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_sigma[id] = value;
  }
}

__kernel void initIndexsigma_kernel(__global double *d_sigma, int src , double val){
  d_sigma[src] = val ; 
}
__kernel void initd_level_kernel(__global int *d_level, unsigned int V)
{
  unsigned int id = get_global_id(0);
  if(id>=V) return ;
  d_level[id] = -1;
}
__kernel void initIndexd_level_kernel(__global int* d_level, int src)
{
  d_level[src] = 0;
}
__kernel void fwd_pass_kernel(int V, __global int* d_meta, __global int* d_data, __global int* d_weight, __global int* d_level, __global int* d_hops_from_source, __global int* d_finished
  ,__global double* d_sigma ,__global double* d_delta ,__global double* d_BC)
{
  unsigned int v = get_global_id(0);
  if(v>= V) return ;
  if(d_level[v]== (*d_hops_from_source))
  {

    //ForAll started here
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if(d_level[w] == -1)
      {
        d_level[w] = (*d_hops_from_source + 1);
        *d_finished = 1;
      }
      if(d_level[w] == (*d_hops_from_source + 1))
      {
        atomicAddD(&d_sigma[w],  d_sigma[v]);

      }
    }
  }// end of if block
} // end of kernel
__kernel void back_pass_kernel(int V, __global int* d_meta, __global int* d_data, __global int* d_weight, __global int* d_level, __global int* d_hops_from_source, __global int* d_finished,__global double* d_sigma ,__global double* d_delta ,__global double* d_BC)
{
  unsigned int v = get_global_id(0);
  if(v>= V) return ;
  if(d_level[v]==(*d_hops_from_source)-1)
  {

    //ForAll started here
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if(d_level[w] == *d_hops_from_source)
      {
        atomicAddD(&d_delta[v],  (d_sigma[v] / d_sigma[w]) * (1 + d_delta[w]));

      } // end IF  
    } // end FOR
    barrier(CLK_GLOBAL_MEM_FENCE);
    d_BC[v] = d_BC[v] + d_delta[v];
  }//end of if
}//end of kernel
