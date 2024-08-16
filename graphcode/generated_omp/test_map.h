#ifndef GENCPP_TEST_MAP_H
#define GENCPP_TEST_MAP_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include"ParallelHeapOpenMPClass.cpp"
#include"deepak_map_openMP.cpp"
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void Test(graph& g , int src);

#endif
