#include"triangle_counting_DSL.h"

void Compute_TC(graph& g)

{
  unsigned V = g.num_nodes();
  unsigned E = g.num_edges();

  int* gpu_OA;
  int* gpu_edgeList;
  int* gpu_edgeList;

  cudaMalloc(&gpu_OA, sizeof(int)*(1+V));
  cudaMalloc(&gpu_edgeList, sizeof(int)*(E));
  cudaMalloc(&gpu_edgeList, sizeof(int)*(E));

  if( V <= 1024)
  {
    block_size = V;
    num_blocks = 1;
  }
  else
  {
    block_size = 1024;
    num_blocks = ceil(((float)V) / block_size);
  }
  cudaMemcpy(&d_gpu_OA,OA, sizeof(int)*(1+V), cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  Compute_TC_kernel<<<num_blocks, block_size>>>(gpu_OA, gpu_edgeList, V, E ;
    cudaDeviceSynchronize();


    long triangle_count = 0;

}