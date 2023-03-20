#ifndef GENCPP_SSSP_DSLV3_H
#define GENCPP_SSSP_DSLV3_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void Compute_SSSP(Graph& g, NodeProperty<int>& dist, EdgeProperty<int>& weight, int src
  , boost::mpi::communicator world );

#endif
