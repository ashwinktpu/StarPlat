#ifndef GENCPP_INPUT3_H
#define GENCPP_INPUT3_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void Compute_SSSP(graph& g,int src);

#endif
