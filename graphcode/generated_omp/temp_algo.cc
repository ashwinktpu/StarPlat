#include"temp_algo.h"

void count_nodes(graph& g)
{
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = y;
  }
  int x = 0 ;
  int y = 0 ;
  y = x + 10;

}