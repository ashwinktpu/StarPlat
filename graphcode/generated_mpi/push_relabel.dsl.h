#ifndef GENCPP_PUSH_RELABEL.DSL_H
#define GENCPP_PUSH_RELABEL.DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void push__(Graph& g, int u, int v, NodeProperty<int>& excess, 
  EdgeProperty<int>& residual_capacity, boost::mpi::communicator world );
void relabel(Graph& g, int u, EdgeProperty<int>& residue, NodeProperty<int>& label
  , boost::mpi::communicator world );
void discharge(Graph& g, int u, NodeProperty<int>& label, NodeProperty<int>& excess, 
  NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, boost::mpi::communicator world );
void do_max_flow(Graph& g, int source, int sink, boost::mpi::communicator world );

#endif
