#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#define DEBUG 0
#include "graph.h"

using namespace sycl;

//-------------------------------------- Kernels ---------------------------------------------------//
void colorGraph_Kernel1(graph *g, queue &Q, int V, int *d_result, int iteration, bool *d_allVerticesColored, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(d_result[u] != -1) {
            continue;
        }

        bool colorMax = true;
        bool colorMin = true;

        int color = 2 * iteration;

        int edge;
        for_neighbours(u, edge) {
            int v = get_neighbour(edge);

            if(d_result[v] != -1 && d_result[v] != color + 1 && d_result[v] != color + 2) continue;

            if(u < v) colorMax = false;
            if(v < u) colorMin = false;
        }
        if(colorMax) d_result[u] = color + 1;
        if(colorMin) d_result[u] = color + 2;

        if(d_result[u] == -1) {
            d_allVerticesColored[0] = false;
        }
    }
    end;

}


void colorGraph_Kernel2(graph *g, queue &Q, int V, int *d_result, bool *d_allVerticesColored, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        int edge;
        for_neighbours(u, edge) {
            int v = get_neighbour(edge);

            if(d_result[u] == d_result[v] && (u != v)) {
                if(u < v) {
                    d_result[v] = -1;
                } else {
                    d_result[u] = -1;
                }
                d_allVerticesColored[0] = false;
            }
        }
    }
    end;

}


int main(int argc, char **argv) {
    std::chrono::steady_clock::time_point tic_0 = std::chrono::steady_clock::now();

    std::string name = argv[1];
    int NUM_THREADS = atoi(argv[2]);
    std::string NUM_THREADS_STR = std::to_string(NUM_THREADS);

    queue Q(default_selector_v);
    std::cout << "Selected Device: " << Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory ALlocation Size: " << Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;

    std::cout << "Number of parallel work items: " << NUM_THREADS << std::endl;

    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

    graph *g = malloc_shared<graph>(1, Q);
    g->load_graph(name, Q);
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();

    std::cout << "Time to load data from files: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    int V = g->get_num_nodes();
    int E = g->get_num_edges();
    int STRIDE = NUM_THREADS;

    std::cout << "#Nodes: " << V << std::endl;
    std::cout << "#Edges: " << E << std::endl;

    tic = std::chrono::steady_clock::now();

    int *h_result = (int *)malloc((V)*sizeof(int));
    bool *h_allVerticesColored = (bool *)malloc(sizeof(bool));

    int *d_result = malloc_device<int>(V, Q);
    bool *d_vertices = malloc_device<bool>(V, Q);
    bool *d_allVerticesColored = malloc_device<bool>(1, Q);
    bool *d_set = malloc_device<bool>(V, Q);


    initialize(d_result, (int)-1, NUM_THREADS, V, Q, 0, 0);
    initialize(d_vertices, false, NUM_THREADS, V, Q);

    int iteration = 0;

    h_allVerticesColored[0] = false;

    while(!h_allVerticesColored[0]) {
        initialize(d_allVerticesColored, true, NUM_THREADS, 1, Q);

        colorGraph_Kernel1(g, Q, V, d_result, iteration, d_allVerticesColored, NUM_THREADS, STRIDE);
        colorGraph_Kernel2(g, Q, V, d_result, d_allVerticesColored, NUM_THREADS, STRIDE);

        memcpy(h_allVerticesColored, d_allVerticesColored, 1, Q);
        iteration++;
    }

    toc = std::chrono::steady_clock::now();
    std::cout << "Time to run vertex coloring: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << "   "<<std::endl;

    std:: set<int> st;

    memcpy(h_result, d_result, V, Q);

    for(int i = 0; i < V; i++) {
        st.insert(h_result[i]);
    }

    std::cout << "Minimum Number of colors needed: " << st.size() << std::endl;
}   