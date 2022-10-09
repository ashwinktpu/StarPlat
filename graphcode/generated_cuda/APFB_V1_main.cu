// #include "PR_V1.h"
#include<bits/stdc++.h>
using namespace std;

#include "APFB_V1.cu"

//   int* h_rmatch = (int *)malloc((V)*sizeof(int));
//   int* h_cmatch = (int *)malloc((V)*sizeof(int));
//   cudaMemcpy(h_rmatch, d_rmatch, V * sizeof(int), cudaMemcpyDeviceToHost);
//   cudaMemcpy(h_cmatch, d_cmatch, V * sizeof(int), cudaMemcpyDeviceToHost);

//   for(int i = 0; i < V; i++){
//     printf("rmatch[%d]: %d, cmatch[%d]: %d\n", i, h_rmatch[i], i, h_cmatch[i]);
//   }

int main(int argc, char** argv) {
    char* inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp);
    
    int nc = 6;
    
    graph g(inp);
    g.parseGraph(isWeighted);
    APFB(g, nc);
    return 0;
}