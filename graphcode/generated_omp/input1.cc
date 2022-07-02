#include"input1.h"

void test(graph& g,int src)
{
  int x = 0 ;
  x = 100;
  int y = 0 ;
  y = 80;
  y = x + y;
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 50;
  }
  prop[src] = x;

}
