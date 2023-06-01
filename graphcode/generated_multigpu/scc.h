// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or afte
#ifndef GENCPP_SCC_H
#define GENCPP_SCC_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void vHong(graph& g);



__global__ void vHong_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_outDeg,int* d_inDeg){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  int nxtSrc = src + 1; // DEVICE ASSTMENT in .h

  d_inDeg[src] = d_inDeg[src]+ d_rev_meta[nxtSrc] - d_rev_meta[src];
  d_outDeg[src] = d_outDeg[src]+ d_meta[nxtSrc] - d_meta[src];
} // end KER FUNC
__device__ bool fpoint1 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel_2(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_range,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int havePar = 0; // DEVICE ASSTMENT in .h

    int haveChild = 0; // DEVICE ASSTMENT in .h

    for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
    {int par = d_src[edge] ;
      if (d_scc[par] == -1){ // if filter begin 
        if (d_range[par] == d_range[src]){ // if filter begin 
          havePar = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      if (d_scc[dst] == -1){ // if filter begin 
        if (d_range[dst] == d_range[src]){ // if filter begin 
          haveChild = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
      d_scc[src] = src;
      d_isPivot[src] = true;
      fpoint1 = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_3(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_inDeg,int* d_scc,int* d_outDeg,int* d_pivotField,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int color = d_range[src]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    int oldSrcValue = -1; // DEVICE ASSTMENT in .h

    int oldSrc = d_pivotField[index]; // DEVICE ASSTMENT in .h

    if (oldSrc >= 0){ // if filter begin 
      oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];

    } // if filter end
    if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){ // if filter begin 
      d_pivotField[index] = src;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_4(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_pivotField,int* d_range,bool* d_visitFw,bool* d_visitBw,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int color = d_range[src]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    if (d_pivotField[index] == src){ // if filter begin 
      d_visitFw[src] = true;
      d_visitBw[src] = true;
      d_isPivot[src] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ bool fpoint2 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel_5(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_visitBw,bool* d_propFw,int* d_scc,bool* d_propBw,int* d_range,bool* d_visitFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int myrange = d_range[src]; // DEVICE ASSTMENT in .h

    if (d_propFw[src] == false && d_visitFw[src] == true){ // if filter begin 
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
            d_visitFw[dst] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propFw[src] = true;

    } // if filter end
    if (d_propBw[src] == false && d_visitBw[src] == true){ // if filter begin 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
      {int par = d_src[edge] ;
        if (d_scc[par] == -1){ // if filter begin 
          if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
            d_visitBw[par] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propBw[src] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_6(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_range,bool* d_visitBw,bool* d_visitFw,bool* d_propBw,bool* d_propFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    if (d_visitFw[src] == false || d_visitBw[src] == false){ // if filter begin 
      int ext = 0; // DEVICE ASSTMENT in .h
      if (d_visitFw[src] == false && d_visitBw[src]==true){ // if filter begin 
        ext = 1;

      } // if filter end
      if (d_visitBw[src] == false && d_visitBw[src] == false){ // if filter begin 
        ext = 2;

      } // if filter end
      int newRange = 3 * d_range[src] + ext;
      newRange = newRange - (newRange / V) * V;
      d_range[src] = newRange + ext;
      d_visitFw[src] = false;
      d_visitBw[src] = false;
      d_propFw[src] = false;
      d_propBw[src] = false;

    } // if filter end
    if (d_visitFw[src] == true && d_visitBw[src] == true){ // if filter begin 
      d_scc[src] = src;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_7(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_range,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int havePar = 0; // DEVICE ASSTMENT in .h

    int haveChild = 0; // DEVICE ASSTMENT in .h

    for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
    {int par = d_src[edge] ;
      if (d_scc[par] == -1){ // if filter begin 
        if (d_range[par] == d_range[src]){ // if filter begin 
          havePar = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      if (d_scc[dst] == -1){ // if filter begin 
        if (d_range[dst] == d_range[src]){ // if filter begin 
          haveChild = 1;

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if ((havePar == 0) || (haveChild == 0)){ // if filter begin 
      d_scc[src] = src;
      d_isPivot[src] = true;
      fpoint1 = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_8(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  d_range[src] = d_range[src]+ src;
} // end KER FUNC
__device__ bool fpoint4 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel_9(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      if (d_scc[dst] == -1){ // if filter begin 
        if (d_range[dst] > d_range[src]){ // if filter begin 
          d_range[src] = d_range[dst];
          fpoint4 = false;

        } // if filter end

      } // if filter end

    }
    
    for(int edge = d_rev_meta[src];edge<d_rev_meta[src+1];edge++){
      int dst = d_src[edge];
    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_10(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int myrange = d_range[src]; // DEVICE ASSTMENT in .h

    if ((myrange != src) && (myrange != d_range[myrange])){ // if filter begin 
      d_range[src] = d_range[myrange];
      fpoint4 = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ bool fpoint5 ; // DEVICE ASSTMENT in .h

__global__ void vHong_kernel_11(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_inDeg,int* d_scc,int* d_outDeg,int* d_pivotField,int* d_range){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int color = d_range[src]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    int oldSrcValue = -1; // DEVICE ASSTMENT in .h

    int oldSrc = d_pivotField[index]; // DEVICE ASSTMENT in .h

    if (oldSrc >= 0){ // if filter begin 
      oldSrcValue = d_inDeg[oldSrc] + d_outDeg[oldSrc];

    } // if filter end
    if (oldSrcValue < (d_inDeg[src] + d_outDeg[src])){ // if filter begin 
      d_pivotField[index] = src;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_12(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_pivotField,int* d_range,bool* d_visitFw,bool* d_visitBw,bool* d_isPivot){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int color = d_range[src]; // DEVICE ASSTMENT in .h

    int index = color - (color / V) * V; // DEVICE ASSTMENT in .h

    if (d_pivotField[index] == src){ // if filter begin 
      d_visitFw[src] = true;
      d_visitBw[src] = true;
      d_isPivot[src] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_13(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,bool* d_visitBw,bool* d_propFw,int* d_scc,bool* d_propBw,int* d_range,bool* d_visitFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    int myrange = d_range[src]; // DEVICE ASSTMENT in .h

    if (d_propFw[src] == false && d_visitFw[src] == true){ // if filter begin 
      for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
        int dst = d_data[edge];
        if (d_scc[dst] == -1){ // if filter begin 
          if (d_visitFw[dst] == false && d_range[dst] == myrange){ // if filter begin 
            d_visitFw[dst] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propFw[src] = true;

    } // if filter end
    if (d_propBw[src] == false && d_visitBw[src] == true){ // if filter begin 
      for (int edge = d_rev_meta[src]; edge < d_rev_meta[src+1]; edge++)
      {int par = d_src[edge] ;
        if (d_scc[par] == -1){ // if filter begin 
          if (d_visitBw[par] == false && d_range[par] == myrange){ // if filter begin 
            d_visitBw[par] = true;
            fpoint2 = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      d_propBw[src] = true;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void vHong_kernel_14(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,bool *d_modified_next,int* d_scc,int* d_range,bool* d_visitBw,bool* d_visitFw,bool* d_propBw,bool* d_propFw){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_scc[src] == -1){ // if filter begin 
    if (d_visitFw[src] == false || d_visitBw[src] == false){ // if filter begin 
      fpoint5 = false;
      int ext1 = 0; // DEVICE ASSTMENT in .h

      int ext2 = 0; // DEVICE ASSTMENT in .h

      if (d_visitFw[src] == false){ // if filter begin 
        ext1 = 1;

      } // if filter end
      if (d_visitBw[src] == false){ // if filter begin 
        ext2 = 1;

      } // if filter end
      int newColor = 3 * d_range[src]; // DEVICE ASSTMENT in .h

      newColor = newColor - (newColor / V) * V;
      d_range[src] = newColor + ext1 + ext2;
      d_visitFw[src] = false;
      d_visitBw[src] = false;
      d_propFw[src] = false;
      d_propBw[src] = false;

    } // if filter end
    if (d_visitFw[src] == true && d_visitBw[src] == true){ // if filter begin 
      d_scc[src] = src;

    } // if filter end

  } // if filter end
} // end KER FUNC

#endif
