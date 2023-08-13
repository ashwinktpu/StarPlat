#ifndef GENCPP_MST_PRIMS_H
#define GENCPP_MST_PRIMS_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void Prims(Graph& g, boost::mpi::communicator world );

#endif
