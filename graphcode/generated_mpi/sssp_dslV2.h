#ifndef GENCPP_SSSP_DSLV2_H
#define GENCPP_SSSP_DSLV2_H
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdbool>
#include <fstream>
#include <sys/time.h>
#include <boost/mpi.hpp>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
#include "graph.hpp"

using namespace std;
namespace mpi = boost::mpi;

void Compute_SSSP(graph g,int* dist,int* weight,int src
);
bool is_finished (int startv,int endv,bool* modified)
{
  int sum = 0,res = 0;
  for(int i = startv;i <= endv; i++)
  {
    if (modified[i - startv] == 1)
      res=1;
  }
  MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);
  if(sum ==0) return false;
  else return true;
}

#endif
