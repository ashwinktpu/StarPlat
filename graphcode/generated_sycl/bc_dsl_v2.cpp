#ifndef GENCPP_BC_DSL_V2_H
#define GENCPP_BC_DSL_V2_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void Compute_BC(graph &g, float *BC, std::set<int> &sourceSet)
{
  queue Q(default_selector_v);
  std::cout << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n", V);
  printf("#edges:%d\n", E);
  int *edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc((V + 1) * sizeof(int));
  h_data = (int *)malloc((E) * sizeof(int));
  h_src = (int *)malloc((E) * sizeof(int));
  h_weight = (int *)malloc((E) * sizeof(int));
  h_rev_meta = (int *)malloc((V + 1) * sizeof(int));

  for (int i = 0; i <= V; i++)
  {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for (int i = 0; i < E; i++)
  {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }

  int *d_meta;
  int *d_data;
  int *d_src;
  int *d_weight;
  int *d_rev_meta;
  bool *d_modified_next;

  d_meta = malloc_device<int>((1 + V), Q);
  d_data = malloc_device<int>((E), Q);
  d_src = malloc_device<int>((E), Q);
  d_weight = malloc_device<int>((E), Q);
  d_rev_meta = malloc_device<int>((V + 1), Q);
  d_modified_next = malloc_device<bool>((V), Q);

  Q.submit([&](handler &h)
           { h.memcpy(d_meta, h_meta, sizeof(int) * (V + 1)); })
      .wait();
  Q.submit([&](handler &h)
           { h.memcpy(d_data, h_data, sizeof(int) * (E)); })
      .wait();
  Q.submit([&](handler &h)
           { h.memcpy(d_src, h_src, sizeof(int) * (E)); })
      .wait();
  Q.submit([&](handler &h)
           { h.memcpy(d_weight, h_weight, sizeof(int) * (E)); })
      .wait();
  Q.submit([&](handler &h)
           { h.memcpy(d_rev_meta, h_rev_meta, sizeof(int) * ((V + 1))); })
      .wait();

  // CSR END
  // LAUNCH CONFIG
  int NUM_THREADS = 1048576;
  int stride = NUM_THREADS;

  // TODO: TIMER START

  // DECLAR DEVICE AND HOST vars in params
  float *d_BC;
  d_BC = malloc_device<float>(V, Q);

  // BEGIN DSL PARSING
  Q.submit([&](handler &h)
           { h.parallel_for(NUM_THREADS, [=](id<1> i)
                            {
    for (; i < V; i += stride) d_BC[i] = (float)0; }); })
      .wait();

  // Generate for all statement
  // for all else part
  // FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for (itr = sourceSet.begin(); itr != sourceSet.end(); itr++)
  {
    int src = *itr;
    double *d_sigma;
    d_sigma = malloc_device<double>(V, Q);

    float *d_delta;
    d_delta = malloc_device<float>(V, Q);

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_delta[i] = (float)0; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
  for (; i < V; i += stride) d_sigma[i] = (double)0; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_sigma[src] = (double)1; }); })
        .wait(); // InitIndexDevice

    // EXTRA vars for ITBFS AND REVBFS
    bool finished;
    int hops_from_source = 0;
    bool *d_finished;
    d_finished = malloc_device<bool>(1, Q);
    int *d_hops_from_source;
    d_hops_from_source = malloc_device<int>(1, Q);
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { *d_hops_from_source = 0; }); })
        .wait();
    int *d_level;
    d_level = malloc_device<int>(V, Q);

    // EXTRA vars INITIALIZATION
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
for (; i < V; i += stride) d_level[i] = (int)-1; }); })
        .wait();
    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_level[src] = 0; }); })
        .wait();

    // long k =0 ;// For DEBUG
    do
    {
      finished = true;
      Q.submit([&](handler &h)
               { h.memcpy(d_finished, &finished, sizeof(bool) * (1)); })
          .wait();

      // Kernel LAUNCH
      Q.submit([&](handler &h)
               { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                {for (; v < V; v += NUM_THREADS){
if(d_level[v] == *d_hops_from_source) {
// Generate for all statement
// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
int w = d_data[edge];
if(d_level[w] == -1) {
  d_level[w] = *d_hops_from_source + 1;
  *d_finished = false;
}
if(d_level[w] == *d_hops_from_source + 1) {
  d_sigma[w] = d_sigma[w] + d_sigma[v];

}
}
} // end if d lvl
} }); })
          .wait(); // kernel end

      Q.submit([&](handler &h)
               { h.single_task([=]()
                               { *d_hops_from_source += 1; }); })
          .wait();
      ++hops_from_source; // updating the level to process in the next iteration

      Q.submit([&](handler &h)
               { h.memcpy(&finished, d_finished, sizeof(bool) * (1)); })
          .wait();
    } while (!finished);

    hops_from_source--;
    Q.submit([&](handler &h)
             { h.memcpy(d_hops_from_source, &hops_from_source, sizeof(int) * (1)); })
        .wait();

    // BACKWARD PASS
    while (hops_from_source > 1)
    {

      // KERNEL Launch

      Q.submit([&](handler &h)
               { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                {for (; v < V; v += NUM_THREADS){
if(d_level[v] == *d_hops_from_source-1) {
// Generate for all statement
// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
int w = d_data[edge];
if(d_level[w] == *d_hops_from_source) {
d_delta[v] = d_delta[v] + (d_sigma[v] / d_sigma[w]) * (1 + d_delta[w]);

} // end IF  
} // end FOR
d_BC[v] = d_BC[v] + d_delta[v];
} // end if d lvl
 } }); })
          .wait(); // kernel end

      hops_from_source--;
      Q.submit([&](handler &h)
               { h.memcpy(d_hops_from_source, &hops_from_source, sizeof(int) * (1)); })
          .wait();
    }

    // cudaFree up!! all propVars in this BLOCK!
    free(d_delta, Q);
    free(d_sigma, Q);
  }

  Q.submit([&](handler &h)
           { h.memcpy(BC, d_BC, sizeof(float) * (V)); })
      .wait();
  for (int i = 0; i < V; i++)
    std::cout << i << " " << BC[i] / 2.0 << std::endl;

} // end FUN

int main(int argc, char **argv)
{
  graph G1(argv[1]);
  G1.parseGraph();
  std::set<int> sourceSet;
  for (int i = 0; i < 9; i++)
    sourceSet.insert(i);
  float *BC;
  BC = (float *)malloc(G1.num_nodes() * sizeof(float));
  Compute_BC(G1, BC, sourceSet);
  return 0;
}
#endif