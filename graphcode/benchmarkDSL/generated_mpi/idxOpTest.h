#ifndef GENCPP_IDXOPTEST_H
#define GENCPP_IDXOPTEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void indexTest(int a, int b, Container<int>& someArr, boost::mpi::communicator world );

#endif
