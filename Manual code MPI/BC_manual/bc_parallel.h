#ifndef GENCPP_SSSP_H
#define GENCPP_SSSP_H
#include<iostream>
#include<cstdlib>
#include<cstdbool>
#include<climits>
#include<fstream>
#include<stack>
#include<queue>
#include <sys/time.h>
#include"mpi.h"
#include"graph_old.hpp"
using namespace std;
void Compute_SSSP(graph g,int src,int* weight,vector<int> seeds);

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
int get_maxDistance (int startv,int endv,int* d)
{
    //printf("at getmaxD\n");
    int lmax=-1,gmax=-1;
    for(int i=startv;i<=endv;i++)
    {
        if (d[i] > lmax)
          lmax = d[i];
    }
      MPI_Allreduce(&lmax, &gmax, 1, MPI_INT, MPI_MAX,MPI_COMM_WORLD);
      return gmax;
      //if(sum ==0) return false;
      //else return true;
}

  #endif

