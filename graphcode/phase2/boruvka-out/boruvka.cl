#include "libOpenCL.h"
#include "union_find.h"

__kernel void DSU_test_kernel(int V,  int E, __global int* d_meta,
                              __global int* d_data, __global int* d_src,
                              __global int* d_weight,
                              __global int* d_rev_meta,
                              __global int * d_cheapest_w){ // BEGIN KER FUN via ADDKERNEL
  unsigned int u = get_global_id(0);
  if(v >= V) return;

  //ForAll started here
  for (int edge = d_meta[u]; edge < d_meta[u+1]; edge++) { // FOR NBR ITR 
    int v = d_data[edge];
    int e = edge; int u_comp = dsu_find(mydsu, u); 
    int v_comp = dsu_find(mydsu, v); 
    if (u_comp != v_comp){ // if filter begin 
      if (d_weight[e] < d_u_comp[cheapest_w]){ // if filter begin 
        d_u_comp[cheapest_u] = d_source[e];
        d_u_comp[cheapest_v] = d_destination[e];
        d_u_comp[cheapest_w] = d_weight[e];

      } // if filter end
      if (d_weight[e] < d_v_comp[cheapest_w]){ // if filter begin 
        d_v_comp[cheapest_u] = d_source[e];
        d_v_comp[cheapest_v] = d_destination[e];
        d_v_comp[cheapest_w] = d_weight[e];

      } // if filter end

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  if (u == 1){ // if filter begin 
    int mst_size = 0; 

    //ForAll started here
    int comp = dsu_find(mydsu, u); 
    int skip = false; 
    if (d_done[comp]){ // if filter begin 
      skip = true;

    } // if filter end
    if (d_cheapest_w[comp] == INT_MAX){ // if filter begin 
      skip = true;

    } // if filter end
    if (skip == false){ // if filter begin 
      d_done[comp] = true;
      int tmp1 = d_comp[cheapest_u]; 
      int tmp2 = d_comp[cheapest_v]; 
      int src_comp = dsu_find(mydsu, tmp1); 
      int dst_comp = dsu_find(mydsu, tmp2); 
      if (src_comp != dst_comp){ // if filter begin 
        atomic_add( mst_size, (int)d_comp[cheapest_w]);

        dsu_merge(mydsu, src_comp, dst_comp);

      } // if filter end

    } // if filter end


  } // if filter end
} // end KER FUNC
