#include"input7.h"

void test(graph& g,int src)
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
  while (true ){
    int sumV = 0;
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop[v]++;
      sumV = sumV + prop[v];
    }
  }

}
