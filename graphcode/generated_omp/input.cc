#include"input.h"

void test(graph& g)
{
  int x = 0;
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
  }
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    x = x+ prop[v];
  }

}