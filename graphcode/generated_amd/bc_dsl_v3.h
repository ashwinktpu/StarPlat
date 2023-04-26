#ifndef GENCPP_BC_DSL_V3_H
#define GENCPP_BC_DSL_V3_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void Compute_BC(graph& g,double * BC,std::set<int>& sourceSet);

#endif
