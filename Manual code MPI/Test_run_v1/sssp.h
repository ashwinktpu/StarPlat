#ifndef GENCPP_SSSP_H
#define GENCPP_SSSP_H
#include<iostream>
#include<cstdlib>
#include<cstdbool>
#include<climits>
#include<fstream>
#include"mpi.h"
#include"graph.hpp"
using namespace std;
void Compute_SSSP(graph g,int src,int* weight);
bool is_finished (int startv,int endv,bool* modified)
{
    int sum,res=0;
    for(int i=startv;i<=endv;i++)
    {
        if (modified[i] == 1)
          res=1;
      }
      MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);
      if(sum ==0) return false;
      else return true;
  }

  #endif

