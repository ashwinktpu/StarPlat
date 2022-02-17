#include"input3.h"

void test(graph& g)
{
  int* prop=new int[g.num_nodes()];
  int* prop1=new int[g.num_nodes()];
  int* prop2=new int[g.num_nodes()];
  int* prop3=new int[g.num_nodes()];
  int* prop4=new int[g.num_nodes()];
  int x = 0 ;
  int y = 0 ;
  int z = 0 ;
  int a = 0 ;
  int temp = 0 ;
  temp = 4;
  y = x;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = x;
  }
  temp = 4;
  y = x++;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop1[t] = x;
  }
  temp = 4;
  x = 10;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop2[t] = x;
  }
  temp = 4;
  y = x;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop3[t] = x++;
  }
  temp = 4;
  x = 10;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop4[t] = x++;
  }

}