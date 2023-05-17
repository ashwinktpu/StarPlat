#ifndef GENCPP_SSSP_V2_H
#define GENCPP_SSSP_V2_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void Compute_SSSP(graph &g, int *dist, int src)
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
    int *d_dist;
    d_dist = malloc_device<int>(V, Q);

    // BEGIN DSL PARSING
    bool *d_modified;
    d_modified = malloc_device<bool>(V, Q);

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_dist[i] = (int)INT_MAX; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
  for (; i < V; i += stride) d_modified[i] = (bool)false; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_modified[src] = (bool)true; }); })
        .wait(); // InitIndexDevice

    Q.submit([&](handler &h)
             { h.single_task([=]()
                             { d_dist[src] = (int)0; }); })
        .wait(); // InitIndexDevice

    bool finished = false; // asst in main

    // FIXED POINT variables
    // BEGIN FIXED POINT
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
for (; i < V; i += stride) d_modified_next[i] = false; }); })
        .wait();

    while (!finished)
    {

        finished = true;
        bool *d_finished = malloc_device<bool>(1, Q);
        Q.submit([&](handler &h)
                 { h.memcpy(d_finished, &finished, 1 * sizeof(bool)); })
            .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> v)
                                  {for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_modified[v] == true){ // if filter begin 
// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
int nbr = d_data[edge];
int e = edge;
 int dist_new = d_dist[v] + d_weight[e];
bool modified_new = true;
if(d_dist[v]!= INT_MAX && d_dist[nbr] > dist_new)
{
  atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_dist[nbr]);
  atomic_data.fetch_min(dist_new);
  d_modified_next[nbr] = modified_new;
  *d_finished = false ;
}

} //  end FOR NBR ITR. TMP FIX!

} // if filter end
} }); })
            .wait(); // end KER FUNC

        Q.submit([&](handler &h)
                 { h.memcpy(&finished, d_finished, 1 * sizeof(bool)); })
            .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> i)
                                  {
for (; i < V; i += stride) d_modified[i] = d_modified_next[i]; }); })
            .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> i)
                                  {
for (; i < V; i += stride) d_modified_next[i] = false; }); })
            .wait();

    } // END FIXED POINT

    // free up!! all propVars in this BLOCK!
    free(d_modified, Q);

    // TIMER STOP
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    Q.submit([&](handler &h)
             { h.memcpy(dist, d_dist, sizeof(int) * (V)); })
        .wait();
    for (int i = 0; i < V; i++)
        std::cout << i << " " << dist[i] << std::endl;
    // std::cout << "Number of rounds required for convergence: " << k << std::endl;

} // end FUN

int main(int argc, char **argv)
{
    graph G1(argv[1]);
    G1.parseGraph(true);
    int src = atoi(argv[2]);
    int *dist;
    dist = (int *)malloc(G1.num_nodes() * sizeof(int));
    Compute_SSSP(G1, dist, src);
    return 0;
}
#endif