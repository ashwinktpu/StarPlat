// #include "PR_V1.h"
#include<bits/stdc++.h>
using namespace std;

#include "APFB_V1.cu"

//   int* h_rmatch = (int *)malloc((V)*sizeof(int));
//   int* h_cmatch = (int *)malloc((V)*sizeof(int));
//   cudaMemcpy(h_rmatch, d_rmatch, V * sizeof(int), cudaMemcpyDeviceToHost);
//   cudaMemcpy(h_cmatch, d_cmatch, V * sizeof(int), cudaMemcpyDeviceToHost);

//   int cntMatchings = 0;
//   for(int i = 0; i < V; i++){
//     // printf("rmatch[%d]: %d, cmatch[%d]: %d\n", i, h_rmatch[i], i, h_cmatch[i]);
//     if(h_cmatch[i] >= 0){
//       cntMatchings += 1;
//     }
//   }
//   printf("Maximum MATCHES: %d\n", cntMatchings);

int main(int argc, char** argv) {
    char* inp = argv[1];
    bool isWeighted = atoi(argv[2]) ? true : false;
    int nc = atoi(argv[3]);
    printf("Taking input from: %s\n", inp);
    
    graph g(inp);
    g.parseGraph(isWeighted);
    APFB(g, nc);
    return 0;
}