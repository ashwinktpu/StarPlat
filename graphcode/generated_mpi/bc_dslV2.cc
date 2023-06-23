#include"bc_dslV2.h"

void Compute_BC(Graph& g, NodeProperty<float>& BC, std::set<int>& sourceSet, boost::mpi::communicator world )
{
  BC.attachToGraph(&g, (float)0);
  NodeProperty<float> sigma;
  NodeProperty<float> delta;
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++)
  {
    int src = *itr;
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
          sigma.atomicAdd(v,sigma.getValue(w));
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
