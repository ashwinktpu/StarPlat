#include"input10.h"

void test(graph& g)
{
  int* prop=new int[g.num_nodes()];
  int* prop1=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
    prop1[t] = 20;
  }
  int cond = 0;
  if (cond )
    {
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop[v] = 10;
    }
  }
  else
  {
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop1[v] = 10;
    }
  }
  if (cond )
    {
    int temp = 0;
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      temp = temp + prop[v];
    }
  }
  else
  {
    int temp = 0;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      temp = temp + prop[v];
    }
  }

}
