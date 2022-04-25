#include"PageRankDSLV2.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)
{
  float num_nodes = (float)g.num_nodes( );
  float* pageRank_nxt=new float[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    pageRank[t] = 1 / num_nodes;
  }
  int iterCount = 0;
  float diff = 0.0 ;
  bool €ñ_·y = false;
  do
  {
    if (€ñ_·y )
      {
    }
    €ñ_·y = true;
    diff = 0.000000;
    #pragma omp parallel for reduction(+ : diff)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      float sum = 0.000000;
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
      {int nbr = g.srcList[edge] ;
        sum = sum + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
      }
      float val = (1 - delta) / num_nodes + delta * sum;
      diff = diff + val - pageRank[v];
      pageRank_nxt[v] = val;
    }
    #pragma omp parallel for
    for (int node = 0; node < g.num_nodes(); node ++) 
    {
      pageRank [node] = pageRank_nxt [node] ;
    }
  }
  while((diff > beta) && (iterCount < maxIter));
}
