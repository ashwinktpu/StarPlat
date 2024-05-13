#include"plusEqualTC.h"

void temp(int a, NodeProperty<int>& e, Graph& g, boost::mpi::communicator world )
{
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    int f = 0;
    int b = 2;
    f = ( f + b) ;
  }
  world.barrier();

  int f_temp = f;
  MPI_Allreduce(&f_temp,&f,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);



}
