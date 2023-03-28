#include"triangle_counting_dsl.h"

auto Compute_TC(Graph& g, boost::mpi::communicator world )
{
  long triangle_count = 0;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    for (int u : g.getNeighbors(v)) 
    {
      if (u < v )
      {
        for (int w : g.getNeighbors(v)) 
        {
          if (w > v )
          {
            if (g.check_if_nbr(u, w) )
            {
              triangle_count = triangle_count + 1;
            }
          }
        }

      }
    }

  }
  world.barrier();

  long triangle_count_temp = triangle_count;
  MPI_Allreduce(&triangle_count_temp,&triangle_count,1,MPI_LONG,MPI_SUM,MPI_COMM_WORLD);

  return triangle_count;

}
