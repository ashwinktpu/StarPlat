#include"input12.h"

void test(graph& g,int src)
{
  int* prop=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
  }
  bool tempVar_݄1V = false;
  do
  {
    if (tempVar_݄1V )
      {
    }
    tempVar_݄1V = true;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop[v] = 10;
    }
  }
  while(true);bool tempVar_P݄1V = false;
  do
  {
    if (tempVar_P݄1V )
      {
    }
    tempVar_P݄1V = true;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      prop[v] = 10;
    }
    prop[src] = 200;
  }
  while(true);int temp = 0;
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    prop[v] = 22;
    temp = temp + prop[v];
  }
  bool tempVar_P݄1V = false;
  do
  {
    if (tempVar_P݄1V )
      {
    }
    tempVar_P݄1V = true;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      int tempGPU = prop[v];
    }
  }
  while(true);
}
