#include<bits/stdc++.h>
#include <CL/sycl.hpp>
#include "graph.hpp"
using namespace sycl;

const int NUM_THREADS = 1048576;
const int STRIDE = NUM_THREADS;

//--------------------  UTILITY FUNCTIONS :: START ------------------------------//

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

//--------------------  UTILITY FUNCTIONS :: END ------------------------------//

//--------------------  KERNELs :: START ------------------------------//

void APFB_kernel_1(queue &Q, int V, int nc, int *d_cmatch, int *d_bfsArray, int *d_L0) {

    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> i) {
            for(int c = i; c < V; c += STRIDE) {
                if(c < nc) {
                    if(d_cmatch[c] == -1) {
                        d_bfsArray[c] = d_L0[0];
                    }
                }
            }
        });
    }).wait();
}

void APFB_kernel_2(queue &Q, int V, int nc, int *d_bfsArray, int *d_bfsLevel, int *d_meta, int *d_data, int *d_rmatch, int *d_NOT_VISITED, bool *d_noNewVertices, int *d_predeccesor, bool *d_noNewPaths) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> i) {
            for(int col_vertex = i; col_vertex < V; col_vertex += STRIDE) {
                if(col_vertex < nc) {
                    if(d_bfsArray[col_vertex] == d_bfsLevel[0]) {
                        for(int edge = d_meta[col_vertex]; edge < d_meta[col_vertex + 1]; edge++) {
                            int neigh_row = d_data[edge];
                            if(neigh_row >= nc) {
                                int col_match = d_rmatch[neigh_row];

                                if(col_match > -1) {
                                    if(d_bfsArray[col_match] == d_NOT_VISITED[0]) {
                                        d_noNewVertices[0] = false;
                                        d_bfsArray[col_match] = d_bfsLevel[0] + 1;
                                        d_predeccesor[neigh_row] = col_vertex;
                                    }
                                }

                                if(col_match == -1) {
                                    d_rmatch[neigh_row] = -2;
                                    d_predeccesor[neigh_row] = col_vertex;
                                    d_noNewPaths[0] = false;
                                }
                            }
                        }
                    }
                }
            }
        });
    }).wait();
}

void APFB_kernel_3(queue &Q, int V, int nc, int *d_rmatch, bool *d_compress) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> i) {
            for(int r = i; r < V; r += STRIDE) {
                if(r >= nc && d_rmatch[r] == -2) {
                    d_compress[r] = true;
                }
            }
        });
    }).wait();
}

void APFB_kernel_4(queue &Q, int V, int nc, bool *d_compress, int *d_predeccesor, int *d_cmatch, int *d_rmatch, bool *d_compress_next, bool *d_compressed) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> i) {
            for(int row_vertex = i; row_vertex < V; row_vertex += STRIDE) {
                if(d_compress[row_vertex] == true) {
                    if(row_vertex >= nc) {
                        int matched_col = d_predeccesor[row_vertex];
                        int matched_row = d_cmatch[matched_col];

                        bool isValid = true;

                        if(matched_row != -1) {
                            if(d_predeccesor[matched_row] == matched_col) {
                                isValid = false;
                            }
                        }

                        if(isValid) {
                            d_cmatch[matched_col] = row_vertex;
                            d_rmatch[row_vertex] = matched_col;
                            if(matched_row != -1) {
                                d_compress_next[matched_row] = true;
                                d_compressed[0] = false;
                            }
                        }
                    }
                }
            }
        });
    }).wait();
}

void APFB_kernel_5(queue &Q, int V, int nc, bool* d_compress, bool *d_compress_next) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> i) {
            for(int row_vertex = i; row_vertex < V; row_vertex += STRIDE) {
                if(row_vertex >= nc) {
                    d_compress[row_vertex] = d_compress_next[row_vertex];
                    d_compress_next[row_vertex] = false;
                }
            }
        });
    }).wait();
}

void APFB_kernel_6(queue &Q, int V, int nc, int *d_rmatch, int *d_cmatch) {
    Q.submit([&](handler &h) {
        h.parallel_for(NUM_THREADS, [=](id<1> i) {
            for(int r = i; r < V; r += STRIDE) {
                if(r >= nc) {
                    int c = d_rmatch[r];

                    if(c >= 0) {
                        if(d_cmatch[c] != r) {
                            d_rmatch[r] = -1;
                        }
                    }

                    if(c == -2) {
                        d_rmatch[r] = -1;
                    }
                }
            }
        });
    }).wait();
}
//--------------------  KERNELs :: END ------------------------------//



void APFB(graph &g, int nc) {

    queue Q(default_selector_v);
    std::cout << "Selected Device:"<< Q.get_device().get_info<info::device::name>() << std::endl;
    std::cout << "Selected Device Maximum Memory Allocation Size:"<< Q.get_device().get_info<info::device::max_mem_alloc_size>()<< std::endl;


    int V = g.num_nodes();
    int E = g.num_edges();

    printf("#nodes : %d\n", V);
    printf("#edges : %d\n", E);
    int *edgeLen = g.getEdgeLen();

    int *h_meta;
    int *h_data;
    int *h_src;
    int *h_weight;
    int *h_rev_meta;

    h_meta = hostMemAllocateInteger(V + 1);
    h_data = hostMemAllocateInteger(E);
    h_src = hostMemAllocateInteger(E);
    h_weight = hostMemAllocateInteger(E);
    h_rev_meta = hostMemAllocateInteger(V + 1);

    for(int i = 0; i <= V; i++) {
        int temp = g.indexofNodes[i];
        h_meta[i] = temp;
        temp = g.rev_indexofNodes[i];
        h_rev_meta[i] = temp;
    }

    for(int i = 0; i < E; i++) {
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

    d_meta = deviceMemAllocateInteger(Q, V + 1);
    d_data = deviceMemAllocateInteger(Q, E);
    d_src = deviceMemAllocateInteger(Q, E);
    d_weight = deviceMemAllocateInteger(Q, E);
    d_rev_meta = deviceMemAllocateInteger(Q, V + 1);
    d_modified_next = deviceMemAllocateBool(Q, V);

    memoryCopy(Q, d_meta, h_meta, V+1);
    memoryCopy(Q, d_data, h_data, E);
    memoryCopy(Q, d_src, h_src, E);
    memoryCopy(Q, d_weight, h_weight, E);
    memoryCopy(Q, d_rev_meta, h_rev_meta, V + 1);

    // TIMER START
    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();


    // Host Variables
    bool *h_noNewPaths;
    int *h_L0;
    int *h_NOT_VISITED;
    int *h_bfsLevel;
    bool *h_noNewVertices;
    bool *h_modified;
    bool *h_compressed;

    h_noNewPaths = hostMemAllocateBool(1);
    h_L0 = hostMemAllocateInteger(1);
    h_NOT_VISITED = hostMemAllocateInteger(1);
    h_bfsLevel = hostMemAllocateInteger(1);
    h_noNewVertices = hostMemAllocateBool(1);
    h_modified = hostMemAllocateBool(V);
    h_compressed = hostMemAllocateBool(1);

    // Device Variables
    bool *d_modified;
    int *d_rmatch;
    int *d_cmatch;
    bool *d_noNewPaths;
    int *d_L0;
    int *d_bfsArray;
    int *d_NOT_VISITED;
    int *d_predeccesor;
    int *d_bfsLevel;
    bool *d_noNewVertices;
    bool *d_compress;
    bool *d_compress_next;
    bool *d_compressed;


    d_modified = deviceMemAllocateBool(Q, V);
    d_rmatch = deviceMemAllocateInteger(Q, V);
    d_cmatch = deviceMemAllocateInteger(Q, V);
    d_noNewPaths = deviceMemAllocateBool(Q, 1);
    d_L0 = deviceMemAllocateInteger(Q, 1);
    d_bfsArray = deviceMemAllocateInteger(Q, V);
    d_NOT_VISITED = deviceMemAllocateInteger(Q, 1);
    d_predeccesor = deviceMemAllocateInteger(Q, V);
    d_bfsLevel = deviceMemAllocateInteger(Q, 1);
    d_noNewVertices = deviceMemAllocateBool(Q, 1);
    d_compress = deviceMemAllocateBool(Q, V);
    d_compress_next = deviceMemAllocateBool(Q, V);
    d_compressed = deviceMemAllocateBool(Q, 1);

    init(Q, d_modified, false, V);
    init(Q, d_rmatch, -1, V);
    init(Q, d_cmatch, -1, V);
    init(Q, d_modified_next, false, V);


    h_noNewPaths[0] = false;

    while(!h_noNewPaths[0]) {
        // std:: cout << "hi\n";
        h_noNewPaths[0] = true;

        memoryCopy(Q, d_noNewPaths, h_noNewPaths, 1);
        h_L0[0] = 0;

        h_NOT_VISITED[0] = h_L0[0] - 1;

        init(Q, d_bfsArray, h_NOT_VISITED[0], V); 

        memoryCopy(Q, d_L0, h_L0, 1);
        //##########################################################//
        //#######################KERNEL - 1#########################//

        APFB_kernel_1(Q, V, nc, d_cmatch, d_bfsArray, d_L0);

        //##########################################################//

        init(Q, d_predeccesor, -1, V);

        h_bfsLevel[0] = h_L0[0];
        h_noNewVertices[0] = false;

        init(Q, d_modified_next, false, V);

        while(!h_noNewVertices[0]) {
            // std:: cout << "hi 2\n";
            h_noNewVertices[0] = true;
            memoryCopy(Q, d_noNewVertices, h_noNewVertices, 1);
            memoryCopy(Q, d_bfsLevel, h_bfsLevel, 1);
            memoryCopy(Q, d_NOT_VISITED, h_NOT_VISITED, 1);
            memoryCopy(Q, d_noNewVertices, h_noNewVertices, 1);
            memoryCopy(Q, d_noNewPaths, h_noNewPaths, 1);

            //##########################################################//
            //#######################KERNEL - 2#########################//

            APFB_kernel_2(Q, V, nc, d_bfsArray, d_bfsLevel, d_meta, d_data, d_rmatch, d_NOT_VISITED, d_noNewVertices, d_predeccesor, d_noNewPaths);

            memoryCopy(Q, h_bfsLevel, d_bfsLevel, 1);
            memoryCopy(Q, h_NOT_VISITED, d_NOT_VISITED, 1);
            memoryCopy(Q, h_noNewVertices, d_noNewVertices, 1);
            memoryCopy(Q, h_noNewPaths, d_noNewPaths, 1);
            //##########################################################//

            h_bfsLevel[0] = h_bfsLevel[0] + 1;
            memoryCopy(Q, h_modified, d_modified_next, V);
            memoryCopy(Q, d_modified, h_modified, V);
            init(Q, d_modified_next, false, V);
        }

        init(Q, d_compress, false, V);
        init(Q, d_compress_next, false, V);

        //##########################################################//
        //#######################KERNEL - 3#########################//

        APFB_kernel_3(Q, V, nc, d_rmatch, d_compress);

        //##########################################################//

        h_compressed[0] = false;
        init(Q, d_modified_next, false, V);

        while(!h_compressed[0]) {
            // std:: cout << "hi 3\n";
            h_compressed[0] = true;

            //##########################################################//
            //#######################KERNEL - 4#########################//

            memoryCopy(Q, d_compressed, h_compressed, 1);

            APFB_kernel_4(Q, V, nc, d_compress, d_predeccesor, d_cmatch, d_rmatch, d_compress_next, d_compressed);

            memoryCopy(Q, h_compressed, d_compressed, 1);

            //##########################################################//

            //##########################################################//
            //#######################KERNEL - 5#########################//
            
            APFB_kernel_5(Q, V, nc, d_compress, d_compress_next);
            memoryCopy(Q, h_modified, d_modified_next, V);
            memoryCopy(Q, d_modified, h_modified, V);
            //##########################################################//

            init(Q, d_modified_next, false, V);
        }

        APFB_kernel_6(Q, V, nc, d_rmatch, d_cmatch);

        memoryCopy(Q, h_noNewPaths, d_noNewPaths, 1);
        memoryCopy(Q, h_modified, d_modified_next, V);
        memoryCopy(Q, d_modified, h_modified, V);
        init(Q, d_modified_next, false, V);
    }
    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();

    int *h_rmatch = hostMemAllocateInteger(V);
    int *h_cmatch = hostMemAllocateInteger(V);

    memoryCopy(Q, h_rmatch, d_rmatch, V);
    memoryCopy(Q, h_cmatch, d_cmatch, V);

    int cntMatchings = 0;
    for(int i = 0; i < V; i++) {
        if(h_cmatch[i] >= 0) {
            cntMatchings += 1;
        }
    }

    printf("Maximum Matches: %d\n", cntMatchings);
    std::cout << "Time required: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() << "[µs]" << "   "<<std::endl;
}




int main(int argc, char** argv) {
    char* inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp); fflush(stdout);

    graph g(inp);
    g.parseGraph();

    int nc = 0;
    printf("If left partition size is not given it takes n/2 by default\n"); fflush(stdout);
    if(argc == 4) nc = atoi(argv[3]);
    else nc = g.num_nodes()/2;
    printf("Left partition size: %d\n", nc); fflush(stdout);

    APFB(g, nc);
    return 0;
}