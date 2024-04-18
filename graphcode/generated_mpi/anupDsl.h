#ifndef GENCPP_ANUPDSL_H
#define GENCPP_ANUPDSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world );

#endif