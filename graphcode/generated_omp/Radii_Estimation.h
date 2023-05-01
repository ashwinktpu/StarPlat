#ifndef GENCPP_RADII_ESTIMATION_H
#define GENCPP_RADII_ESTIMATION_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void rad(graph& g , int k);

#endif
