#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <CL/sycl.hpp>
#include "utils.h"

using namespace sycl;

#define ATOMIC_INT atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space>
#define ATOMIC_FLOAT atomic_ref<float, memory_order::relaxed, memory_scope::device, access::address_space::global_space>
#define get_node(j) g->V[j]
#define get_neighbour(j) g->E[j]
#define get_parent(j) g->RE[j]
#define get_weight(j) g->W[j]
#define get_num_neighbours(j) g->I[j + 1] - g->I[j]
#define begin_neighbours(u) g->I[u]
#define end_neighbours(u) g->I[u + 1]
#define begin_parents(u) g->RI[u]
#define end_parents(u) g->RI[u + 1]
#define for_neighbours(u, j) for (j = begin_neighbours(u); j < end_neighbours(u); j++)
#define for_parents(u, j) for (j = begin_parents(u); j < end_parents(u); j++)

#define forall(N, NUM_THREADS) Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> u){for (; u < N; u += NUM_THREADS)
#define end \
    });     \
    }).wait();

class graph
{
private:
    int32_t num_nodes;
    int32_t num_edges;
    std::string gname;

public:
    int *V, *I, *E, *W, *RE, *RI;

    int get_num_nodes()
    {
        return num_nodes;
    }

    int get_num_edges()
    {
        return num_edges;
    }

    std::string get_graph_name()
    {
        return gname;
    }

    void set_num_nodes(int n)
    {
        num_nodes = n;
    }

    void set_num_edges(int n)
    {
        num_edges = n;
    }

    void set_graph_name(std::string name)
    {
        gname = name;
    }

    void load_graph(std::string name, queue Q)
    {
        std::vector<int> h_V, h_I, h_E, h_W, h_RE, h_RI;

        load_from_file("csr_graphs/" + name + "/V", h_V);
        load_from_file("csr_graphs/" + name + "/I", h_I);
        load_from_file("csr_graphs/" + name + "/E", h_E);
        load_from_file("csr_graphs/" + name + "/W", h_W);
        load_from_file("csr_graphs/" + name + "/RE", h_RE);
        load_from_file("csr_graphs/" + name + "/RI", h_RI);

        int num_nodes = h_V.size();
        int num_edges = h_E.size();

        set_num_edges(num_edges);
        set_num_nodes(num_nodes);
        set_graph_name(name);
        V = malloc_device<int>(num_nodes, Q);
        I = malloc_device<int>((num_nodes + 1), Q);
        E = malloc_device<int>(num_edges, Q);
        W = malloc_device<int>(num_edges, Q);
        RE = malloc_device<int>(num_edges, Q);
        RI = malloc_device<int>((num_nodes + 1), Q);

        Q.submit([&](handler &h)
                 { h.memcpy(V, &h_V[0], h_V.size() * sizeof(int)); });

        Q.submit([&](handler &h)
                 { h.memcpy(I, &h_I[0], h_I.size() * sizeof(int)); });

        Q.submit([&](handler &h)
                 { h.memcpy(E, &h_E[0], h_E.size() * sizeof(int)); });

        Q.submit([&](handler &h)
                 { h.memcpy(W, &h_W[0], h_W.size() * sizeof(int)); });

        Q.submit([&](handler &h)
                 { h.memcpy(RE, &h_RE[0], h_RE.size() * sizeof(int)); });

        Q.submit([&](handler &h)
                 { h.memcpy(RI, &h_RI[0], h_RI.size() * sizeof(int)); });

        Q.wait();
    }

    void free_memory(queue Q)
    {
        free(V, Q);
        free(I, Q);
        free(E, Q);
        free(W, Q);
        free(RE, Q);
        free(RI, Q);
    }
};

// initialize device arr with val, if needed set arr[pos] = pos_val
template <typename T>
void initialize(T *arr, T val, int NUM_THREADS, int N, queue Q, int pos = -1, T pos_val = -1)
{
    int stride = NUM_THREADS;
    Q.submit([&](handler &h)
             { h.parallel_for(NUM_THREADS, [=](id<1> i)
                              {
                                  for (; i < N; i += stride)
                                  {
                                      arr[i] = val;
    
                                      if (i == pos)
                                      {
                                          arr[pos] = pos_val;
                                      }
                                  } }); });
    Q.wait();
}

// memcpy from src to dest
template <typename T>
void memcpy(T *dest, T *src, int N, queue Q)
{
    Q.submit([&](handler &h)
             { h.memcpy(dest, src, N * sizeof(T)); })
        .wait();
}

// copy contents of src to dest, both src and dest are of length N
template <typename T>
void copy(T *dest, T *src, int NUM_THREADS, int N, queue Q)
{
    int stride = NUM_THREADS;
    Q.submit([&](handler &h)
             { h.parallel_for(
                   NUM_THREADS, [=](id<1> i)
                   {
                               for (; i < N ;i += stride)
                               {
                                    dest[i] = src[i];
                               } }); })
        .wait();
}

// returns True if u and v are neighbours
int neighbours(int v, int w, graph *g)
{
    int connected = 0;
    int start_edge = begin_neighbours(v);
    int end_edge = end_neighbours(v) - 1;

    if (get_neighbour(start_edge) == w)
    {
        connected = 1;
    }
    else if (get_neighbour(end_edge) == w)
    {
        connected = 1;
    }
    else
    {
        int mid = start_edge + (end_edge - start_edge) / 2;
        while (start_edge <= end_edge)
        {
            if (get_neighbour(mid) == w)
            {
                connected = 1;
                break;
            }
            if (w < get_neighbour(mid))
            {
                end_edge = mid - 1;
            }
            else
            {
                start_edge = mid + 1;
            }
            mid = start_edge + (end_edge - start_edge) / 2;
        }
    }

    return connected;
}