#ifndef GENCPP_COLOR_H
#define GENCPP_COLOR_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"
#include "libcuda.cuh"
#include <cooperative_groups.h>
#include <curand.h>
#include<bits/stdc++.h>


void colorGraph(graph& g);

__device__ int fpoint1;

__global__ void compute_colors(int V, int E, int* d_meta, int* d_data, int* d_src,int *d_rev_meta,unsigned int* d_color,bool* d_modified,bool* d_modified_next){
    unsigned v = blockIdx.x*blockDim.x+threadIdx.x;
    if(v>=V) return;
    if(d_modified[v]==false){
        // printf("hi");
        int cnt = 0;
        int total = 0;
        for(int edge = d_meta[v];edge<d_meta[v+1];edge++){
            int nbr = d_data[edge];
            total+=1;
            if(d_modified[nbr]){
                cnt+=1;
            }
            else if(d_color[v]>d_color[nbr]){
                cnt+=1;
            }
        }
        for(int edge = d_rev_meta[v];edge<d_rev_meta[v+1];edge++){
            int nbr = d_src[edge];
            total+=1;
            if(d_modified[nbr]){
                cnt+=1;
            }
            else if(d_color[v]>d_color[nbr]){
                cnt+=1;
            }
        }
        if(cnt==total){
            d_modified_next[v]=true;
            atomicAdd(&fpoint1,1);
        }
    }
}

#endif