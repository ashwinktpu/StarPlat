#include<bit/stdc++.h>
#include<CL/sycl.hpp>
#include "graph.hpp"
using namespace sycl;

//-------------------------- CONSTANTS ---------------------------------//

const int NUM_THREADS = 1048576;
const int STRIDE = NUM_THREADS;




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