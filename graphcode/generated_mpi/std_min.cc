#include"std_min.h"

void helloWorld(NodeProperty<int>& x, int a, int b, Graph& g
  , boost::mpi::communicator world )
{
  int x = std::min(x.getValue(a),x.getValue(b));

}
