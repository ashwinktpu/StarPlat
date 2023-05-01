#ifndef GENCPP_K-CORE_H
#define GENCPP_K-CORE_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void k_core(graph& g , int k);

#endif
