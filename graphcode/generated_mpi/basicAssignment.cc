#include"basicAssignment.h"

void tempFunc(int a, NodeProperty<int>& e, Graph& g, boost::mpi::communicator world )
{
  e.setValue(a,1);

}
