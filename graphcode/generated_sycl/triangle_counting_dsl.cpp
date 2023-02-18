#ifndef GENCPP_TRIANGLE_COUNTING_DSL_H
#define GENCPP_TRIANGLE_COUNTING_DSL_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl

void Compute_TC(graph& g)
{
  default_selector d_selector
  queue Q(d_selector)
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;
  bool* d_modified_next;

  d_meta=malloc_device<int>((1+V), Q);
  d_data=malloc_device<int>((E), Q);
  d_src=malloc_device<int>((E), Q);
  d_weight=malloc_device<int>((E), Q);
  d_rev_meta=malloc_device<int>((V+1), Q);
  d_modified_next=malloc_device<bool>((V), Q);


  #endif
