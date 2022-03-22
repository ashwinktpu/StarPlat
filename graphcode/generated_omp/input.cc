#include"input.h"

void Compute_SSSP(graph& g,int src)
{
  int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
    modified_nxt[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  while ( !finished )
  {
    finished = true;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
      {int v = g.srcList[edge] ;
        if (modified[nbr] == true ){
          int e = edge;
          if (dist[v] > dist[nbr] + weight[e] ){
            dist[v] = dist[nbr] + weight[e];
            modified[v] = true;
          }
        }
      }
    }
  }

}