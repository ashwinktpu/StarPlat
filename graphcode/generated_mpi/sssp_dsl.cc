#include"sssp_dsl.h"

void Compute_SSSP(graph g,int src)
{
  int my_rank,np,part_size,startv,endv;
  struct timeval start, end, start1, end1;
  long seconds,micros;
  mpi::communicator world;
  my_rank = world.rank();
  np = world.size();
  int max_degree;
  int *index,*rev_index, *weight,*edgeList;
  int *local_index,*local_rev_index, *local_weight,*local_edgeList;
  int dest_pro;
  if(my_rank == 0)
  {
    gettimeofday(&start, NULL);
    g.parseGraph();
    gettimeofday(&end, NULL);
    seconds = (end.tv_sec - start.tv_sec);
    micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("The graph loading time = %ld micro secs.\n",micros);
    max_degree = g.max_degree();
    weight = g.getEdgeLen();
    edgeList = g.getEdgeList();
    index = g.getIndexofNodes();
    rev_index = g.rev_indexofNodes;
    part_size = g.num_nodes()/np;
    MPI_Bcast (&max_degree,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    local_rev_index = new int[part_size+1];
    for(int i=0;i<part_size+1;i++) {
      local_index[i] = index[i];
      local_rev_index[i] = rev_index[i];
    }
    int num_ele = local_index[part_size]-local_index[0];
    local_weight = new int[num_ele];
    for(int i=0;i<num_ele;i++)
    local_weight[i] = weight[i];
    local_edgeList = new int[num_ele];
    for(int i=0;i<num_ele;i++)
    local_edgeList[i] = edgeList[i];
    for(int i=1;i<np;i++)
    {
      int pos = i*part_size;
      MPI_Send (index+pos,part_size+1,MPI_INT,i,0,MPI_COMM_WORLD);
      MPI_Send (rev_index+pos,part_size+1,MPI_INT,i,1,MPI_COMM_WORLD);
      int start = index[pos];
      int end = index[pos+part_size];
      int count_int = end - start;
      MPI_Send (weight+start,count_int,MPI_INT,i,2,MPI_COMM_WORLD);
      MPI_Send (edgeList+start,count_int,MPI_INT,i,3,MPI_COMM_WORLD);
    }
    delete [] weight;
    delete [] edgeList;
    delete [] index;
  }
  else
  {
    MPI_Bcast (&max_degree,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_rev_index = new int[part_size+1];
    MPI_Recv (local_rev_index,part_size+1,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    int num_ele = local_index[part_size]-local_index[0];
    local_weight = new int[num_ele];
    MPI_Recv (local_weight,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_edgeList = new int[num_ele];
    MPI_Recv (local_edgeList,num_ele,MPI_INT,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
  startv = my_rank*part_size;
  endv = startv+part_size-1;
  int* dist=new int[part_size];
  bool* modified=new bool[part_size];
  dist = new int[part_size];
  modified = new bool[part_size];
  for (int t = 0; t < part_size; t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  modified[src-startv] = true;
  dist[src-startv] = 0;
  bool finished = false;
  int num_iter=0;
  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  while ( is_finished(startv,endv,modified)  )
  {
    num_iter++;
    vector < map<int,int> > send_data(np);
    vector < map<int,int> > receive_data(np);
    vector < map<int,float> > send_data_float(np);
    vector < map<int,float> > receive_data_float(np);
    vector < map<int,double> > send_data_double(np);
    vector < map<int,double> > receive_data_double(np);
    std::map<int,int>::iterator itr;
    for (int v = startv; v <= endv; v++) 
    {
      if ( modified[v-startv] == true )
      {
        modified[v-startv] [v]  = false;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {
          int nbr = g.edgeList[edge] ;
          if(nbr >= startv && nbr <=endv)
          {
            int e = edge;
             int dist_new = dist[v-startv] + weight[e-startv];
            bool modified_new = true;
            if (dist[nbr-startv] > dist_new)
            {
              dist[nbr-startv] = dist_new;
              modified[nbr-startv] = modified_new;
            }
          }
          else
          {
            dest_pro = nbr / part_size;
            itr = send_data[dest_pro].find(nbr);
            int e = edge;
            if (itr != send_data[dest_pro].end())
              itr->second = min( send_data[dest_pro][nbr], dist[v-startv] + weight[e-startv]);
            else
              send_data[dest_pro][nbr] = dist[v-startv] + weight[e-startv];
          }
        }
      }
    }
    all_to_all(world, send_data, receive_data);
    int e = edge;
    for(int t=0;t<np;t++)
    {
      if(t != my_rank)
      {
        for (auto x : receive_data[t])
        {
          int dist_new = x.second;
          bool modified_new = true;
          int nbr = x.first;
          if (dist[nbr-startv] > dist_new)
          {
            dist[nbr-startv] = dist_new;
            modified[nbr-startv] = modified_new;
          }
        }
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    send_data.clear();
    receive_data.clear();
    send_data_float.clear();
    receive_data_float.clear();
    send_data_double.clear();
    receive_data_double.clear();
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