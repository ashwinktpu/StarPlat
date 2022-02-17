#include"input4.h"

void test(graph& g,int src,int sink)
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
  int a = 0 ;
  int temp = 0 ;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop[t] = temp;
  }
  temp = 4;
  y = prop[src];
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop1[t] = prop[src];
  }
  temp = 4;
  prop[src] = 10;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop2[t] = prop[sink];
  }
  temp = 4;
  y = prop[src];
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop3[t] = prop[sink];
  }
  temp = 4;
  prop[sink] = 10;
  temp = 4;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    prop4[t] = prop[sink]++;
    prop5[t] = prop1[sink]++;
  }
  temp = 4;

}