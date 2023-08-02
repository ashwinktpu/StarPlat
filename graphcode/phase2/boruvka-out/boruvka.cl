#include "libOpenCL.h"
__kernel void DSU_test_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;

  //ForAll started here
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int v = d_data[edge];
    int e = edge; int u_comp = ; 
    int v_comp = ; 
    if (u_comp != v_comp){ // if filter begin 
      if (d_weight[e] < d_u_comp[cheapest]){ // if filter begin 
        d_u_comp[cheapest] = 0;

      } // if filter end
      if (d_weight[e] < d_v_comp[cheapest]){ // if filter begin 
        d_v_comp[cheapest] = 0;

      } // if filter end

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
