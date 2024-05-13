#ifndef GENCPP_TESTVEC_H
#define GENCPP_TESTVEC_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void vec(Graph& g, boost::mpi::communicator world );

#endif
