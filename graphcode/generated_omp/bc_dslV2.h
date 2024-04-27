#ifndef GENCPP_BC_DSLV2_H
#define GENCPP_BC_DSLV2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void Compute_BC(graph& g , float* BC , std::set<int>& sourceSet);

#endif
