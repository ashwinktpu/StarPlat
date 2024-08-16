#include<CL/sycl.hpp>
#include<iostream>
#include<fstream>
#include<set>
#include "graph.hpp"

using namespace sycl;
using namespace std;

 const int NUM_THREADS = 1048576;
 const int STRIDE = NUM_THREADS;


void memoryCopy(queue &Q, bool *to, bool *from, int memSize) {
    Q.submit([&](handler &h) {
        h.memcpy(to, from, memSize*sizeof(bool));
    }).wait();
}

void memoryCopy(queue &Q, int *to, int *from, int memSize) {
    Q.submit([&](handler &h) {
        h.memcpy(to, from, memSize*sizeof(int));
    }).wait();
}

void init(queue &Q, bool *arr, bool val, int arrLen) {
    if(arrLen == 1) {
        Q.submit([&](handler &h) {
            h.single_task([=]() {
                arr[0] = val;
            });
        }).wait();
    } else {
        Q.submit([&](handler &h) {
            h.parallel_for(NUM_THREADS, [=](id<1> i) {
                for(int v = i; v < arrLen; v += STRIDE) {
                    arr[v] = val;
                }
            });
        }).wait();
    }
}

void init(queue &Q, int *arr, int val, int arrLen) {
    if(arrLen == 1) {
        Q.submit([&](handler &h) {
            h.single_task([=]() {
                arr[0] = val;
            });
        }).wait();
    } else {
        Q.submit([&](handler &h) {
            h.parallel_for(NUM_THREADS, [=](id<1> i) {
                for(int v = i; v < arrLen; v += STRIDE) {
                    arr[v] = val;
                }
            });
        }).wait();
    }
}

int* hostMemAllocateInteger(int len) {
    return (int *)malloc((len)*sizeof(int));
}

bool* hostMemAllocateBool(int len) {
    return (bool *)malloc((len)*sizeof(bool));
}

int* deviceMemAllocateInteger(queue &Q, int len) {
    return malloc_device<int>(len, Q);
}

bool* deviceMemAllocateBool(queue &Q, int len) {
    return malloc_device<bool>(len, Q);
}



//-------------------------------------- Kernels ---------------------------------------------------//
void colorGraph_Kernel1(queue &Q, int V, int *d_result, int iteration, int *d_rowPtr, int *d_colIndices, bool *d_allVerticesColored) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> v) {
            for(; v < V; v += STRIDE) {

                if(d_result[v] != -1) {
                    continue;
                }

                bool colorMax = true;
                bool colorMin = true;

                int color = 2 * iteration;

                for(int nbr = d_rowPtr[v]; nbr < d_rowPtr[v + 1]; ++nbr) {
                    int u = d_colIndices[nbr];

                    if(d_result[u] != -1 && d_result[u] != color + 1 && d_result[u] != color + 2) continue;

                    if(v < u) colorMax = false;
                    if(u < v) colorMin = false;
                }

                if(colorMax) d_result[v] = color + 1;
                if(colorMin) d_result[v] = color + 2;

                if(d_result[v] == -1) {
                    d_allVerticesColored[0] = false;
                }
            }
        });
    }).wait();
}


void colorGraph_Kernel2(queue &Q, int V, int *d_rowPtr, int *d_colIndices, int *d_result, bool *d_allVerticesColored) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> v) {
            for(; v < V; v += STRIDE) {
                for(int nbr = d_rowPtr[v]; nbr < d_rowPtr[v + 1]; ++nbr) {
                    int u = d_colIndices[nbr];

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
        });
    }).wait();
}


//---------------------------------------- Algorithm ------------------------------------------------//
void colorGraph(graph &g) {
    queue Q(default_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;

    int V = g.num_nodes();
    int E = g.num_edges();

    printf("Number of nodes:%d\n", V);
    printf("Number of edges:%d\n", E);
    int *edgeLen = g.getEdgeLen();

    int *h_rowPtr;
    int *h_colIndices;
    int *h_revrowPtr;
    int *h_revcolIndices;
    int *h_edgeweight;
    int *h_result;

    h_rowPtr = hostMemAllocateInteger(V + 1);
    h_colIndices = hostMemAllocateInteger(E);
    h_revrowPtr = hostMemAllocateInteger(V + 1);
    h_revcolIndices = hostMemAllocateInteger(E);
    h_edgeweight = hostMemAllocateInteger(E);
    h_result = hostMemAllocateInteger(V);

    for (int i = 0; i <= V; i++){   
        h_rowPtr[i] = g.indexofNodes[i];
        h_revrowPtr[i] = g.rev_indexofNodes[i];
    }
    
    
    for (int i = 0; i < E; i++){
        h_colIndices[i] = g.edgeList[i];
        h_revcolIndices[i] = g.srcList[i];
        h_edgeweight[i] = edgeLen[i]; 
    }

    int *d_rowPtr;
    int *d_colIndices;
    int *d_revrowPtr;
    int *d_revcolIndices;
    int *d_edgeweight;
    int *d_level;
    bool *d_modified;
    bool *d_modified_next;
    int *d_result;
    bool *d_available;

    d_rowPtr = deviceMemAllocateInteger(Q, V + 1);
    d_colIndices = deviceMemAllocateInteger(Q, E);
    d_revrowPtr = deviceMemAllocateInteger(Q, V + 1);
    d_revcolIndices = deviceMemAllocateInteger(Q, E);
    d_edgeweight = deviceMemAllocateInteger(Q, E);
    d_level = deviceMemAllocateInteger(Q, V);
    d_modified_next = deviceMemAllocateBool(Q, V);
    d_result = deviceMemAllocateInteger(Q, V);
    d_available = deviceMemAllocateBool(Q, V);

    memoryCopy(Q, d_rowPtr, h_rowPtr, V + 1);
    memoryCopy(Q, d_colIndices, h_colIndices, E);
    memoryCopy(Q, d_revrowPtr, h_revrowPtr, V + 1);
    memoryCopy(Q, d_revcolIndices, h_colIndices, E);
    memoryCopy(Q, d_edgeweight, h_edgeweight, E);

    // TIMER START
    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

    init(Q, d_level, (int)INT_MAX, V);
    init(Q, d_result, (int)-1, V);
    init(Q, d_available, (bool)false, V);
    init(Q, d_result, (int)0, 1);
    init(Q, d_modified_next, (bool)false, V);

    bool *h_vertices = hostMemAllocateBool(V + 1);
    bool *h_allVerticesColored = hostMemAllocateBool(1);

    bool *d_vertices = deviceMemAllocateBool(Q, V + 1);
    bool *d_set = deviceMemAllocateBool(Q, V + 1);
    bool *d_allVerticesColored = deviceMemAllocateBool(Q, 1);

    init(Q, d_vertices, (bool)false, V);
    memoryCopy(Q, h_vertices, d_vertices, V + 1);

    int iteration = 0;

    h_allVerticesColored[0] = false;

    while(!h_allVerticesColored[0]) {
        init(Q, d_allVerticesColored, true, 1);

        colorGraph_Kernel1(Q, V, d_result, iteration, d_rowPtr, d_colIndices, d_allVerticesColored);
        colorGraph_Kernel2(Q, V, d_rowPtr, d_colIndices, d_result, d_allVerticesColored);
        memoryCopy(Q, h_allVerticesColored, d_allVerticesColored, 1);
        iteration++;
    }

    // std:: cout << iteration << std:: endl;
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();

    std:: set<int> st;

    memoryCopy(Q, h_result, d_result, V);

    for(int i = 0; i < V; i++) {
        st.insert(h_result[i]);
    }


   std:: cout << "Minimum Number of colors needed : " << st.size() << std::endl;
   std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << "   "<<std::endl;
}

int main(int argc, char** argv)
{   
    char* inp = argv[1];
    graph G1(inp);
    G1.parseGraph();
    colorGraph(G1);
    return 0;
}