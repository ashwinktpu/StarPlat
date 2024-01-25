// metis_integration.h

#include <vector>
#include <iostream>
#include <metis.h>
#include <chrono>
#include <filesystem>

std::vector<int> partitionGraph(std::vector<int>& xadj, std::vector<int>& adjncy, std::vector<int>& weights, int numPartitions);
