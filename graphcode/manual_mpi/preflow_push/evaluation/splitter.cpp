#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> // for std::max

void partitionGraph(const std::string& graphFile, int numPartitions) {
    std::ifstream inputFile(graphFile);
    int u, v, w;

    int vx = 0;
    std::vector<std::vector<int>> edges;
    std::vector<int> partition;

    int edgeCount = 0;

    while (inputFile >> u >> v >> w) {
        edges.push_back({u, v, w});
        vx = std::max({vx, u, v});
    }

    partition.resize(vx + 1);

    int i = 0;
    for (; i <= vx / 4; ++i) {
        partition[i] = i % numPartitions;
    }
    for (; i <= vx / 2; ++i) {
        partition[i] = i % numPartitions;
    }
    for (; i <= 3 * vx / 4; ++i) {
        partition[i] = i % numPartitions;
    }
    for (; i <= vx; ++i) {
        partition[i] = i % numPartitions;
    }

    std::vector<std::ofstream> files(numPartitions);

    for (int i = 0; i < numPartitions; ++i) {
        std::string fileName ;
		if (i<10) {fileName = "../part_0" + std::to_string(i);}
		else {fileName = "../part_" + std::to_string(i);}
        files[i].open(fileName);
   }

    for (const auto& edge : edges) {
        u = edge[0];
        v = edge[1];
        w = edge[2];

        files[partition[u]] << u << " " << v << " " << w << " " << partition[v] << std::endl;
    }

    // Close the files
    for (int i = 0; i < numPartitions; ++i) {
        files[i].close();
    }
}

int main() {
    std::string graphFile = "input.txt";
    int numPartitions = 32;

    partitionGraph(graphFile, numPartitions);

    return 0;
}
 
