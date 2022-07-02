#include"input11.h"

void test(graph& g)
{
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
  }
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    prop[v] = 26;
  }
  int cond = 1;
  if (cond )
    {
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop[v] = 20;
    }
  }
  else
  {
  }

}
