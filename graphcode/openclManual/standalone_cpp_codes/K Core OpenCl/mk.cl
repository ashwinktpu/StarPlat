#include "libOpenCL.h"
__kernel void Compute_TC_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int* triangle_count){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  printf("working on vertex %d",v);
  //ForAll started here
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int u = d_data[edge];
    if (u < v){ // if filter begin 

      //ForAll started here
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int w = d_data[edge];
        if (w > v){ // if filter begin 
          if (isNeighbour(u, w, d_meta, d_data)){ // if filter begin 
            atomic_add( triangle_count, (int)1);


          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
