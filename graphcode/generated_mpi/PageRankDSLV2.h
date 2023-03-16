#ifndef GENCPP_PAGERANKDSLV2_H
#define GENCPP_PAGERANKDSLV2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void Compute_PR(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, boost::mpi::communicator world );

#endif
