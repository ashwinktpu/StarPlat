#include"idxOpTest.h"

void indexTest(int a, int b, Container<int>& someArr, boost::mpi::communicator world )
{
  someArr.getValue (a) = b;

}
