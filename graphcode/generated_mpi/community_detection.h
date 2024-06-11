#ifndef GENCPP_COMMUNITY_DETECTION_H
#define GENCPP_COMMUNITY_DETECTION_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto Compute_CD(Graph& g, EdgeProperty<int>& weight, boost::mpi::communicator world );

#endif
