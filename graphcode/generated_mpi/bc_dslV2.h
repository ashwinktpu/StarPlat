#ifndef GENCPP_BC_DSLV2_H
#define GENCPP_BC_DSLV2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void Compute_BC(Graph& g, NodeProperty<float>& BC, std::set<int>& sourceSet, boost::mpi::communicator world );

#endif
