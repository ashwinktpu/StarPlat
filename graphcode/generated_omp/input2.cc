#include"input2.h"

void test(graph& g,int src)
{
  int x = 0 ;
  int y = 0 ;
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    x = 100;
    y = 60;
  }
  int z = x + y;
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    int temp = x;
    y = 77;
  }
  y = 44;
  x++;
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    int temp1 = x;
  }

}
