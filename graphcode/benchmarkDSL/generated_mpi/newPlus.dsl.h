#ifndef GENCPP_NEWPLUS.DSL_H
#define GENCPP_NEWPLUS.DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void do_max_flow(Graph& g, int source, int sink, EdgeProperty<int>& residual_capacity
  , boost::mpi::communicator world );

#endif
