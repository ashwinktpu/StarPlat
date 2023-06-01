#ifndef GENCPP_PAGERANKDSL_H
#define GENCPP_PAGERANKDSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include"../graph.hpp"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank);

#endif
