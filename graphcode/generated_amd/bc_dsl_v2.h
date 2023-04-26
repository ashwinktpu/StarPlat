#ifndef GENCPP_BC_DSL_V2_H
#define GENCPP_BC_DSL_V2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet);

#endif
