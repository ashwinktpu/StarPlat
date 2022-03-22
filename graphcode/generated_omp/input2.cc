#include"input2.h"

void Compute_SSSP(graph& g)
{
  int x = 0 ;
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    if (modified[v] == true ){
      x = x + 10;
    }
  }

}