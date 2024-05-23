#include"testPlusEqual.dsl.h"

void test(int a, int b, NodeProperty<int>& someProperty, boost::mpi::communicator world )
{
  someProperty.getValue(a) = someProperty.getValue(a)+ 1;

}
