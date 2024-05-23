#ifndef GENCPP_GETVINFA_H
#define GENCPP_GETVINFA_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void testAssignment(Graph& g, int u, int t1, boost::mpi::communicator world );

#endif
