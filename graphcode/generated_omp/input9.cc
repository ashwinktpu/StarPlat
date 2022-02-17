#include"input9.h"

void test(graph& g)
{
  int* prop=new int[g.num_nodes()];
  int* prop1=new int[g.num_nodes()];
  int* prop7=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = 10;
  }
  if (true ){
    int* prop2=new int[g.num_nodes()];
    int* prop3=new int[g.num_nodes()];
    int x = 11;
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      prop2[t] = 10;
      prop3[t] = 11;
    }
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop1[t] = 15;
  }
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    int* prop9=new int[g.num_nodes()];
    int* prop10=new int[g.num_nodes()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      prop9[t] = 13;
      prop10[t] = 14;
    }
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop7[t] = 13;
  }

}