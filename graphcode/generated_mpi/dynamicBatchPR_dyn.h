#ifndef GENCPP_DYNAMICBATCHPR_H
#define GENCPP_DYNAMICBATCHPR_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

#include"../mpi_header/updates.h"

void staticPR(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, boost::mpi::communicator world );
void dynamicBatchPR_add(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, NodeProperty<bool>& modified, boost::mpi::communicator world );
void dynamicBatchPR_del(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, NodeProperty<bool>& modified, boost::mpi::communicator world );
void DynPR(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, Updates & updateBatch, int batchSize, boost::mpi::communicator world );

#endif
