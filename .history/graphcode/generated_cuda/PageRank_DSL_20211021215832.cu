#include"PageRank_DSL.h"

__global__ void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)
{
  float num_nodes = (float)g.num_nodes( );
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    pageRank[t] = 1 / num_nodes;
  }
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    .
    {
      float sum = 0.000000;
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
      {int nbr = g.srcList[edge] ;
        sum = sum + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
      }
      float val = (1 - delta) / num_nodes + delta * sum;
      diff = diff+ val - pageRank[v];
      pageRank[v] = val;
    }
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}
void Compute_PR(int * rev_OA, int * OA, int * cpu_srcList , float * node_pr , int V, int E)
{
  int *gpu_rev_OA;
  int *gpu_srcList;
  int * gpu_OA;
  float * gpu_node_pr;
  cudaMalloc( &gpu_rev_OA, sizeof(int) * (1+V) );
  cudaMalloc( &gpu_OA, sizeof(int) * (1+V) );
  cudaMalloc( &gpu_srcList, sizeof(int) * (E) );
  cudaMalloc( &gpu_node_pr, sizeof(float) * (V) );
  unsigned int block_size;
  unsigned int num_blocks;
  for(int i=0; i< V; i++)
  {
    node_pr[i]= 1.0/V;
  }
  if(V <= 1024)
  {
    block_size = V;
    num_blocks = 1;
    else
    {
      block_size = 1024;
      num_blocks = ceil(((float)V) / block_size);
    }
    cudaMemcpy(gpu_rev_OA, rev_OA, sizeof(int) * (1+V), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_OA, OA, sizeof(int) * (1+V), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_srcList, cpu_srcList, sizeof(int) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_node_pr, node_pr , sizeof(float) * (V), cudaMemcpyHostToDevice);
    float beta = 0.001;
    float delta = 0.85;
    int maxIter = 100;
    int iterCount=0;
    float diff;
