#ifndef GENCPP_DYNAMICBATCHTCV2_H
#define GENCPP_DYNAMICBATCHTCV2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

#include"../mpi_header/updates.h"

auto staticTC(Graph& g, boost::mpi::communicator world );
auto dynamicBatchTCV2_add(Graph& g, int triangle_countSent, EdgeProperty<bool>& modified, Updates * addBatch
  , boost::mpi::communicator world );
auto dynamicBatchTCV2_del(Graph& g, int triangle_countSent, EdgeProperty<bool>& modified, Updates * deleteBatch
  , boost::mpi::communicator world );
void DynTC(Graph& g, Updates * updateBatch, int batchSize, boost::mpi::communicator world );

#endif
