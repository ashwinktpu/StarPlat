// graph_conversion.h

#include <vector>
#include <unordered_map>
#include <utility>

void convertToUndirected(const std::vector<std::tuple<int, int, int>>& edges, 
                         std::vector<std::unordered_map<int, int>>& undirectedGraph);
