#include"getVInFA.h"

void testAssignment(Graph& g, int u, int t1, boost::mpi::communicator world )
{
  world.barrier();
  if ( u != -1 && world.rank () == g.get_node_owner (u) )
   { for (int v:g.getNeighbors(u))
    {
      v_leader_rank = world.rank();
      v = g.get_other_vertex(u, t1);
    }

  }
  world.barrier () ;


}
