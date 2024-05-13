#include"basicFilter.h"

void do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world )
{
  NodeProperty<int> label;
  NodeProperty<int> excess;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (excess.getValue(v) > 0 && v != source && v != sink && label.getValue(v) < g.num_nodes( ) )
    {
      g.frontier_push(v, world);

    }
  }
  world.barrier();


  {
    while (g.frontier_pop( ) ){
      excess.setValue(a,1);
      excess.atomicAdd (u, 1);
      if (excess.getValue(a) > 0 && a != source && a != sink && label.getValue(a) < g.num_nodes( ) )
      {
        g.frontier_push(a, world);

      }
      if (excess.getValue(u) > 0 && u != source && u != sink && label.getValue(u) < g.num_nodes( ) )
      {
        g.frontier_push(u, world);

      }
    }
  }

}
