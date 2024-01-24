#include<bit/stdc++.h>
#include<CL/sycl.hpp>
#include "graph.hpp"
using namespace sycl;

// ---------------------------   Constants --------------------------//

 const int NUM_THREADS = 1048576;
 const int stride = NUM_THREADS;

 

int main(int argc, char** argv) {
    char *inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp); fflush(stdout);

    graph g(inp);
    g.parseGraph(isWeighted);

    max_coloring(g);
}