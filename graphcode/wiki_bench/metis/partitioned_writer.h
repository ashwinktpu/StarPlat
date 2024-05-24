// partitioned_writer.h

#include <vector>
#include <string>

void writePartitionedGraph(const std::vector<std::tuple<int, int, int>>& edges, 
                           const std::vector<int>& partitionResult, 
                           int numPartitions, int offset, std::string outputFolderPath);
