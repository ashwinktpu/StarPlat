#ifndef GENCPP_PAGERANK_DSL_H
#define GENCPP_PAGERANK_DSL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float * pageRank);

#endif
