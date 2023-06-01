#ifndef GENCPP_PAGERANKDSLV2_H
#define GENCPP_PAGERANKDSLV2_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void Compute_PR(graph &g, float beta, float delta, int maxIter,
                float *pageRank)
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

  // TIMER START
  std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

  // DECLAR DEVICE AND HOST vars in params
  float *d_pageRank;
  d_pageRank = malloc_device<float>(V, Q);

  // BEGIN DSL PARSING
  float num_nodes = (float)g.num_nodes(); // asst in main

  float *d_pageRank_nxt;
  d_pageRank_nxt = malloc_device<float>(V, Q);

  Q.submit([&](handler &h)
           { h.parallel_for(NUM_THREADS, [=](id<1> i)
                            {
    for (; i < V; i += stride) d_pageRank[i] = (float)1 / num_nodes; }); })
      .wait();

  Q.submit([&](handler &h)
           { h.parallel_for(NUM_THREADS, [=](id<1> i)
                            {
  for (; i < V; i += stride) d_pageRank_nxt[i] = (float)0; }); })
      .wait();

  int iterCount = 0; // asst in main

  float diff; // asst in main

  do
  {
    diff = 0.000000;
    float *d_diff = malloc_device<float>(1, Q);
    Q.submit([&](handler &h)
             { h.memcpy(d_diff, &diff, 1 * sizeof(float)); })
        .wait();

    float *d_delta = malloc_device<float>(1, Q);
    Q.submit([&](handler &h)
             { h.memcpy(d_delta, &delta, 1 * sizeof(float)); })
        .wait();

    float *d_num_nodes = malloc_device<float>(1, Q);
    Q.submit([&](handler &h)
             { h.memcpy(d_num_nodes, &num_nodes, 1 * sizeof(float)); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> v)
                              {for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
  float sum = 0.000000; // asst in main

  // for all else part
  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {int nbr = d_src[edge] ;
    sum = sum + d_pageRank[nbr] / (d_meta[nbr+1]-d_meta[nbr]);

  } //  end FOR NBR ITR. TMP FIX!
  float val = (1 - delta) / num_nodes + delta * sum; // asst in main

  atomic_ref<float, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_diff[0]);
  atomic_data += val - d_pageRank[v];
  d_pageRank_nxt[v] = val;
} }); })
        .wait(); // end KER FUNC

    Q.submit([&](handler &h)
             { h.memcpy(&diff, d_diff, 1 * sizeof(float)); })
        .wait();

    Q.submit([&](handler &h)
             { h.memcpy(&delta, d_delta, 1 * sizeof(float)); })
        .wait();

    Q.submit([&](handler &h)
             { h.memcpy(&num_nodes, d_num_nodes, 1 * sizeof(float)); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
for (; i < V; i += stride) d_pageRank[i] = d_pageRank_nxt[i]; }); })
        .wait();
    iterCount++;

  } while ((diff > beta) && (iterCount < maxIter));

  // free up!! all propVars in this BLOCK!
  free(d_pageRank_nxt, Q);

  // TIMER STOP
  std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
  std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

  Q.submit([&](handler &h)
           { h.memcpy(pageRank, d_pageRank, sizeof(float) * (V)); })
      .wait();
} // end FUN

#endif
