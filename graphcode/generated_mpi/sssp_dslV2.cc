#include"sssp_dslV2.h"

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
  int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  int num_iter=0;
  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  while ( is_finished(startv,endv,modified)  )
  {
    num_iter++;
    vector < map<int,int> > send_data(np);
    vector < map<int,int> > receive_data(np);
    std::map<int,int>::iterator itr;
    int dest_pro;
    for (int v = startv; v <= endv; v++) 
    {
      if ( modified [v] == true )
      {
        modified [v]  = false;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {
          int nbr = g.edgeList[edge] ;
          if(nbr >= startv && nbr <=endv)
          {
            int e = edge;
             int dist_new = dist[v] + weight[e];
            bool modified_new = true;
            if (dist[nbr] > dist_new)
            {
              dist[nbr] = dist_new;
              modified[nbr] = modified_new;
            }
          }
          else
          {
            dest_pro = nbr / part_size;
            itr = send_data[dest_pro].find(nbr);
            int e = edge;
            if (itr != send_data[dest_pro].end())
              itr->second = min( send_data[dest_pro][nbr], dist[v] + weight[e]);
            else
              send_data[dest_pro][nbr] = dist[v] + weight[e];
          }
        }
      }
    }
    all_to_all(world, send_data, receive_data);
    for(int t=0;t<np;t++)
    {
      if(t != my_rank)
      {
        for (auto x : receive_data[t])
        {
          int dist_new = x.second;
          bool modified_new = true;
          int nbr = x.first;
          if (dist[nbr] > dist_new)
          {
            dist[nbr] = dist_new;
            modified[nbr] = modified_new;
          }
        }
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    send_data.clear();
    receive_data.clear();
  }

  gettimeofday(&end, NULL);
  seconds = (end.tv_sec - start.tv_sec);
  micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
  if(my_rank==0)
  {
    printf("The number of iterations taken = %d\n",num_iter);
    printf("The iteration time = %ld micro secs.\n",micros);
    printf("The iteration time = %ld secs.\n",seconds);
  }
  MPI_Finalize();
}