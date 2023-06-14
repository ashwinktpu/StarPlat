#include "PageRank_DSL_V2.h"
#include<bits/stdc++.h>
using namespace std;
int main(int argc, char *argv[])
{
    graph g = graph(argv[1]);
    cout<<" Parsing Graph ....";
    g.parseGraph();
    cout<<"done"<<endl;
    int V = g.num_nodes();
    float beta = 0.001;
    float delta = 0.85;
    int maxIter = 100;
    float *pageRank = (float*) malloc(V*sizeof(float));
    Compute_PR(g, beta, delta, maxIter, pageRank);

    FILE* op = fopen(argv[2],"w");
    for(int i=0; i<V; i++)
    {
        fprintf(op, "%f\n", pageRank[i]);
    }
    fclose(op);
    cout<<"finished."<<endl;
    return 0;
}
