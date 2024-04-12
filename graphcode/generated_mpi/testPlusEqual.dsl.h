#ifndef GENCPP_TESTPLUSEQUAL.DSL_H
#define GENCPP_TESTPLUSEQUAL.DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void test(int a, int b, NodeProperty<int>& someProperty, boost::mpi::communicator world );

#endif
