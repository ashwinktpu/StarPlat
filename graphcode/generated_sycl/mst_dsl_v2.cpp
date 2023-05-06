#ifndef GENCPP_MST_DSL_V2_H
#define GENCPP_MST_DSL_V2_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void Boruvka(graph &g)
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

    // BEGIN DSL PARSING
    int *d_nodeId;
    d_nodeId = malloc_device<int>(V, Q);

    int *d_color;
    d_color = malloc_device<int>(V, Q);

    bool *d_isMSTEdge;
    d_isMSTEdge = malloc_device<bool>(E, Q);

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
    for (; i < V; i += stride) d_nodeId[i] = (int)-1; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
  for (; i < V; i += stride) d_color[i] = (int)-1; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
for (; i < E; i += stride) d_isMSTEdge[i] = (bool)false; }); })
        .wait();

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> u)
                              {for (; u < V; u += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
d_nodeId[u] = d_nodeId[u]+ u + 1;
d_color[u] = d_color[u]+ u + 1;
} }); })
        .wait(); // end KER FUNC

    bool *d_modified;
    d_modified = malloc_device<bool>(V, Q);

    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
for (; i < V; i += stride) d_modified[i] = (bool)false; }); })
        .wait();

    bool noNewComp = false; // asst in main

    // FIXED POINT variables
    // BEGIN FIXED POINT
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
for (; i < V; i += stride) d_modified_next[i] = false; }); })
        .wait();

    while (!noNewComp)
    {

        noNewComp = true;
        bool *d_noNewComp = malloc_device<bool>(1, Q);
        Q.submit([&](handler &h)
                 { h.memcpy(d_noNewComp, &noNewComp, 1 * sizeof(bool)); })
            .wait();

        int *d_minEdge;
        d_minEdge = malloc_device<int>(V, Q);

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> i)
                                  {
for (; i < V; i += stride) d_minEdge[i] = (int)-1; }); })
            .wait();

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> src)
                                  {for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
// for all else part
for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
int dst = d_data[edge];
if (d_color[src] != d_color[dst]){ // if filter begin 
int e = edge;
int minEdge = d_minEdge[src];
if (minEdge == -1){ // if filter begin 
d_minEdge[src] = e;

} // if filter end
if (minEdge != -1){ // if filter begin 
int minDst = d_data[minEdge];
if (d_weight[e] < d_weight[minEdge] || (d_weight[e] == d_weight[minEdge] && d_color[dst] < d_color[minDst])){ // if filter begin 
d_minEdge[src] = e;

} // if filter end

} // if filter end

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
} }); })
            .wait(); // end KER FUNC

        int *d_minEdgeOfComp;
        d_minEdgeOfComp = malloc_device<int>(V, Q);

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> i)
                                  {
for (; i < V; i += stride) d_minEdgeOfComp[i] = (int)-1; }); })
            .wait();

        bool finishedMinEdge = false; // asst in main

        // FIXED POINT variables
        // BEGIN FIXED POINT
        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> i)
                                  {
for (; i < V; i += stride) d_modified_next[i] = false; }); })
            .wait();

        while (!finishedMinEdge)
        {

            finishedMinEdge = true;
            bool *d_finishedMinEdge = malloc_device<bool>(1, Q);
            Q.submit([&](handler &h)
                     { h.memcpy(d_finishedMinEdge, &finishedMinEdge, 1 * sizeof(bool)); })
                .wait();

            Q.submit([&](handler &h)
                     { h.parallel_for(NUM_THREADS, [=](id<1> u)
                                      {for (; u < V; u += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
int comp = d_color[u]; // asst in main

int minEdge = d_minEdgeOfComp[comp]; // asst in main

int e = d_minEdge[u]; // asst in main

if (e != -1){ // if filter begin 
int dst = d_data[e]; // asst in main

if (minEdge == -1){ // if filter begin 
d_minEdgeOfComp[comp] = e;
*d_finishedMinEdge = false;

} // if filter end
if (minEdge != -1){ // if filter begin 
int minDst = d_data[minEdge];
if (d_weight[e] < d_weight[minEdge] || (d_weight[e] == d_weight[minEdge] && d_color[dst] < d_color[minDst])){ // if filter begin 
d_minEdgeOfComp[comp] = e;
*d_finishedMinEdge = false;

} // if filter end

} // if filter end

} // if filter end
} }); })
                .wait(); // end KER FUNC

            Q.submit([&](handler &h)
                     { h.memcpy(&finishedMinEdge, d_finishedMinEdge, 1 * sizeof(bool)); })
                .wait();

            Q.submit([&](handler &h)
                     { h.memcpy(&finishedMinEdge, d_finishedMinEdge, 1 * sizeof(bool)); })
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

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> src)
                                  {for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_color[src] == d_nodeId[src]){ // if filter begin 
int srcMinEdge = d_minEdgeOfComp[src];
if (srcMinEdge != -1){ // if filter begin 
int dst = d_data[srcMinEdge];
int dstLead = d_color[dst];
int dstMinEdge = d_minEdgeOfComp[dstLead];
if (dstMinEdge != -1){ // if filter begin 
int dstOfDst = d_data[dstMinEdge];
int dstOfDstLead = d_color[dstOfDst];
if (d_color[src] == d_color[dstOfDstLead] && d_color[src] > d_color[dstLead]){ // if filter begin 
d_minEdgeOfComp[dstLead] = -1;

} // if filter end

} // if filter end

} // if filter end

} // if filter end
} }); })
            .wait(); // end KER FUNC

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> src)
                                  {for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_color[src] == d_nodeId[src]){ // if filter begin 
int srcMinEdge = d_minEdgeOfComp[src];
if (srcMinEdge != -1){ // if filter begin 
d_isMSTEdge[srcMinEdge] = true;

} // if filter end

} // if filter end
} }); })
            .wait(); // end KER FUNC

        Q.submit([&](handler &h)
                 { h.parallel_for(NUM_THREADS, [=](id<1> src)
                                  {for (; src < V; src += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_color[src] == d_nodeId[src]){ // if filter begin 
int srcMinEdge = d_minEdgeOfComp[src];
if (srcMinEdge != -1){ // if filter begin 
*d_noNewComp = false;
int dst = d_data[srcMinEdge];
d_color[src] = d_color[dst];

} // if filter end

} // if filter end
} }); })
            .wait(); // end KER FUNC

        Q.submit([&](handler &h)
                 { h.memcpy(&noNewComp, d_noNewComp, 1 * sizeof(bool)); })
            .wait();

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
                     { h.parallel_for(NUM_THREADS, [=](id<1> u)
                                      {for (; u < V; u += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
int my_color = d_color[u]; // asst in main

int other_color = d_color[my_color]; // asst in main

if (my_color != other_color){ // if filter begin 
*d_finished = false;
d_color[u] = other_color;

} // if filter end
} }); })
                .wait(); // end KER FUNC

            Q.submit([&](handler &h)
                     { h.memcpy(&finished, d_finished, 1 * sizeof(bool)); })
                .wait();

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
        free(d_minEdgeOfComp, Q);
        free(d_minEdge, Q);

        Q.submit([&](handler &h)
                 { h.memcpy(&noNewComp, d_noNewComp, 1 * sizeof(bool)); })
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

    bool *h_isMSTEdge;
    h_isMSTEdge = (bool *)malloc((E) * sizeof(bool));
    Q.submit([&](handler &h)
             { h.memcpy(h_isMSTEdge, d_isMSTEdge, sizeof(bool) * (E)); })
        .wait();

    long long mst = 0;
    for (int i = 0; i < E; i++)
    {
        if (h_isMSTEdge[i] == true)
            mst += h_weight[i];
    }
    printf("MST Weight: %lld\n", mst);

    // cudaFree up!! all propVars in this BLOCK!
    free(d_isMSTEdge, Q);
    free(d_modified, Q);
    free(d_color, Q);
    free(d_nodeId, Q);

    // TIMER STOP
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

} // end FUN

int main(int argc, char **argv)
{
    char *inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp);
    graph g(inp);
    g.parseGraph();
    Boruvka(g);
    return 0;
}
#endif