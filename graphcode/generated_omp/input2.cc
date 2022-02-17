#include"input2.h"

void test(graph& g,int* prop)
{
  int* prop1=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 110;
    prop1[t] = 111;
  }

}