// partitioned_writer.cpp

#include "partitioned_writer.h"
#include <fstream>
#include <iomanip>

void writePartitionedGraph(const std::vector<std::tuple<int, int, int>>& edges, 
                           const std::vector<int>& partitionResult, 
                           int numPartitions) {
    std::vector<std::ofstream> files(numPartitions);

    // Open files for each partition
    for (int i = 0; i < numPartitions; ++i) {
        std::string filename ;
        if (i < 10 ) filename = "../part_0" + std::to_string(i) ;
        else filename = "../part_" + std::to_string(i) ;
        files[i].open(filename);
    }

    // Write edges to corresponding partition files
    for (const auto& edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int weight = std::get<2>(edge);
        int partition = partitionResult[v]; // Assuming partition is based on vertex 'v'

        files[partitionResult[u]] << u << " " << v << " " << weight << " " << partition << "\n";
    }

    // Close all files
    for (auto& file : files) {
        file.close();
    }
}
