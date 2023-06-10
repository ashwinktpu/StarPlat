#include "triangle_counting_DSL.h"
#include<bits/stdc++.h>
using namespace std;
int main(int argc, char *argv[])
{
    graph g = graph(argv[1]);
    cout<<" Parsing Graph ....";
    g.parseGraph();
    cout<<"done"<<endl;
    int V = g.num_nodes();
    int n = atoi(argv[3]);
    Compute_TC(g);
    return 0;
}
