#ifndef GENCPP_GALLOISDSL_H
#define GENCPP_GALLOISDSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../exp_mpi_header/graph_mpi.h"

int  maxFlow(Graph& g, int source, int sink, boost::mpi::communicator world );

#endif
