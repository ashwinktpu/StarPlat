#include"PageRank_DSL.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)

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
  Compute_PR_kernel<<<num_blocks, block_size>>>(gpu_OA, gpu_edgeList, V, E ;
    cudaDeviceSynchronize();


    float num_nodes = (float)g.num_nodes( );
    for (int t = 0; t < V; g ++) 
    {
      pageRank[t] = 1 / num_nodes;
    }
    int iterCount = 0;
    float diff = 0.0 ;
    do
    diff = 0.000000;
    iterCount++;
    while((diff > beta) && (iterCount < maxIter));
}