#ifndef GENCPP_EDMONDS-KARP_H
#define GENCPP_EDMONDS-KARP_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void ek(graph& g , int s , int d , int* weight
);

#endif
