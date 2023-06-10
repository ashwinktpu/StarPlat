#include "sssp_dslV2.h"

#include <bits/stdc++.h>

using namespace std;
int main(int argc, char *argv[])
{
    graph g = graph(argv[1]);
    cout<<" Parsing Graph ....";
    g.parseGraph();
    cout<<"done"<<endl;
    int V = g.num_nodes();
    
    int *dist = (int*) malloc(V*sizeof(int));
    int src = atoi(argv[3]);
    Compute_SSSP(g, dist, src);

    FILE* op = fopen(argv[2],"w");
    for(int i=0; i<V; i++)
    {
        fprintf(op, "%d %d\n",i,dist[i]);
    }
    fclose(op);
    cout<<"finished."<<endl;
    return 0;
}
