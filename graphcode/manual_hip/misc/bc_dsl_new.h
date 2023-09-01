#ifndef GENCPP_BC_DSL_NEW_H
#define GENCPP_BC_DSL_NEW_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void Compute_BC_new(graph& g,double* BC,std::set<int>& sourceSet);

#endif
