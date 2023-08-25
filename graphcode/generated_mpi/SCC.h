#ifndef GENCPP_SCC_H
#define GENCPP_SCC_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void scc(Graph& g, boost::mpi::communicator world );
auto dfs(int u, NodeProperty<bool>& vis, NodeProperty<int>& tim, NodeProperty<int>& low, 
  Graph& g, int cur_t, boost::mpi::communicator world );
void dfs1(int u, Graph& g, NodeProperty<bool>& vis, boost::mpi::communicator world );

#endif
