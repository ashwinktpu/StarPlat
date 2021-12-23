#include<cuda.h>


template <typename T>
__global__ void initKernel0(T* init_array, T id, T init_value) { // MOSTLY 1 thread kernel inits one type value at index id
  init_array[id]=init_value;
}


template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T init_value) { // intializes one 1D array with init val
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if(id < V)   {
    init_array[id]=init_value;
  }
}

template <typename T1, typename T2>
__global__ void initKernel2( unsigned V, T1* init_array1, T1 init_value1, T2* init_array2, T2 init_value2)  { // intializes two 1D array may be of two types
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if(id < V) {
    init_array1[id]=init_value1;
    init_array2[id]=init_value2;
  }
}
