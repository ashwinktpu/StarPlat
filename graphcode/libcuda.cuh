#include<cuda.h>


template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T initVal)
{
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if(id < V)
  {
    init_array[id]=initVal;
  }
}