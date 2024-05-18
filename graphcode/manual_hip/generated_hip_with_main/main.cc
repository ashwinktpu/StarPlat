int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: ./program <input_file>" << std::endl;
    return 1;
  }

  std::string inputFileName = argv[1];

  graph g(argv[1]);
  g.parseGraph();

  int V = g.num_nodes();
  int E = g.num_edges();
  int* hDist = new int[V];
  int* hWeight = new int[E];

  cout << "Number of nodes: " << V << endl;
  cout << "Number of edges: " << E << endl;

  int src = 0;

  // Compute SSSP
  Compute_SSSP(g, hDist, g.getEdgeLen(), src);

  for (int i = 0; i < V; i++) {
    std::cout << "Distance from source to node " << i << ": " << hDist[i] << std::endl;
  }

  // Clean up memory
  delete[] hDist;
  delete[] hWeight;

  return 0;
}