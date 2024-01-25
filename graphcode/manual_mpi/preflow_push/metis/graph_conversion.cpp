// graph_conversion.cpp

#include "graph_conversion.h"

void convertToUndirected(const std::vector<std::tuple<int, int, int>>& edges, 
                         std::vector<std::unordered_map<int, int>>& undirectedGraph) {
    for (const auto& edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int weight = std::get<2>(edge);

        while (undirectedGraph.size() <= std::max(u, v)) {
            undirectedGraph.emplace_back();
        }

        undirectedGraph[u][v] = weight;
        undirectedGraph[v][u] = weight; // Assuming the weight is the same in both directions
    }
}
