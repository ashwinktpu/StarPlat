#include"pageRankDSL.h"

void Compute_PR(graph g,float beta,float delta,int maxIter,
  float* pageRank)
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
  int local_ipDeg=0, global_ipDeg=0;
  for (int i=startv; i<=endv;i++)
  {
    for (int j = g.indexofNodes[i]; j<g.indexofNodes[i+1]; j++)
    {
      int nbr = g.edgeList[j];
      if(!(nbr >= startv && nbr <=endv))
        local_ipDeg++;
    }
  }

  all_reduce(world, local_ipDeg, global_ipDeg, mpi::maximum<int>());
  if(my_rank==0)
    printf("Global inter part degree %d\n",global_ipDeg);
  float num_nodes = g.ori_num_nodes();
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    pageRank[t] = 1 / num_nodes;
  }
  int iterCount = 0;
  float diff =  = 0.0 ;
  do
  {
    diff = 0.000000;
    for (int v = startv; v <= endv; v++) 
    {
      float sum = new float[g.num_nodes()];
      for(int i0=0;i0<g.num_nodes();i0++)
        sum[i0] = 0.000000;
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge++) 
      {
        int nbr = g.srcList[edge] ;
        if (nbr >= startv && nbr <= endv)
        {
          sum = sum + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
        }
      }
      for (int edge1 = g.indexofNodes[v]; edge1 < g.indexofNodes[v+1]; edge1++)
      {
        int out_nbr = g.edgeList[edge1];
        if(!(out_nbr >= startv && out_nbr <= endv))
        {
          dest_pro = out_nbr / part_size;
          send_data[dest_pro].push_back(v);
          send_data[dest_pro].push_back(out_nbr);
          send_data[dest_pro].push_back(pageRank[nbr]);
        }
      }
    }
    float val = ( 1 - delta)  / num_nodes + delta * sum;
    pageRank[v] = val;
  }
  all_to_all(world, send_data, receive_data);
  for(int t=0;t<np;t++)
  {
    if(t != my_rank)
    {
      for (auto x : receive_data[t])
      {
      }
    }
  }
} while ( diff > beta && iterCount < maxIter);

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