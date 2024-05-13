#include"getV.h"

void testAssignment(Graph& g, int u, int t1, boost::mpi::communicator world )
{
  int v = g.get_other_vertex(u,t1);

}
