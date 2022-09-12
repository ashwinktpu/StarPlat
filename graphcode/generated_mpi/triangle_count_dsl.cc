#include"triangle_count_dsl.h"

void Compute_TC(graph g)
{
  int my_rank,np,part_size,startv,endv;
  struct timeval start, end, start1, end1;
  long seconds,micros;
  mpi::communicator world;
  my_rank = world.rank();
  np = world.size();

  gettimeofday(&start1, NULL);
  g.parseGraph();
  gettimeofday(&end1, NULL);
  seconds = (end1.tv_sec - start1.tv_sec);
  micros = ((seconds * 1000000) + end1.tv_usec) - (start1.tv_usec);
  if(my_rank == 0)
  {
    printf("The graph loading time = %ld secs.\n",seconds);
  }
  int max_degree = g.max_degree();
  int *weight = g.getEdgeLen();

  part_size = g.num_nodes()/np;
  startv = my_rank*part_size;
  endv = startv + (part_size-1);

  int dest_pro;
  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  long triangle_count = 0;
  for (int v = startv; v <= endv; v++) 
  {
    for (int edge0 = g.indexofNodes[v]; edge0 < g.indexofNodes[v+1]; edge0++) 
    {
      int u = g.edgeList[edge0] ;
      if ( u < v )
      {
        for (int edge1 = g.indexofNodes[v]; edge1 < g.indexofNodes[v+1]; edge1++) 
        {
          int w = g.edgeList[edge1] ;
          if ( w > v )
          {
            if (g.check_if_nbr(u, w) ){
              triangle_count += 1;
            }
          }
        }
      }
    }
  }

  gettimeofday(&end, NULL);
  seconds = (end.tv_sec - start.tv_sec);
  micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
  if(my_rank==0)
  {
    printf("The iteration time = %ld micro secs.\n",micros);
    printf("The iteration time = %ld secs.\n",seconds);
  }
  MPI_Finalize();
}