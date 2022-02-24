#include"input1.h"

void test(graph& g,int src)
{
  int y = 0 ;
  int x = 0 ;
  int* prop=new int[g.num_nodes()];
  if (y ){
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      int t = x + 1;
      prop[v] = 100;
    }
  }
  else
  {
    prop[src] = 100;
  }
  y = x;

}