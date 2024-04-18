#include "triangle_counting_dsl.h"


long Compute_TC(
  graph& g
) {

  int V = g.num_nodes();
  int E = g.num_edges();

  int *hOffsetArray;
  int *hEdgelist;

  hOffsetArray = (int*) malloc(sizeof(int) * (V + 1));
  hEdgelist = (int*) malloc(sizeof(int) * (E));

  for(int i = 0; i <= V; i++) {
    hOffsetArray[i] = g.indexofNodes[i];
  }
  for(int i = 0; i < E; i++) {
    hEdgelist[i] = g.edgeList[i];
  }

  for(int i = 0; i < V; i++) {
    printf("Node %d: ", i);
    for(int j = hOffsetArray[i]; j < hOffsetArray[i + 1]; j++) {
      printf("%d ", hEdgelist[j]);
    }
    printf("\n");
  }

  int *dOffsetArray;
  int *dEdgelist;

  hipMalloc(&dOffsetArray, sizeof(int) * (V + 1));
  hipMalloc(&dEdgelist, sizeof(int) * (E));

  hipMemcpy(dOffsetArray, hOffsetArray, sizeof(int) * (V + 1), hipMemcpyHostToDevice);
  hipMemcpy(dEdgelist, hEdgelist, sizeof(int) * (E), hipMemcpyHostToDevice);


  const unsigned threadsPerBlock = 512;
  const unsigned numThreads = (V < threadsPerBlock) ? 512 : V;
  const unsigned numBlocks = (V + threadsPerBlock - 1) / threadsPerBlock;


  long hTriangle_count = 0;
  long *dTriangle_count;
  hipMalloc(&dTriangle_count, sizeof(long));
  hipMemcpy(dTriangle_count, &hTriangle_count, sizeof(long) * (1), hipMemcpyHostToDevice);
  Compute_TCKernel<<<numBlocks, numThreads>>>(V, E, dOffsetArray, dEdgelist, dTriangle_count);
  hipDeviceSynchronize();
  hipMemcpy(&hTriangle_count, dTriangle_count, sizeof(long) * (1), hipMemcpyDeviceToHost);

  hipError_t hipErr = hipGetLastError();
  if (hipErr != hipSuccess) {
    std::cout << "HIP Error: " << hipGetErrorString(hipErr) << std::endl;
  }



  return hTriangle_count;
}

int main(int argc, char* argv[]) {
  // Check if the correct number of command line arguments are provided
  if (argc != 2) {
    std::cout << "Usage: ./triangle_counting <input_file>" << std::endl;
    return 1;
  }

  // Read the input file
  std::string input_file = argv[1];
  graph g(argv[1]);
  g.parseGraph();

  // Compute the triangle count
  long triangle_count = Compute_TC(g);

  // Print the result
  std::cout << "Triangle count: " << triangle_count << std::endl;

  return 0;
}
