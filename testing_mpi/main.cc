#include "../graphcode/generated_mpi/mst_prims.h"
#include "../mpi_header/graph_mpi.h" 

int main(int argv, char* argc[])
{
    boost::mpi::environment env(argv, argc);
    boost::mpi::communicator world;
    Graph graph(argc[1], world);
    world.barrier();
    Prims(graph, world);
    world.barrier();
    NodeProperty<int> minCost;
    NodeProperty<int> minEdge;
    NodeProperty<bool> visited;

    string info = "Rank " + to_string(world.rank()) + " " + "has nodes from " + to_string(graph.start_node()) + " to " + to_string(graph.end_node()) + "\n";
    for(int i=graph.start_node(); i<=graph.end_node(); i++)
    {
        info += "Node id: " + to_string(i) + \
                " minCost: " + to_string(minCost.getValue(i)) + \
                " minEdge: " + to_string(minEdge.getValue(i)) + \
                " visited: " + to_string(visited.getValue(i)) + "\n";
    }

    return 0;
}