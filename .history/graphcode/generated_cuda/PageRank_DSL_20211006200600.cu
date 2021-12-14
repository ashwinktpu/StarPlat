#include"PageRank_DSL.h"

template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T initVal)
{
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if(id < V)
  {
    init_array[id]=initVal;
  }
}
__global__ void Compute_PR_kernel(graph g,float beta,float delta,int maxIter,
  float* pageRank)
{
  unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
  unsigned int v = id;
  if (id < V) 

}
void SSSP(int * OA , int * edgeList , int* cpu_edgeLen  , int src ,int V, int E )
{
  int MAX_VAL = 2147483647 ;
  int * gpu_edgeList;
  int * gpu_edgeLen;
  int * gpu_dist;
  int * gpu_OA;
  bool * gpu_modified_prev;
  bool * gpu_modified_next;
  bool * gpu_finished;


  cudaMalloc(&gpu_OA,sizeof(int) *(1+V));
  cudaMalloc(&gpu_edgeList,sizeof(int) *(E));
  cudaMalloc(&gpu_edgeLen,sizeof(int) *(E));
  cudaMalloc(&gpu_dist,sizeof(int) *(V));
  cudaMalloc(&gpu_modified_prev,sizeof(bool) *(V));
  cudaMalloc(&gpu_modified_next,sizeof(bool) *(V));
  cudaMalloc(&gpu_finished,sizeof(bool) *(1));

  unsigned int block_size;
  unsigned int num_blocks;
   if(V <= 1024)
   {
    block_size  = V;
    block_size  = 1;
  }
  else
  {
    block_size = 1024;
    num_blocks = ceil(((float)V) / block_size);
  }
  bool *  = new bool[1];
  [0] = false;
  for (int t = 0; t < V; t ++) 
  {
    pageRank[t] = 1 / num_nodes;
  }
  cudaMemcpy (gpu_OA, OA, sizeof(int) *(1+V) ,cudaMemcpyHostToDevice);
  cudaMemcpy (gpu_edgeList, edgeList, sizeof(int) *(E) ,cudaMemcpyHostToDevice);
  cudaMemcpy (gpu_edgeLen, cpu_edgeLen , sizeof(int) *(E) ,cudaMemcpyHostToDevice);
  cudaMemcpy (gpu_dist, dist, sizeof(int) *(V) ,cudaMemcpyHostToDevice);
  cudaMemcpy (gpu_modified_prev, modified , sizeof(bool) *(V) ,cudaMemcpyHostToDevice);
  cudaMemcpy (gpu_finished, finished , sizeof(bool) *(1) ,cudaMemcpyHostToDevice);

  int k =0;

  while ( ![0] )
  {
    [0] =true;
    initKernel<bool><<<1,1>>>(1, gpu_finished, true);
    Compute_SSSP_kernel<<<num_blocks , block_size>>>(gpu_OA,gpu_edgeList, gpu_edgeLen ,gpu_dist,src, V ,MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);
    cudaDeviceSynchronize();
    initKernel<bool><<<num_blocks,block_size>>>(V, gpu_modified_prev, false);
    cudaDeviceSynchronize();
    bool *tempModPtr  = gpu_modified_next;
    gpu_modified_next = gpu_modified_prev;
    gpu_modified_prev = tempModPtr;
    ++k;
    if(k==V)
    {
      break;
    }
  }

  cudaMemcpy(dist,gpu_dist , sizeof(int) * (V), cudaMemcpyDeviceToHost);
  char *outputfilename = "output.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");
  for (int i = 0; i <V; i++)
  {
    fprintf(outputfilepointer, "%d  %d\n", i, dist[i]);
  }

}
