#include <cuda.h>

template <typename T>
__global__ void initKernel0(T* init_array, T id, T init_value) {  // MOSTLY 1 thread kernel inits one type value at index id
  init_array[id] = init_value;
}

template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T init_value) {  // intializes one 1D array with init val
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (id < V) {
    init_array[id] = init_value;
  }
}

template <typename T1, typename T2>
__global__ void initKernel2(unsigned V, T1* init_array1, T1 init_value1, T2* init_array2, T2 init_value2) {  // intializes two 1D array may be of two types
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (id < V) {
    init_array1[id] = init_value1;
    init_array2[id] = init_value2;
  }
}

//Only use for reduce_MIN multigpu
__global__ void Compute_Min(int* array1,int* array2,unsigned V,int devicecount){
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v<=V){
    for(int i=0;i<devicecount;i++){
      if(array2[v]>array1[i*(V+1)+v]){
        array2[v] = array1[i*(V+1)+v];
      }
    }
  }
}

__global__ void Compute_Or(bool* array1,bool* array2,unsigned V,int devicecount){
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v<=V){
    for(int i=0;i<devicecount;i++){
      if(array1[i*(V+1)+v]==true){
        array2[v]=true;
      }
    }
  }
}

template <typename T>
__global__ void Compute_correct(T* array1,T* array2,unsigned V,int devicecount){
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v<=V){
    T elem;
    for(int i=0;i<devicecount;i++){
      if(array1[i*(V+1)+v]!=array2[i*(V+1)+v]){
        elem = array1[i*(V+1)+v];
        break;
      }
      else{
        elem = array2[i*(V+1)+v]; 
      }
    }
    for(int i=0;i<devicecount;i++){
      array1[i*(V+1)+v]=elem;
      array2[i*(V+1)+v]=elem;
    }
  }
}


template<typename T1,typename T2>
__global__ void combineRandom(int V,T1* d_color,T2* d_color1,T2* d_color2){
    unsigned v = blockIdx.x*blockDim.x+threadIdx.x;
    if(v>=V) return;
    T1 x = (T1)d_color1[v];
    T1 y = (T1)d_color2[v];
    for(int i=0;i<32;i++){
      x = x*(T1)2;
    }
    d_color[v]=x+y;
}


//NOT USED
__global__ void accumulate_bc(unsigned n, double* d_delta, double* d_nodeBC, int* d_level, unsigned s) {
  unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid >= n || tid == s || d_level[tid] == -1) return;
  d_nodeBC[tid] += d_delta[tid] / 2.0;
}

//~ initIndex<<<1.1>>>(v,arr,s,val);
template <typename T>
__global__ void initIndex(int V, T* init_array, int s, T init_value) {  // intializes an index 1D array with init val
  //~ unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (s < V) {  // bound check
    init_array[s] = init_value;
  }
}

// Single thread kernels. Basically i++ or i-- on device
template <typename T>
__global__ void incrementDeviceVar(T* d_var) {
  *d_var = *d_var + 1;
}
template <typename T>
__global__ void decrementDeviceVar(T* d_var) {
  *d_var = *d_var - 1;
}


//GPU specific check for neighbours for TC algorithm
__device__ bool findNeighborSorted(int s, int d, int *d_meta, int *d_data)  //we can move this to graph.hpp file
{
  int startEdge = d_meta[s];
  int endEdge = d_meta[s + 1] - 1;

  if (d_data[startEdge] == d)
    return true;
  if (d_data[endEdge] == d)
    return true;

  int mid = (startEdge + endEdge) / 2;

  while (startEdge <= endEdge) {
    if (d_data[mid] == d)
      return true;

    if (d < d_data[mid])
      endEdge = mid - 1;
    else
      startEdge = mid + 1;

    mid = (startEdge + endEdge) / 2;
  }

  return false;
}


#define cudaCheckError()                                                               \
  {                                                                                    \
    cudaError_t e = cudaGetLastError();                                                \
    if (e != cudaSuccess) {                                                            \
      printf("Cuda failure %s:%d: '%s'\n", __FILE__, __LINE__, cudaGetErrorString(e)); \
      exit(0);                                                                         \
    }                                                                                  \
  }
