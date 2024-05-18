#include<bits/stdc++.h>
#include "graph.hpp"
using namespace std;

int main(int argc, char **argv){
    graph g(argv[1]);
    g.parseGraph();
    cout<<g.num_nodes()<<endl;
    cout<<g.num_edges()<<endl;
}