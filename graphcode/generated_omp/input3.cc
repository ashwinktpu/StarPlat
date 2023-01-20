#include"input3.h"

void test(graph& g)
{
  int y = 10;
  int valid = 1;
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
  }
  while ((y > 0) && valid ){
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop[v] = 20;
      valid = valid / prop[v];
      if (valid )
        {
        y++;
      }
    }
    y--;
  }

}
