#ifndef GENCPP_BP_H
#define GENCPP_BP_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void bp(graph& g , int k , int* weight);

#endif
