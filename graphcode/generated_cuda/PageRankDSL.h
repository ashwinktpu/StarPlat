#ifndef GENCPP_PAGERANKDSL_H
#define GENCPP_PAGERANKDSL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank);

float sum = 0.000000;
float val = (1 - delta) / num_nodes + delta * sum;
diff = diff+ val - d_pageRank[v];
d_pageRank[v] = val;

#endif
