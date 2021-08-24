#ifndef GENCPP_SSSP_H
#define GENCPP_SSSP_H
#include<iostream>
#include<cstdlib>
#include<cstdbool>
#include<climits>
#include<fstream>
#include <sys/time.h>
#include <boost/mpi.hpp>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
//#include"mpi.h"
//#include"graph.hpp"
#include "graph.hpp"
using namespace std;
namespace mpi = boost::mpi;
void Compute_SSSP(graph g,int src,int* weight);
bool is_finished (int startv,int endv,vector <int> modified)
{
    int sum = 0,res=0;
      res = (int) modified.size();
      printf("[ - Inside fun] size of modified %ld\n",modified.size());
      MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);
      //all_reduce(world, res, sum, mpi::sum<int>());
      printf("The value of sum %d\n",sum);
      if(sum > 0) 
        return true;
      else 
        return false;
  }

  #endif

