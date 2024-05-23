#include"basicGetEdge.h"

void helloWorld(int a, int b, Graph& g, boost::mpi::communicator world )
{
  world.barrier();
  if ( world.rank () == g.get_node_owner (a) )
   { int vIdx = 0 ;
    for (int v:g.getNeighbors(a))
    {
      Edge e = g.get_edge(a, _t1);
    }

  }
  world.barrier ()


}
