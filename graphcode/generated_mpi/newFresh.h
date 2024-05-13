#ifndef GENCPP_NEWFRESH_H
#define GENCPP_NEWFRESH_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto find(int u, std::vector<int>& parent, boost::mpi::communicator world );
void merge(int u, int v, std::vector<int>& parent, std::vector<int>& rnk
  , boost::mpi::communicator world );
auto qi_seq(std::vector<int>& degree, std::vector<int>& edges, int n, std::vector<int>& parent, 
  std::vector<int>& rnk, boost::mpi::communicator world );
auto quicksi(std::vector<int>& degree, std::vector<std::vector<int>>& records, Graph& g, int d, 
  std::vector<int>& H, std::vector<int>& F, boost::mpi::communicator world );

#endif
