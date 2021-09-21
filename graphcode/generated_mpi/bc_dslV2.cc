#include"bc_dslV2.h"

void Compute_BC(graph g,std::vector<int> sourceSet)
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
  float* BC=new float[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    BC[t] = 0;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  for (int i = 0; i < sourceSet.size(); i++)
  {
    int src = sourceSet[i];
    float* sigma=new float[g.num_nodes()];
    float* delta=new float[g.num_nodes()];
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      delta[t] = 0;
    }
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      sigma[t] = 0;
    }
    sigma[src] = 1;
    int phase = 0 ;
    vector <int> active;
    vector<int> active_next;
    vector <vector<int>> p (g.num_nodes());
    int* d = new int[g.num_nodes()];
    for (int t = 0; t < g.num_nodes(); t++)
    {
      d[t] = -1;
    }
    active.push_back(src);
    d[src] = 0;
    while(is_finished(startv,endv,active))
    {
      vector < vector <float> > send_data(np);
      vector < vector <float> > receive_data(np);
      while (active.size() > 0)
      {
        int v = active.back();
        active.pop_back();
        if(v >=startv && v<= endv)
        {
          for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
          {
            int w = g.edgeList[edge] ;
            if(w >= startv && w <= endv)
            {
              if (d[w] < 0)
              {
                active_next.push_back(w);
                d[w] = d[v] + 1;
              }
              if (d[w] == d[v] + 1)
                {
                  p[w].push_back(v);
                  sigma[w] = sigma[w] + sigma[v];
                }
              }
              else
              {
                dest_pro = w / part_size;
                send_data[dest_pro].push_back(d[v]);
                send_data[dest_pro].push_back(w);
                send_data[dest_pro].push_back(v);
                send_data[dest_pro].push_back(sigma[v]);
              }
            }
          }
        }
        all_to_all(world, send_data, receive_data);
        for(int t=0;t<np;t++)
        {
          if(t != my_rank)
          {
            for (int x=0; x < receive_data[t].size();x+=4)
            {
              int d_v = receive_data[t][x];
              int w = receive_data[t][x+1];
              int v = receive_data[t][x+2];
              float sigma_v = receive_data[t][x+3];
              if (d[w] < 0 )
              {
                active_next.push_back(w);
                d[w] = d_v + 1;
              }
              if (d[w] == d_v+1)
              {
                p[w].push_back(v);
                sigma[w] = sigma[w] + sigma_v;
              }
            }
          }
        }
        active.swap(active_next);
        active_next.clear();
        MPI_Barrier(MPI_COMM_WORLD);
        send_data.clear();
        receive_data.clear();
        phase = phase + 1 ;
      }
      phase = phase -1 ;
      bool* modified = new bool[g.num_nodes()];
      for (int t = 0; t < g.num_nodes(); t++)
      {
        modified[t] = false;
      }
      MPI_Barrier(MPI_COMM_WORLD);
      while (phase > 0)
      {
        vector <vector <float> > send_data(np);
        vector <vector <float> > receive_data(np);
        for(int v=startv; v<=endv; v++)
        {
          if(d[v] == phase)
          {
            modified[v] = true;
            for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
            {
              int w = g.edgeList[edge] ;
              if(w >= startv && w <= endv)
              {
                if ( d[w]== d[v] + 1 )
                {
                  delta[v] = delta[v] + ( sigma[v] / sigma[w])  * ( 1 + delta[w]) ;
                }
              }
            }
          }
          if(d[v] == (phase+1))
          {
            for (int j=0;j<p[v].size();j++)
            {
              int w = p[v][j];
              if(!(w >= startv && w <= endv))
              {
                dest_pro = w / part_size;
                send_data[dest_pro].push_back(v);
                send_data[dest_pro].push_back(w);
                send_data[dest_pro].push_back(delta[v]);
                send_data[dest_pro].push_back(sigma[v]);
              }
            }
          }
        }
        all_to_all(world, send_data, receive_data);
        for(int t=0;t<np;t++)
        {
          if(t != my_rank)
          {
            for (int x=0; x < receive_data[t].size();x+=4)
            {
              int w = receive_data[t][x];
              int v = receive_data[t][x+1];
              float delta_w = receive_data[t][x+2];
              float sigma_w = receive_data[t][x+3];
              delta[v] = delta[v] + ( sigma[v] / sigma_w)  * ( 1 + delta_w) ;
            }
          }
        }
        for (int v=startv;v<=endv;v++)
        {
          if( v != src && modified[v] == true)
          {
            modified[v] = false;
            BC[v] = BC[v] + delta[v];
          }
        }
        phase--;
        MPI_Barrier(MPI_COMM_WORLD);
        send_data.clear();
        receive_data.clear();
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