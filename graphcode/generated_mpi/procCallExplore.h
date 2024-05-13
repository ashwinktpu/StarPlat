#ifndef GENCPP_PROCCALLEXPLORE_H
#define GENCPP_PROCCALLEXPLORE_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void hell(int a, NodeProperty<int>& excess, boost::mpi::communicator world );

#endif
