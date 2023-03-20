#include"bc_dslV2.h"

void Compute_BC(Graph& g, NodeProperty<float>& BC, std::set<int>& sourceSet, boost::mpi::communicator world )
{
  BC.attachToGraph(&g, (float)0);
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++)
  {
    int src = *itr;
    NodeProperty<float> sigma;
    NodeProperty<float> delta;
    delta.attachToGraph(&g, (float)0);
    sigma.attachToGraph(&g, (float)0);
    if(world.rank() == g.get_node_owner(src))
    {
      sigma.setValue(src,1);
    }
    g.create_bfs_dag(src);
    for(int phase=0;phase<g.num_bfs_phases();phase++)
    {
      for( int v : g.get_bfs_nodes_for_phase(phase) )
      {
        for (int w :g.get_bfs_children(v))
        {
          sigma.atomicAdd(w,sigma.getValue(v));
        }

      }
      world.barrier();
    }
    for(int phase=g.num_bfs_phases()-1;phase>0;phase--)
    {
      for(int v : g.get_bfs_nodes_for_phase(phase))
      {
        for (int w :g.get_bfs_children(v))
        {
          delta.atomicAdd(v,(sigma.getValue(v) / sigma.getValue(w)) * (1 + delta.getValue(w)));
        }

        BC.atomicAdd(v,delta.getValue(v));
      }
      world.barrier();
    }
  }


}

int main(int argc, char *argv[])
{
   
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    printf("program started\n"); 
    Graph graph(argv[1],world);
    world.barrier();

    //BC
    std::set<int> sourceSet; sourceSet.insert(0);
    NodeProperty<float> BC;
    Compute_BC(graph, BC, sourceSet, world);
    for(int i=graph.start_node() ;i<=graph.end_node();i++)
    {
        printf("%d %f\n", i, BC.getValue(i));
    }
    
    world.barrier();
    return 0;
}