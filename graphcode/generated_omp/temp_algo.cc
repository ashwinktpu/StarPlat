#include"temp_algo.h"

void count_nodes(graph& g)
{
  int x = 0 ;
  int y = 0 ;
  int z = 0 ;
  int w = 0;
  int* prop=new int[g.num_nodes()];
  y = x + 10;
  z = x;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = (x || z);
  }
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    prop[v] = y;
  }

}
