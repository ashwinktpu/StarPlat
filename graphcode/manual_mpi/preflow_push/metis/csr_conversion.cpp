// csr_conversion.cpp

#include "csr_conversion.h"

void convertToCSR(const std::vector<std::unordered_map<int, int>>& graph, 
                  std::vector<int>& xadj, 
                  std::vector<int>& adjncy,
                  std::vector<int>& weights) {
    xadj.clear();
    adjncy.clear();
    weights.clear();

    int edgeCount = 0;
    xadj.push_back(edgeCount);

    for (const auto& neighbors : graph) {
        for (const auto& neighbor : neighbors) {
            adjncy.push_back(neighbor.first);
            weights.push_back(neighbor.second);
            edgeCount++;
        }
        xadj.push_back(edgeCount);
    }
}
