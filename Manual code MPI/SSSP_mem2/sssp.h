#ifndef GENCPP_SSSP_H
#define GENCPP_SSSP_H
#include<iostream>
#include<cstdlib>
#include<cstdbool>
#include<climits>
#include<fstream>
#include <stdbool.h>
#include <sys/time.h>
#include"mpi.h"
#include"graph.hpp"
using namespace std;
void Compute_SSSP(graph g,int src,int* weight);
bool is_finished (bool* local_modified,int part_size)
{
    int sum=0;
    int res=0;
    for(int i = 0; i < part_size; i++)
    {
        if (local_modified[i] == 1)
          res=1;
    }
      MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);
      if(sum == 0) return false;
      else return true;
  }

  #endif

