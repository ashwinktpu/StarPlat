#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

// Include the headers of the modules
#include "graph_conversion.h"
#include "csr_conversion.h"
#include "metis_integration.h"
#include "partitioned_writer.h"

int main(int argc, char** argv) {
    std::string inputFilename = argv[1]; // Replace with your actual input file
    int numPartitions = atoi(argv[2]); // Adjust the number of partitions as needed
    int offset = atoi (argv[3]) ;
    std::string outputFolderPath = (argv[4]) ;

    std::cout << inputFilename << std::endl ;
    // Step 1: Read the weighted directed graph
    std::ifstream inputFile(inputFilename);
    std::vector<std::tuple<int, int, int>> edges; // Store edges as (u, v, weight)

    int u, v, weight;
    while (inputFile >> u >> v >> weight) {
        edges.emplace_back(u, v, weight);
    }
    inputFile.close();

    // Step 2: Convert to an undirected graph
    std::vector<std::unordered_map<int, int>> undirectedGraph;
    convertToUndirected(edges, undirectedGraph);

    // Step 3: Convert the undirected graph to CSR format
    std::vector<int> xadj, adjncy, weights;
    convertToCSR(undirectedGraph, xadj, adjncy, weights);

    // Step 4: Partition the graph using METIS (placeholder)
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> partitionResult = partitionGraph(xadj, adjncy, weights, numPartitions);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by partitionGraph: "
                    << duration.count() << " microseconds" << std::endl;


    // Step 5: Write the partitioned graph into separate files
    writePartitionedGraph(edges, partitionResult, numPartitions, offset, outputFolderPath);

    return 0;
}
