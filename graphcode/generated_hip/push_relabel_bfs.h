#ifndef GENHIP_PUSH_RELABEL_BFS_H
#define GENHIP_PUSH_RELABEL_BFS_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"

void do_max_flow(
  graph&  g,int  source,int  sink,int  kernel_parameter
);

__device__ int source ;
__device__ int sink ;
__device__ int kernel_parameter ;

__device__ int excesstotal; // DEVICE ASSTMENT in .h
__global__ void do_max_flow_kernel0(int V, int E, int* dOffsetArray, int* dEdgelist, int* dResidual_capacity, int* dExcess){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source) {
    for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR 
      int vv = dEdgelist[edge];

      // Traversing for get_edge
      int forward_edge = 0;
      for(int edge=dOffsetArray[v]; edge<dOffsetArray[v+1]; edge++){
        if(dEdgelist[edge] == vv){
          forward_edge = edge;
        }
      }


      // Traversing for get_edge
      int backward_edge = 0;
      for(int edge=dOffsetArray[vv]; edge<dOffsetArray[vv+1]; edge++){
        if(dEdgelist[edge] == v){
          backward_edge = edge;
        }
      }

      int d = dResidual_capacity[forward_edge];  // DEVICE ASSTMENT in .h
      atomicAdd(& excesstotal, (int)d);
      atomicAdd(&dExcess[vv], (int)d);
      atomicAdd(&dResidual_capacity[forward_edge], (int)-d);
      atomicAdd(&dResidual_capacity[backward_edge], (int)d);

    } //  end FOR NBR ITR. TMP FIX!

  } 
} // end KER FUNC

__device__ bool flag; // DEVICE ASSTMENT in .h
__global__ void do_max_flow_kernel1(int V, int E, int* dOffsetArray, int* dEdgelist, int* dExcess, int* dLabel, int* dResidual_capacity){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int x = kernel_parameter;  // DEVICE ASSTMENT in .h
  do{
    x = x - 1;
    int hh = 100000000;  // DEVICE ASSTMENT in .h
    int lv = -1;  // DEVICE ASSTMENT in .h
    if (dExcess[v] > 0 && v != source && v != sink && dLabel[v] < V) {
      for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR 
        int vv = dEdgelist[edge];

        // Traversing for get_edge
        int e = 0;
        for(int edge=dOffsetArray[v]; edge<dOffsetArray[v+1]; edge++){
          if(dEdgelist[edge] == vv){
            e = edge;
          }
        }

        int xxx = dResidual_capacity[e];  // DEVICE ASSTMENT in .h
        if (dLabel[vv] < hh && xxx > 0) {
          lv = vv;
          hh = dLabel[vv];

        } 

      } //  end FOR NBR ITR. TMP FIX!
      if (dLabel[v] > hh && lv != -1) {

        // Traversing for get_edge
        int current_edge = 0;
        for(int edge=dOffsetArray[v]; edge<dOffsetArray[v+1]; edge++){
          if(dEdgelist[edge] == lv){
            current_edge = edge;
          }
        }

        int cec = dResidual_capacity[current_edge];  // DEVICE ASSTMENT in .h
        if (dExcess[v] > 0 && cec > 0) {

          // Traversing for get_edge
          int forward_edge = 0;
          for(int edge=dOffsetArray[v]; edge<dOffsetArray[v+1]; edge++){
            if(dEdgelist[edge] == lv){
              forward_edge = edge;
            }
          }


          // Traversing for get_edge
          int backward_edge = 0;
          for(int edge=dOffsetArray[lv]; edge<dOffsetArray[lv+1]; edge++){
            if(dEdgelist[edge] == v){
              backward_edge = edge;
            }
          }

          int fec = dResidual_capacity[forward_edge];  // DEVICE ASSTMENT in .h
          int bec = dResidual_capacity[backward_edge];  // DEVICE ASSTMENT in .h
          int d = fec;  // DEVICE ASSTMENT in .h
          if (dExcess[v] < fec) {
            d = dExcess[v];

          } 
          atomicAdd(&dExcess[v], (int)-d);
          atomicAdd(&dExcess[lv], (int)d);
          atomicAdd(&dResidual_capacity[forward_edge], (int)-d);
          atomicAdd(&dResidual_capacity[backward_edge], (int)d);

        } 

      }  else {
        if (lv != -1) {
          dLabel[v] = hh + 1;

        } 
      }

    } 

  }while(x > 0);
} // end KER FUNC

__global__ void do_max_flow_kernel2(int V, int E, int* dOffsetArray, int* dEdgelist, int* dResidual_capacity, int* dExcess, int* dLabel){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR 
    int vv = dEdgelist[edge];

    // Traversing for get_edge
    int e = 0;
    for(int edge=dOffsetArray[v]; edge<dOffsetArray[v+1]; edge++){
      if(dEdgelist[edge] == vv){
        e = edge;
      }
    }


    // Traversing for get_edge
    int ee = 0;
    for(int edge=dOffsetArray[vv]; edge<dOffsetArray[vv+1]; edge++){
      if(dEdgelist[edge] == v){
        ee = edge;
      }
    }

    int ec = dResidual_capacity[e];  // DEVICE ASSTMENT in .h
    if (ec > 0 && dLabel[v] > dLabel[vv] + 1 && dExcess[v] > 0) {
      int d = ec;  // DEVICE ASSTMENT in .h
      if (d > dExcess[v]) {
        d = dExcess[v];
      } 
      atomicAdd(&dExcess[v], (int)-d);
      atomicAdd(&dExcess[vv], (int)d);
      atomicAdd(&dResidual_capacity[e], (int)-d);
      atomicAdd(&dResidual_capacity[ee], (int)d);

    } 

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC

__global__ void fwd_pass(int n, int* dOffsetArray, int* dEdgelist, int* dRevOffsetArray, int* SrcList, int* dWeight, bool* dFinished, bool* dVisitBfs, int* dVisit, int* dResidual_capacity, int* dLabel) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if (dVisit[v] == 1) {
    for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++)
    {
      int w = dSrcList[edge] ;

      // Traversing for get_edge
      int e = 0;
      for(int edge=dOffsetArray[w]; edge<dOffsetArray[w+1]; edge++){
        if(dEdgelist[edge] == v){
          e = edge;
        }
      }

      int ec = dResidual_capacity[e];  // DEVICE ASSTMENT in .h
      if (dVisit[w] == 0 && ec > 0) {
        dVisit[w] = 1;
        dLabel[w] = dLabel[v] + 1;

      } 

    } //  end FOR NBR ITR. TMP FIX!
    dVisit[v] = 2;

  } 
  dVisitBfs[v] = true;
  for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++) { // FOR NBR REV ITR 
    int w = dSrcList[edge];
    if (dVisitBfs[w] == false) {
      *dFinished = false;
      dVisitBfs[w] = true;
    }
  }
} // kernel end

__global__ void do_max_flow_kernel3(int V, int E, int* dVisit, int* dExcess, int* dLabel){ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (dVisit[v] == 0) {
    dLabel[v] = 1000000000;
    int x = dExcess[v];  // DEVICE ASSTMENT in .h
    atomicAdd(& excesstotal, (int)-x);
    dExcess[v] = 0;

  } 
} // end KER FUNC

template <typename T>
__global__ void initIndex(int V, T* init_array, int s, T init_value) {  // intializes an index 1D array with init val
  if (s < V) {  // bound check
    init_array[s] = init_value;
  }
}
template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T init_value) {  // intializes one 1D array with init val
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (id < V) {
    init_array[id] = init_value;
  }
}

#endif
