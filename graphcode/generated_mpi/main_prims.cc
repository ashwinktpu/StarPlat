#include "mst_prims_new.h"
#include "../mpi_header/graph_mpi.h" 
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argv, char* argc[])
{
    if(argv < 2)
    {
        cout << "Usage: ./main_prims <graph_file>" << endl;
        return 0;
    }
    boost::mpi::environment env(argv, argc);
    boost::mpi::communicator world;
    
    auto start = high_resolution_clock::now();
    Graph graph(argc[1], world);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    if(world.rank() == 0) cout << "Time taken to read graph: " << duration.count() << " microseconds" << endl;

    world.barrier();
    
    long long int mst_weight = 0;
    int num_mst_nodes = 0;
    Compute_MST_Prims(graph, 0, num_mst_nodes, mst_weight, world);
    world.barrier();

    if(world.rank() == 0)
    {
        cout << "MST weight: " << mst_weight << endl;
        cout << "Number of nodes in MST: " << num_mst_nodes << endl;
    }
    return 0;
}