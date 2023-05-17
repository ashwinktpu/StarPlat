#ifndef GENCPP_SCC_V3_H
#define GENCPP_SCC_V3_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void scc(graph& g);
auto dfs(int u,bool* vis,int* tim,int* low,
  graph& g,int cur_t);
void dfs1(int u,graph& g,bool* vis);

#endif
