#include"basicGetEdge.h"

void helloWorld(int a, int b, Graph& g, boost::mpi::communicator world )
{
  world.barrier();
  int _t1 = 0 ;

  if ( a != -1 && world.rank () == g.get_node_owner (a) )
   { for (int v:g.getNeighbors(a))
    {
      v_leader_rank = world.rank();
      v = g.get_other_vertex(a, _t1);
      Edge e = g.get_edge_i(a, _t1);
      _t1++;
    }

  }
  world.barrier () ;


}
