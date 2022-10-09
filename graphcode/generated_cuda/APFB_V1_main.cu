// #include "PR_V1.h"
#include<bits/stdc++.h>
using namespace std;

#include "APFB_V1.cu"

int main(int argc, char** argv) {
    char* inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp);
    
    graph g(inp);
    g.parseGraph(isWeighted);
    APFB(g, nc);
    return 0;
}