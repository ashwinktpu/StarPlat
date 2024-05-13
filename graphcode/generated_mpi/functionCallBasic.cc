#include"functionCallBasic.h"

void testFunc(Graph& g, int u, boost::mpi::communicator world )
{
  g.frontier_push(u, world);


}
