#include"../mpi_header/graph_mpi.h"
#include"PageRankDSLV2.h"
#include"triangle_counting_dsl.h"
#include"sssp_dslV3.h"
#include"sssp_dslV2.h"
#include"bc_dslV2.h"

int main(int argc, char *argv[])
{
   
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    //printf("program started\n"); 
    Graph graph(argv[1], world);
    world.barrier();

    
    // Triangle Counting
    //need to add print statement in generated code to check the value of triangle count
    
    // Compute_TC(graph, world );
    

    

    // SSSP 
    /*NodeProperty<int> dist;
    Compute_SSSP(graph, dist, graph.weights, 0,world);
    
    for(int i=graph.start_node() ;i<=graph.end_node();i++)
    {
        printf("%d %d\n", i, dist.getValue(i));
    }*/

    // TODO: sssp DSLV2
    // int src = 0;
    // NodeProperty<int> dist;
    // Compute_SSSP(graph, dist, graph.weights, 0, world);

    //PageRank
    /*float beta = 0.01;
    float delta = 0.85;
    int maxIter = 100;
    NodeProperty<float> pageRank;
    Compute_PR(graph, beta, delta, maxIter, pageRank, world);
    for(int i=graph.start_node() ;i<=graph.end_node();i++)
    {
        printf("%d %f\n", i, pageRank.getValue(i));
    }*/
    

    //BC
    /*std::set<int> sourceSet; sourceSet.insert(0);
    NodeProperty<float> BC;
    Compute_BC(graph, BC, sourceSet, world);
    for(int i=graph.start_node() ;i<=graph.end_node();i++)
    {
        printf("%d %f\n", i, BC.getValue(i));
    }*/
    return 0;
}
