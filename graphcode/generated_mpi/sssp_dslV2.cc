#include"sssp_dslV2.h"

void Compute_SSSP(graph g,int* dist,int* weight,int src
)
{
  int my_rank,np,part_size,startv,endv;
  struct timeval start, end, start1, end1;
  long seconds,micros;
  mpi::communicator world;
  my_rank = world.rank();
  np = world.size();
  int *index,*rev_index, *all_weight,*edgeList, *srcList;
  int *local_index,*local_rev_index, *weight,*local_edgeList, *local_srcList;
  int _num_nodes, _actual_num_nodes;
  int dest_pro;
  if(my_rank == 0)
  {
    gettimeofday(&start, NULL);
    g.parseGraph();
    _num_nodes = g.num_nodes();
    _actual_num_nodes = g.ori_num_nodes();
    all_weight = g.getEdgeLen();
    edgeList = g.getEdgeList();
    srcList = g.getSrcList();
    index = g.getIndexofNodes();
    rev_index = g.getRevIndexofNodes();
    part_size = g.num_nodes()/np;
    MPI_Bcast (&_num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&_actual_num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    local_rev_index = new int[part_size+1];
    for(int i=0;i<part_size+1;i++) {
      local_index[i] = index[i];
      local_rev_index[i] = rev_index[i];
    }
    int num_ele = local_index[part_size]-local_index[0];
    weight = new int[num_ele];
    local_edgeList = new int[num_ele];
    for(int i=0;i<num_ele;i++)
    {
        weight[i] = all_weight[i];
        local_edgeList[i] = edgeList[i];
    }
    num_ele = local_rev_index[part_size]-local_rev_index[0];
    local_srcList = new int[num_ele];
    for(int i=0;i<num_ele;i++)
      local_srcList[i] = srcList[i];
    for(int i=1;i<np;i++)
    {
      int pos = i*part_size;
      MPI_Send (index+pos,part_size+1,MPI_INT,i,0,MPI_COMM_WORLD);
      MPI_Send (rev_index+pos,part_size+1,MPI_INT,i,1,MPI_COMM_WORLD);
      int start = index[pos];
      int end = index[pos+part_size];
      int count_int = end - start;
      MPI_Send (all_weight+start,count_int,MPI_INT,i,2,MPI_COMM_WORLD);
      MPI_Send (edgeList+start,count_int,MPI_INT,i,3,MPI_COMM_WORLD);
      start = rev_index[pos];
      end = rev_index[pos+part_size];
      count_int = end - start;
      MPI_Send (srcList+start,count_int,MPI_INT,i,4,MPI_COMM_WORLD);
    }
    delete [] all_weight;
    delete [] edgeList;
    delete [] index;
    delete [] rev_index;
    delete [] srcList;
    gettimeofday(&end, NULL);
    seconds = (end.tv_sec - start.tv_sec);
    micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("The graph loading & distribution time = %ld secs.\n",seconds);
  }
  else
  {
    MPI_Bcast (&_num_nodes,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&_actual_num_nodes,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_rev_index = new int[part_size+1];
    MPI_Recv (local_rev_index,part_size+1,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    int num_ele = local_index[part_size]-local_index[0];
    weight = new int[num_ele];
    MPI_Recv (weight,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_edgeList = new int[num_ele];
    MPI_Recv (local_edgeList,num_ele,MPI_INT,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    num_ele = local_rev_index[part_size]-local_rev_index[0];
    local_srcList = new int[num_ele];
    MPI_Recv (local_srcList,num_ele,MPI_INT,0,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    int begin = local_index[0];
    for (int i = 0; i < part_size+1; i++)
      local_index[i] = local_index[i] - begin;
    begin = local_rev_index[0];
    for (int i = 0; i < part_size+1; i++)
      local_rev_index[i] = local_rev_index[i] - begin;
  }
  startv = my_rank*part_size;
  endv = startv+part_size-1;
  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  dist = new int[part_size];
  weight = new int[part_size];
  bool* modified=new bool[part_size];
  for (int t = 0; t < part_size; t++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  if(src >= startv && src <= endv)
    modified[src - startv]  = true;
  if(src >= startv && src <= endv)
    dist[src - startv]  = 0;
  bool finished = false;
  int num_iter=0;
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
      if ( modified [v - startv] == true )
      {
        modified [v - startv]  = false;
        for (int edge0 = local_index[v - startv]; edge0 < local_index[v - startv + 1]; edge0++) 
        {
          int nbr = local_edgeList[edge0] ;
          if(nbr >= startv && nbr <=endv)
          {
            int e = edge0 + startv;
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
            int e = edge0 + startv;
            if (itr != send_data[dest_pro].end())
              itr->second = min( send_data[dest_pro][nbr], dist[v-startv] + weight[e-startv]);
            else
              send_data[dest_pro][nbr] = dist[v-startv] + weight[e-startv];
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
    vector < map<int,int> >().swap(send_data);
    receive_data.clear();
    vector < map<int,int> >().swap(receive_data);
    send_data_float.clear();
    vector < map<int,float> >().swap(send_data_float);
    receive_data_float.clear();
    vector < map<int,float> >().swap(receive_data_float);
    send_data_double.clear();
    vector < map<int,double> >().swap(send_data_double);
    receive_data_double.clear();
    vector < map<int,double> >().swap(receive_data_double);
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
  int* final_dist;
  if (my_rank == 0)
  {
      final_dist = new int [_num_nodes];
      gather(world, dist, part_size, final_dist, 0);
      for (int t = 0; t < _actual_num_nodes; t++)
        cout << "dist[" << t << "] = " << final_dist[t] << endl;
  }
  else
  {
      gather(world, dist, part_size, final_dist, 0);
  }
  int* final_weight;
  if (my_rank == 0)
  {
      final_weight = new int [_num_nodes];
      gather(world, weight, part_size, final_weight, 0);
      for (int t = 0; t < _actual_num_nodes; t++)
        cout << "weight[" << t << "] = " << final_weight[t] << endl;
  }
  else
  {
      gather(world, weight, part_size, final_weight, 0);
  }
  delete [] local_index;
  delete [] local_rev_index;
  delete [] weight;
  delete [] local_edgeList;
  delete [] local_srcList;
  MPI_Finalize();
}