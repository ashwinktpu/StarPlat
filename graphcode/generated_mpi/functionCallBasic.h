#ifndef GENCPP_FUNCTIONCALLBASIC_H
#define GENCPP_FUNCTIONCALLBASIC_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void testFunc(Graph& g, int u, boost::mpi::communicator world );

#endif
