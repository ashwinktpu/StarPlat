#include"test_dsl.h"

void Compute_SSSP(graph g,int src)
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
  int* dist=new int[g.num_nodes()];
  dist = new int[part_size];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
  }
  dist[src] = 0;
  int accum = 0;
  int* count = new int[part_size];
  for(int i0=0;i0<part_size;i0++)
    count[i0] = 0;
  for (int v = startv; v <= endv; v++) 
  {
    for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
    {
      int nbr = g.edgeList[edge] ;
      count[v]  = count[v]  + 1;
    }
  }
  for (int v = startv; v <= endv; v++) 
  {
    accum += count[v] ;
  }
  accum = all_reduce(world, accum, std::plus<int>());

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