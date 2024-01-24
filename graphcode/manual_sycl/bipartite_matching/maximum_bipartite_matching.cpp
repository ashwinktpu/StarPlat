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