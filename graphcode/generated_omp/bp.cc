#include"bp.h"

void bp(graph& g , int k , int* weight)
{
  int* cur_prob=new int[g.num_nodes()];
  int* prior_prob=new int[g.num_nodes()];
  int iter = 0;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    cur_prob[t] = 1;
    prior_prob[t] = 1;
  }
  do
  {
    iter++;
    #pragma omp parallel for reduction(+ : new_prob)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      int new_prob = 0;
      for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
      {int nbr = g.edgeList[edge] ;
        int e = edge;
        new_prob = new_prob + weight[e] * prior_prob[v];
      }
      cur_prob[v] = new_prob;
    }
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prior_prob[v] = cur_prob[v];
    }
  }
  while(iter < k);
}


