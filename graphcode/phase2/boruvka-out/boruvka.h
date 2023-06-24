#ifndef GENCPP_BORUVKA_H
#define GENCPP_BORUVKA_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void Compute_SSSP(graph& g,int * dist,int src);

#endif
