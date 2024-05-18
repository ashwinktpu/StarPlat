#ifndef GENCPP_SSSP_DSL_PREV_H
#define GENCPP_SSSP_DSL_PREV_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void Compute_SSSP_prev(graph& g,int* dist,int* weight,int src
);

#endif
