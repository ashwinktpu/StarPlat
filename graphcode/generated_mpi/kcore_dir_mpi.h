#ifndef GENCPP_KCORE_DIR_MPI_H
#define GENCPP_KCORE_DIR_MPI_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto Compute_KC(Graph& g, int k, boost::mpi::communicator world );

#endif
