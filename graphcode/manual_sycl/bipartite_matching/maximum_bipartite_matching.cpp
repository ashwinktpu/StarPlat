#include<bit/stdc++.h>
#include<CL/sycl.hpp>
#include "graph.hpp"
using namespace sycl;

//-------------------------- CONSTANTS ---------------------------------//

const int NUM_THREADS = 1048576;
const int STRIDE = NUM_THREADS;

//-------------------------- Utility Functions -------------------------//

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


//------------------------------ Kernels ------------------------------//

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


int main(int argc, char** argv) {
    char* inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp); fflush(stdout);

    graph g(inp);
    g.parseGraph(isWeighted);

    int nc = 0;
    printf("If left partition size is not given it takes n/2 by default\n"); fflush(stdout);
    if(argc == 4) nc = atoi(argv[3]);
    else nc = g.num_nodes()/2;
    printf("Left partition size: %d\n", nc); fflush(stdout);

    APFB(g, nc);
    return 0;
}