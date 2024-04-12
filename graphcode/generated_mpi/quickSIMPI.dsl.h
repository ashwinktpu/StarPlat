#ifndef GENCPP_QUICKSIMPI.DSL_H
#define GENCPP_QUICKSIMPI.DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto find(int u, NodeProperty<int>& parent, boost::mpi::communicator world );
void merge(Graph& g, int u, int v, NodeProperty<int>& parent, 
  NodeProperty<int>& rnk, boost::mpi::communicator world );
auto qi_seq(Container<int>& degree, Container<int>& edges, Container<int>& idxs, int n, 
  NodeProperty<int>& parent, NodeProperty<int>& rnk, boost::mpi::communicator world );
auto quicksi(Container<int>& degree, Container<Container<int>>& records, Graph& g, int d, 
  NodeProperty<int>& H, NodeProperty<int>& F, boost::mpi::communicator world );

#endif
