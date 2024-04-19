#ifndef GENCPP_TRYOMP.DSL_H
#define GENCPP_TRYOMP.DSL_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

auto find(int u , int* parent);
void merge(int u , int v , int* parent , int* rnk
);
auto qi_seq(std::vector<int> degree , std::vector<int> edges , int n , int* parent , 
  int* rnk);
auto quicksi(std::vector<int> degree , std::vector<std::vector<int>> records , graph& g , int d , 
  int* H , int* F);

#endif
