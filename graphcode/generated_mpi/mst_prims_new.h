#ifndef GENCPP_MST_PRIMS_NEW_H
#define GENCPP_MST_PRIMS_NEW_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"
#include <iostream>

void Compute_MST_Prims(Graph &g, int start_node, int &num_mst_nodes, long long int &mst_weight, boost::mpi::communicator world);

#endif