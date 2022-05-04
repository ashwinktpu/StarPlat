#include"input2.h"

void Compute_SSSP(graph& g,int* dist,int src)
{
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
    finished = true;
  }

}
