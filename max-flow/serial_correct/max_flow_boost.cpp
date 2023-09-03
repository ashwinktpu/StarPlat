#include <fstream>
#include "graph_mpi_weight.hpp"
#include <boost/graph/adjacency_list.hpp>

using namespace std;

// covnert edge list to adjacency list.

vector<int> * edge_to_adj (vector<vector<int>> &edges, int vertices) {

  vector<int>* adj = new vector<int> [vertices];
  
  for (auto edge : edges) {

    adj[edge[0]].push_back ({adj[edge[1]], adj[edge[2]]});
  }

  return adj;
}

int main (int argc, char ** argv) {

  char* file_name = argv[1];
  graph g (file_name);
  g.parseGraph () ;

   
  return 0;
}
