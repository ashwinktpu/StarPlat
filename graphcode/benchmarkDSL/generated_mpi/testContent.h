#ifndef GENCPP_TESTCONTENT_H
#define GENCPP_TESTCONTENT_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

void testContent(int temp, boost::mpi::communicator world );

#endif
