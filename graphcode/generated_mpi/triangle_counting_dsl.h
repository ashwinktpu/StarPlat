#ifndef GENCPP_TRIANGLE_COUNTING_DSL_H
#define GENCPP_TRIANGLE_COUNTING_DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto Compute_TC(Graph& g, boost::mpi::communicator world );

#endif
