#include"input6.h"

void test(graph& g)
{
  int* prop=new int[g.num_nodes()];
  int* prop1=new int[g.num_nodes()];
  int* prop2=new int[g.num_nodes()];
  int* prop3=new int[g.num_nodes()];
  int* prop4=new int[g.num_nodes()];
  int* prop5=new int[g.num_nodes()];
  int x = 0 ;
  int y = 0 ;
  int z = 0 ;
  x = 10;
  y = 11;
  z = 12;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = x;
    prop1[t] = y;
    prop2[t] = z;
  }
  int temp = x++ + z++;
  z = 15;
  y = 12;
  z = 13;
  x = 12;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop3[t] = y;
    prop4[t] = z;
    prop5[t] = x;
  }
  y = 13;
  z = 14;
  x = 15;

}