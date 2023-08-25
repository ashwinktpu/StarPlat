#include"../mpi_header/graph_mpi.h"
#include"sssp_dslV3.h"
// Include Timing Code
#include <chrono>
#include <iostream>

using namespace std::chrono;

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        std::cout << "Usage: " << argv[0] << " <graph_file>" << std::endl;
        exit(1);
    }

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    auto start = high_resolution_clock::now();
    
    Graph graph(argv[1],world);
    world.barrier();

    NodeProperty<int> dist;
    Compute_SSSP(graph, dist, graph.weights, 0,world);
    
    // for(int i=graph.start_node() ;i<=graph.end_node();i++)
    // {
    //     if(world.rank()==graph.get_node_owner(i))
    //     {
    //         std::cout<<"Node "<<i<<" has distance "<<dist.getValue(i)<<std::endl;
    //     }
    // }

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    
    world.barrier();

    if(world.rank()==0)
    {
        std::cout << "Time taken by function: "
            << duration.count() << " microseconds" << std::endl;
    }
    return 0;
}