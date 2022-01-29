#include"temp_algo.h"

void count_nodes(graph& g)
{
  int result = 0;
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    result = result + 1;
  }

}