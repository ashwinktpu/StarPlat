#ifndef GENCPP_SSSP_H
#define GENCPP_SSSP_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<omp.h>
#include"graph.hpp"
#include"gm_spinlock/gm_lock.h"

void Compute_SSSP(graph g,int* weight,int src);


#endif
