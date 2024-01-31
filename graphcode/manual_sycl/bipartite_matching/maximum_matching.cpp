#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#define DEBUG 0
#include "graph.h"

using namespace sycl;

//--------------------  UTILITY FUNCTIONS :: START ------------------------------//

void memoryCopy(queue &Q, bool *to, bool *from, int memSize, int NUM_THREADS, int STRIDE) {
    Q.submit([&](handler &h) {
        h.memcpy(to, from, memSize*sizeof(bool));
    }).wait();
}

void memoryCopy(queue &Q, int *to, int *from, int memSize, int NUM_THREADS, int STRIDE) {
    Q.submit([&](handler &h) {
        h.memcpy(to, from, memSize*sizeof(int));
    }).wait();
}

void init(queue &Q, bool *arr, bool val, int arrLen, int NUM_THREADS, int STRIDE) {
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

void init(queue &Q, int *arr, int val, int arrLen, int NUM_THREADS, int STRIDE) {
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

//--------------------  UTILITY FUNCTIONS :: END ------------------------------//

//--------------------  KERNELs :: START ------------------------------//

void APFB_kernel_1(queue &Q, int V, int nc, int *d_cmatch, int *d_bfsArray, int *d_L0, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(u < nc) {
            if(d_cmatch[u] == -1) {
                d_bfsArray[u] = d_L0[0];
            }
        }
    }
    end;
}

void APFB_kernel_2(graph *g, queue &Q, int V, int nc, int *d_bfsArray, int *d_bfsLevel,  int *d_rmatch, int *d_NOT_VISITED, bool *d_noNewVertices, int *d_predeccesor, bool *d_noNewPaths, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(u < nc) {
            if(d_bfsArray[u] == d_bfsLevel[0]) {
                int edge;
                for_neighbours(u, edge) {
                    int neigh_row = get_neighbour(edge);
                    if(neigh_row >= nc) {
                        int col_match = d_rmatch[neigh_row];

                        if(col_match > -1) {
                            if(d_bfsArray[col_match] == d_NOT_VISITED[0]) {
                                d_noNewVertices[0] = false;
                                d_bfsArray[col_match] = d_bfsLevel[0] + 1;
                                d_predeccesor[neigh_row] = u;
                            }
                        }

                        if(col_match == -1) {
                            d_rmatch[neigh_row] = -2;
                            d_predeccesor[neigh_row] = u;
                            d_noNewPaths[0] = false;
                        }
                    }
                }
            }
        }
    }
    end;
}

void APFB_kernel_3(queue &Q, int V, int nc, int *d_rmatch, bool *d_compress, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(u >= nc && d_rmatch[u] == -2) {
            d_compress[u] = true;
        }
    }
    end;
}

void APFB_kernel_4(queue &Q, int V, int nc, bool *d_compress, int *d_predeccesor, int *d_cmatch, int *d_rmatch, bool *d_compress_next, bool *d_compressed, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(d_compress[u] == true) {
            if(u >= nc) {
                int matched_col = d_predeccesor[u];
                int matched_row = d_cmatch[matched_col];

                bool isValid = true;

                if(matched_row != -1) {
                    if(d_predeccesor[matched_row] == matched_col) {
                        isValid = false;
                    }
                }

                if(isValid) {
                    d_cmatch[matched_col] = u;
                    d_rmatch[u] = matched_col;
                    if(matched_row != -1) {
                        d_compress_next[matched_row] = true;
                        d_compressed[0] = false;
                    }
                }
            }
        }
    }
    end;
}

void APFB_kernel_5(queue &Q, int V, int nc, bool* d_compress, bool *d_compress_next, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(u >= nc) {
            d_compress[u] = d_compress_next[u];
            d_compress_next[u] = false;
        }
    } end;
}

void APFB_kernel_6(queue &Q, int V, int nc, int *d_rmatch, int *d_cmatch, int NUM_THREADS, int STRIDE) {

    forall(V, NUM_THREADS) {
        if(u >= nc) {
            int c = d_rmatch[u];
            if(c >= 0) {
                if(d_cmatch[c] != u) {
                    d_rmatch[u] = -1;
                }
            }

            if(c == -2) {
                d_rmatch[u] = -1;
            }
        }
    }
    end;
}
//--------------------  KERNELs :: END ------------------------------//



int main(int argc, char **argv) {
     std::chrono::steady_clock::time_point tic_0 = std::chrono::steady_clock::now();


    std::string name = argv[1];
    int NUM_THREADS = atoi(argv[2]);
    std::string NUM_THREADS_STR = std::to_string(NUM_THREADS);

    
    queue Q(default_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;

    std::cout << "Number of parallel work items: " << NUM_THREADS << std::endl;

    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();
    graph *g = malloc_shared<graph>(1, Q);
    g->load_graph(name, Q);
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();

    std::cout << "Time to load data from files: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    int nc = 0;
    printf("If left partition size is not given it takes n/2 by default\n"); fflush(stdout);
    if(argc == 4) nc = atoi(argv[3]);
    else nc = g->get_num_nodes()/2;
    printf("Left partition size: %d\n", nc); fflush(stdout);


    int V = g->get_num_nodes();
    int E = g->get_num_edges();
    int STRIDE = NUM_THREADS;

    std::cout <<"#Nodes: " << V << std::endl;
    std::cout << "#Edges: " << E << std::endl;

    bool *h_noNewPaths = (bool *)malloc(1 * sizeof(bool));
    int *h_L0 = (int *)malloc(1 * sizeof(int));
    int *h_NOT_VISITED = (int *)malloc(1 * sizeof(int));
    int *h_bfsLevel = (int *)malloc(1 * sizeof(int));
    bool *h_noNewVertices = (bool *)malloc(1 * sizeof(bool));
    bool *h_modified = (bool *)malloc(V * sizeof(bool));
    bool *h_compressed = (bool *)malloc(1 * sizeof(bool));

    // h_noNewPaths = hostMemAllocateBool(1);
    // h_L0 = hostMemAllocateInteger(1);
    // h_NOT_VISITED = hostMemAllocateInteger(1);
    // h_bfsLevel = hostMemAllocateInteger(1);
    // h_noNewVertices = hostMemAllocateBool(1);
    // h_modified = hostMemAllocateBool(V);
    // h_compressed = hostMemAllocateBool(1);

    // Device Variables
    bool *d_modified = malloc_device<bool>(V, Q);
    int *d_rmatch = malloc_device<int>(V, Q);
    int *d_cmatch = malloc_device<int>(V, Q);
    bool *d_noNewPaths = malloc_device<bool>(1, Q);
    int *d_L0 = malloc_device<int>(1, Q);
    int *d_bfsArray = malloc_device<int>(V, Q);
    int *d_NOT_VISITED = malloc_device<int>(1, Q);
    int *d_predeccesor = malloc_device<int>(V, Q);
    int *d_bfsLevel = malloc_device<int>(1, Q);
    bool *d_noNewVertices = malloc_device<bool>(1, Q);
    bool *d_compress = malloc_device<bool>(V, Q);
    bool *d_compress_next = malloc_device<bool>(V, Q);
    bool *d_compressed = malloc_device<bool>(1, Q);
    bool *d_modified_next = malloc_device<bool>(V, Q);


    // d_modified = deviceMemAllocateBool(Q, V);
    // d_rmatch = deviceMemAllocateInteger(Q, V);
    // d_cmatch = deviceMemAllocateInteger(Q, V);
    // d_noNewPaths = deviceMemAllocateBool(Q, 1);
    // d_L0 = deviceMemAllocateInteger(Q, 1);
    // d_bfsArray = deviceMemAllocateInteger(Q, V);
    // d_NOT_VISITED = deviceMemAllocateInteger(Q, 1);
    // d_predeccesor = deviceMemAllocateInteger(Q, V);
    // d_bfsLevel = deviceMemAllocateInteger(Q, 1);
    // d_noNewVertices = deviceMemAllocateBool(Q, 1);
    // d_compress = deviceMemAllocateBool(Q, V);
    // d_compress_next = deviceMemAllocateBool(Q, V);
    // d_compressed = deviceMemAllocateBool(Q, 1);
    // d_modified_next = deviceMemAllocateBool(Q, V);

    // init(Q, d_modified, false, V, NUM_THREADS, STRIDE);
    // init(Q, d_rmatch, -1, V, NUM_THREADS, STRIDE);
    // init(Q, d_cmatch, -1, V, NUM_THREADS, STRIDE);
    // init(Q, d_modified_next, false, V, NUM_THREADS, STRIDE);

    initialize(d_modified, false, NUM_THREADS, V, Q);
    initialize(d_rmatch, -1, NUM_THREADS, V, Q);
    initialize(d_cmatch, -1, NUM_THREADS, V, Q);
    initialize(d_modified_next, false,NUM_THREADS, V, Q);


    tic = std::chrono::steady_clock::now();
    std::cout << "Starting maximum matching calculations..." << std::endl;

    h_noNewPaths[0] = false;

    while(!h_noNewPaths[0]) {
        // std:: cout << "hi\n";
        h_noNewPaths[0] = true;

        // memoryCopy(Q, d_noNewPaths, h_noNewPaths, 1, NUM_THREADS, STRIDE);
        memcpy(d_noNewPaths, h_noNewPaths, 1, Q);
        h_L0[0] = 0;

        h_NOT_VISITED[0] = h_L0[0] - 1;

        // init(Q, d_bfsArray, h_NOT_VISITED[0], V, NUM_THREADS, STRIDE); 
        initialize(d_bfsArray, h_NOT_VISITED[0], NUM_THREADS, V, Q);

        // memoryCopy(Q, d_L0, h_L0, 1, NUM_THREADS, STRIDE);
        memcpy(d_L0, h_L0, 1, Q);
        //##########################################################//
        //#######################KERNEL - 1#########################//


        APFB_kernel_1(Q, V, nc, d_cmatch, d_bfsArray, d_L0, NUM_THREADS, STRIDE);


        //##########################################################//

        // init(Q, d_predeccesor, -1, V, NUM_THREADS, STRIDE);
        initialize(d_predeccesor, -1, NUM_THREADS, V, Q);

        h_bfsLevel[0] = h_L0[0];
        h_noNewVertices[0] = false;

        // init(Q, d_modified_next, false, V, NUM_THREADS, STRIDE);
        initialize(d_modified_next, false, NUM_THREADS, V, Q);

        while(!h_noNewVertices[0]) {
            // std:: cout << "hi 2\n";
            h_noNewVertices[0] = true;
            // memoryCopy(Q, d_noNewVertices, h_noNewVertices, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, d_bfsLevel, h_bfsLevel, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, d_NOT_VISITED, h_NOT_VISITED, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, d_noNewVertices, h_noNewVertices, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, d_noNewPaths, h_noNewPaths, 1, NUM_THREADS, STRIDE);

            memcpy(d_noNewVertices, h_noNewVertices, 1, Q);
            memcpy(d_bfsLevel, h_bfsLevel, 1, Q);
            memcpy(d_NOT_VISITED, h_NOT_VISITED, 1, Q);
            memcpy(d_noNewVertices, h_noNewVertices, 1, Q);
            memcpy(d_noNewPaths, h_noNewPaths, 1, Q);

            //##########################################################//
            //#######################KERNEL - 2#########################//

            APFB_kernel_2(g, Q, V, nc, d_bfsArray, d_bfsLevel,  d_rmatch, d_NOT_VISITED, d_noNewVertices, d_predeccesor, d_noNewPaths, NUM_THREADS, STRIDE);

            // memoryCopy(Q, h_bfsLevel, d_bfsLevel, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, h_NOT_VISITED, d_NOT_VISITED, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, h_noNewVertices, d_noNewVertices, 1, NUM_THREADS, STRIDE);
            // memoryCopy(Q, h_noNewPaths, d_noNewPaths, 1, NUM_THREADS, STRIDE);

            memcpy(h_bfsLevel, d_bfsLevel, 1, Q);
            memcpy(h_NOT_VISITED, d_NOT_VISITED, 1, Q);
            memcpy(h_noNewVertices, d_noNewVertices, 1, Q);
            memcpy(h_noNewPaths, d_noNewPaths, 1, Q);
            //##########################################################//
            h_bfsLevel[0] = h_bfsLevel[0] + 1;
            // memoryCopy(Q, h_modified, d_modified_next, V, NUM_THREADS, STRIDE);
            // memoryCopy(Q, d_modified, h_modified, V, NUM_THREADS, STRIDE);
            // init(Q, d_modified_next, false, V, NUM_THREADS, STRIDE);

            memcpy(h_modified, d_modified_next, V, Q);
            memcpy(d_modified, h_modified, V, Q);
            initialize(d_modified_next, false, NUM_THREADS, V, Q);

        }
        // init(Q, d_compress, false, V, NUM_THREADS, STRIDE);
        // init(Q, d_compress_next, false, V, NUM_THREADS, STRIDE);

        initialize(d_compress, false, NUM_THREADS, V, Q);
        initialize(d_compress_next, false, NUM_THREADS, V, Q);

        //##########################################################//
        //#######################KERNEL - 3#########################//

        APFB_kernel_3(Q, V, nc, d_rmatch, d_compress, NUM_THREADS, STRIDE);

        //##########################################################//

        h_compressed[0] = false;
        // init(Q, d_modified_next, false, V, NUM_THREADS, STRIDE);
        initialize(d_modified_next, false, NUM_THREADS, V, Q);

        while(!h_compressed[0]) {
            // std:: cout << "hi 3\n";
            h_compressed[0] = true;

            //##########################################################//
            //#######################KERNEL - 4#########################//

            // memoryCopy(Q, d_compressed, h_compressed, 1, NUM_THREADS, STRIDE);
            memcpy(d_compressed, h_compressed, 1, Q);

            APFB_kernel_4(Q, V, nc, d_compress, d_predeccesor, d_cmatch, d_rmatch, d_compress_next, d_compressed, NUM_THREADS, STRIDE);

            // memoryCopy(Q, h_compressed, d_compressed, 1, NUM_THREADS, STRIDE);
            memcpy(h_compressed, d_compressed, 1, Q);
            //##########################################################//

            //##########################################################//
            //#######################KERNEL - 5#########################//
            
            APFB_kernel_5(Q, V, nc, d_compress, d_compress_next, NUM_THREADS, STRIDE);
            // memoryCopy(Q, h_modified, d_modified_next, V, NUM_THREADS, STRIDE);
            // memoryCopy(Q, d_modified, h_modified, V, NUM_THREADS, STRIDE);

            memcpy(h_modified, d_modified_next, V, Q);
            memcpy(d_modified, h_modified, V, Q);
            //##########################################################//

            // init(Q, d_modified_next, false, V, NUM_THREADS, STRIDE);
            initialize(d_modified_next, false, NUM_THREADS, V, Q);
        }
        APFB_kernel_6(Q, V, nc, d_rmatch, d_cmatch, NUM_THREADS, STRIDE);

        // memoryCopy(Q, h_noNewPaths, d_noNewPaths, 1, NUM_THREADS, STRIDE);
        // memoryCopy(Q, h_modified, d_modified_next, V, NUM_THREADS, STRIDE);
        // memoryCopy(Q, d_modified, h_modified, V, NUM_THREADS, STRIDE);
        // init(Q, d_modified_next, false, V, NUM_THREADS, STRIDE);

        memcpy(h_noNewPaths, d_noNewPaths, 1, Q);
        memcpy(h_modified, d_modified_next, V, Q);
        memcpy(d_modified, h_modified, V, Q);
        initialize(d_modified_next, false, NUM_THREADS, V, Q);
    }
    toc = std::chrono::steady_clock::now();
    std::cout<< "Time to run maximum bipartite matching: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << std::endl;

    int *h_rmatch = hostMemAllocateInteger(V);
    int *h_cmatch = hostMemAllocateInteger(V);

    // memoryCopy(Q, h_rmatch, d_rmatch, V, NUM_THREADS, STRIDE);
    // memoryCopy(Q, h_cmatch, d_cmatch, V, NUM_THREADS, STRIDE);

    memcpy(h_rmatch, d_rmatch, V, Q);
    memcpy(h_cmatch, d_cmatch, V, Q);

    int cntMatchings = 0;
    for(int i = 0; i < V; i++) {
        if(h_cmatch[i] >= 0) {
            cntMatchings += 1;
        }
    }

    printf("Maximum Matches: %d\n", cntMatchings);



}