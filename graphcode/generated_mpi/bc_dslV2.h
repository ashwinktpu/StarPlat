#ifndef GENCPP_BC_DSLV2_H
#define GENCPP_BC_DSLV2_H
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
#include "../graph.hpp"

using namespace std;
namespace mpi = boost::mpi;

void Compute_BC(graph g,std::vector<int> sourceSet);
bool is_finished (int startv,int endv,vector <int> modified)
{
  int sum = 0,res = 0;
  res = modified.size();
  MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);
  if(sum > 0) return true;
  else return false;
}

#endif
