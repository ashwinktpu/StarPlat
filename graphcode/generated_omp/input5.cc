#include"input5.h"

void test(graph& g,int src)
{
  int* prop=new int[g.num_nodes()];
  int* prop1=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 110;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop1[t] = prop[src];
  }

}