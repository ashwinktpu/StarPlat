#ifndef GENCPP_UNARYASSIGNMENTS_H
#define GENCPP_UNARYASSIGNMENTS_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void unaryAssignment(Graph& g, int tx, boost::mpi::communicator world );

#endif
