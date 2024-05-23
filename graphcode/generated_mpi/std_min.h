#ifndef GENCPP_STD_MIN_H
#define GENCPP_STD_MIN_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void helloWorld(NodeProperty<int>& x, int a, int b, Graph& g
  , boost::mpi::communicator world );

#endif
