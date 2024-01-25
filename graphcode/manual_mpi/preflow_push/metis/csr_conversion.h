// csr_conversion.h

#include <vector>
#include <unordered_map>

void convertToCSR(const std::vector<std::unordered_map<int, int>>& graph, 
                  std::vector<int>& xadj, 
                  std::vector<int>& adjncy,
                  std::vector<int>& weights);
