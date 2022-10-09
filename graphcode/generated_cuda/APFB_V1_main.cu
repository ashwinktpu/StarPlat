// #include "PR_V1.h"
#include<bits/stdc++.h>
using namespace std;

#include "APFB_V1.cu"

int main(int argc, char** argv) {
    char* inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    printf("Taking input from: %s\n", inp);
    
    int nc = 3;
    
//       bool* h_rmatch = (bool *)malloc((V)*sizeof(int));
//   bool* h_cmatch = (bool *)malloc((V)*sizeof(int));
//   cudaMemcpy(h_rmatch, d_rmatch, V * sizeof(int), cudaMemcpyDeviceToHost);
//   cudaMemcpy(h_cmatch, d_rmatch, V * sizeof(int), cudaMemcpyDeviceToHost);

//   for(int i = 0; i < V; i++){
//     printf("H rmatch[%d]: %d, cmatch[%d]: %d\n", i, h_rmatch[i], i, h_cmatch[i]);
//   }
    
    graph g(inp);
    g.parseGraph(isWeighted);
    APFB(g, nc);
    return 0;
}