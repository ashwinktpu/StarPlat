#ifndef GENCPP_DYNAMICBATCHSSSP_H
#define GENCPP_DYNAMICBATCHSSSP_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

#include"../mpi_header/updates.h"

void staticSSSP(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  int src, boost::mpi::communicator world );
void dynamicBatchSSSP_add(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  NodeProperty<bool>& modified, boost::mpi::communicator world );
void dynamicBatchSSSP_del(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  NodeProperty<bool>& modified, boost::mpi::communicator world );
void DynSSSP(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  Updates * updateBatch, int batchSize, int src, boost::mpi::communicator world );

#endif
