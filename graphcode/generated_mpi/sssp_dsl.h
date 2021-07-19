#ifndef GENCPP_SSSP_DSL_H
#define GENCPP_SSSP_DSL_H
#include<iostream>
#include<cstdlib.h>
#include<climits>
#include<cstdbool>
#include<fstream>
#include<sys/time.h>
#include"mpi.h"
#include"../graph.hpp"

void Compute_SSSP(graph& g,int src);
bool is_finished (int startv,int endv,bool* modified)
{
  int sum = 0,res = 0;
  for(int i = startv;i <= endv; i++)
  {
    if (modified[i] == 1)
      res=1;
  }
  MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);
  if(sum ==0) return false;
  else return true;
}

#endif
