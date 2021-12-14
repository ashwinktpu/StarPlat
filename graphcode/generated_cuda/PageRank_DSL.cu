#include"PageRank_DSL.h"

__global__ void Compute_PR_kernel(graph& g,float beta,float delta,int maxIter,
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
    unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
    unsigned int v =id
    {
      float sum = 0.000000;
      for (int edge = gpu_rev_OA[v]; edge < gpu_rev_OA[v+1]; edge ++) 
      {int nbr = srcList[edge] ;
        sum = sum + pageRank[nbr] / (gpu_OA[nbr+1]-gpu_OA[nbr]);
      }
      float val = (1 - delta) / num_nodes + delta * sum;
      diff = diff+ val - pageRank[v];
      pageRank[v] = val;
    }
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}