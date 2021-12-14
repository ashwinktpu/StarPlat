#include"PageRank_DSL_V2.h"

__global__ void Compute_PR_kernel(graph& g,float beta,float delta,int maxIter,
  float* pageRank)

{
  float num_nodes = (float)g.num_nodes( );
  float* pageRank_nxt=new float[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    pageRank[t] = 1 / num_nodes;
  }
  int iterCount = 0;
  float diff = 0.0 ;
  do
  diff = 0.000000;
  pageRank = pageRank_nxt;
  iterCount++;
  while((diff > beta) && (iterCount < maxIter));
}