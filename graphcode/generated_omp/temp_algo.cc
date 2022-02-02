#include"temp_algo.h"

void count_nodes(graph& g)
{
  int* prop1=new int[g.num_nodes()];
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
    prop1[t] = 10;
  }
  int result = 0;
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    int* prop3=new int[g.num_nodes()];
    int* prop2=new int[g.num_nodes()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      prop2[t] = 10;
      prop3[t] = result;
    }
    result = result + 1;
    prop[v] = 11;
  }

}