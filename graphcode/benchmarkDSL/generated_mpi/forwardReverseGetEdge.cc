#include"forwardReverseGetEdge.h"

void helloWorld(int a, int b, Graph& g, boost::mpi::communicator world )
{
  world.barrier();
  int _t1 = -1 ;

  if ( world.rank () == g.get_node_owner (a) )
   { for (int v:g.getNeighbors(a))
    _t1 ++ ;

    {
      Edge e1 = g.get_edge_i(a, _t1);
      Edge e2 = g.get_edge_r_i(_t1, a);
    }

  }
  world.barrier ()


}
