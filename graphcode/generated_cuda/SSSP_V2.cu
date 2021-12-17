#include"SSSP_V2.h"

void Compute_SSSP(int * OA , int * edgeList , int* cpu_edgeLen  , int src)

{
  unsigned V = g.num_nodes();
  unsigned E = g.num_edges();

  int MAX_VAL = 2147483647 ;
  int * gpu_edgeList;
   int * gpu_edgeLen;
  int * gpu_dist;
   int * gpu_OA;
  bool * gpu_modified_prev;
  bool * gpu_finished;
  int *gpu_rev_OA;
  int *gpu_srcList;
  float  *gpu_node_pr;

  cudaMalloc(&gpu_OA, sizeof(int)*(1+V));
  cudaMalloc(&gpu_edgeList, sizeof(int)*(E));
  cudaMalloc(&gpu_edgeLen, sizeof(int)*(E));
  cudaMalloc(&gpu_dist, sizeof(int)*(V));
  cudaMalloc(&gpu_modified_prev, sizeof(bool)*(V));
  cudaMalloc(&gpu_modified_next, sizeof(bool)*(V));
  cudaMalloc(&gpu_finished, sizeof(bool)*(1));
  cudaMalloc(&gpu_srcList, sizeof(int)*(E));
  cudaMalloc(&gpu_node_pr, sizeof(flaot)*(V));

  unsigned int block_size;
  unsigned int num_blocks;
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
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeList,edgeList, sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_edgeLen,cpu_edgeLen , sizeof(int)*E, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_dist,modified , sizeof(bool)*V, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_finished,finished , sizeof(bool)*1, cudaMemcpyHostToDevice);
  cudaMemcpy(&d_gpu_srcList,cpu_srcList, sizeof(int)*(E), cudaMemcpyHostToDevice);
  Compute_SSSP_kernel<<<num_blocks, block_size>>>(gpu_OA, gpu_edgeList, V, E ;
    cudaDeviceSynchronize();


    bool* modified=new bool[g.num_nodes()];
    bool* modified_nxt=new bool[g.num_nodes()];
    for (int t = 0; t < V; g ++) 
    {
      dist[t] = INT_MAX;
      modified[t] = false;
      modified_nxt[t] = false;
    }
    modified[src] = true;
    dist[src] = 0;
    bool finished = false;
    while ( !finished[0] )
    {
      finished[0] = true;
      {
      }
       initKernel<bool> <<< 1, 1>>>(1, gpu_finished, true);
       Compute_SSSP_kernel<<<num_blocks , block_size>>>(gpu_OA,gpu_edgeList, gpu_edgeLen ,gpu_dist,src, V ,MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);
       initKernel<bool><<<num_blocks,block_size>>>(V, gpu_modified_prev, false);
       cudaMemcpy(finished, gpu_finished,  sizeof(bool) *(1), cudaMemcpyDeviceToHost);
      bool* tempModPtr = modified_nxt ;
      modified_nxt = modified_prev ;
      modified_prev = tempModPtr ;
      modified_nxt[v] = false ;

}