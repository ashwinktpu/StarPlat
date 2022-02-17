#include"input8.h"

void test(graph& g)
{
  int* prop=new int[g.num_nodes()];
  int* prop1=new int[g.num_nodes()];
  int x = 0 ;
  int y = 0 ;
  x = 10;
  y = 1;
  y = 2;
  y = 3;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = x;
    prop1[t] = x + y;
  }
  y = 4;
  y = 5;
  y = 6;
  x = 11;

}