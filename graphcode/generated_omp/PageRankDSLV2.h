#ifndef GENCPP_PAGERANKDSLV2_H
#define GENCPP_PAGERANKDSLV2_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank);

#endif
