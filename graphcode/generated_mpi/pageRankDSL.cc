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
  float diff =  0.0 ;
  do
  {
    vector < vector <float> > send_data(np);
    vector < vector <float> > receive_data(np);
    diff = 0.000000;
    float* sum = new float[g.num_nodes()];
    for(int i0=0;i0<g.num_nodes();i0++)
      sum[i0] = 0.000000;
    for (int v = startv; v <= endv; v++) 
    {
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge++) 
      {
        int nbr = g.srcList[edge] ;
        if (nbr >= startv && nbr <= endv)
        {
          sum[v]  = sum[v]  + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
        }
      }
      for (int edge1 = g.indexofNodes[v]; edge1 < g.indexofNodes[v+1]; edge1++)
      {
        int nbr = g.edgeList[edge1];
        if(!(nbr >= startv && nbr <= endv))
        {
          dest_pro = nbr / part_size;
          send_data[dest_pro].push_back(v);
          send_data[dest_pro].push_back(nbr);
          send_data[dest_pro].push_back(pageRank[nbr]);
        }
      }
    }
    all_to_all(world, send_data, receive_data);
    for(int t=0;t<np;t++)
    {
      if(t != my_rank)
      {
        for (int x=0; x < receive_data[t].size();x+=3)
        {
          int nbr = receive_data[t][x];
          int v = receive_data[t][x+1];
          float pageRank_nbr = receive_data[t][x+2];
          sum[v]  = sum[v]  + pageRank_nbr / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
        }
      }
    }
    for (int v = startv; v <= endv; v++) 
    {
      float val = ( 1 - delta)  / num_nodes + delta * sum[v] ;
      diff += val - pageRank[v];
      pageRank[v] = val;
    }
    iterCount++;
    diff = all_reduce(world, diff, std::plus<float>());
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