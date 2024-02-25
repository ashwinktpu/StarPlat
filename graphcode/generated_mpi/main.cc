#include"../mpi_header/graph_mpi.h"
#include"PageRankDSLV2.h"
#include"triangle_counting_dsl.h"
#include"sssp_dslV3.h"
#include"bc_dslV2.h"
#include "push_relabel.dsl.h"

int main(int argc, char *argv[])
{
   
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    printf("program started\n"); 
 //   Graph graph(argv[1],world);
    Graph residual_graph(argv[1],world, 1);
    world.barrier();
    printf ("residual graph csr\n") ;
    residual_graph.print_csr () ;
    world.barrier () ;

    NodeProperty<int> label, excess, curr_edge ;
    EdgeProperty<int> residue ;
    world.barrier () ;
    // Triangle Counting
    // need to add print statement in generated code to check the value of triangle count
    do_max_flow(residual_graph,0,1, label, excess, curr_edge, residue,  world);
    

//    printf ("residual graph csr post max flow\n") ;
 //   residual_graph.print_csr () ;


    

    // SSSP 
    /*NodeProperty<int> dist;
    Compute_SSSP(graph, dist, graph.weights, 0,world);
    
    for(int i=graph.start_node() ;i<=graph.end_node();i++)
    {
        printf("%d %d\n", i, dist.getValue(i));
    }*/
    



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
    
    world.barrier();
    return 0;
}
