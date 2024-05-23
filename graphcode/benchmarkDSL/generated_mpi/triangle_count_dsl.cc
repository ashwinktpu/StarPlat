#include"triangle_count_dsl.h"

auto Compute_TC(graph g)
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
  long triangle_count = 0;
  vector < map<int,vector<int>> > send_data(np);
  vector < map<int,vector<int>> > receive_data(np);
  std::map<int,vector<int>>::iterator itr;
  for (int v = startv; v <= endv; v++) 
  {
    for (int edge0 = local_index[v - startv]; edge0 < local_index[v - startv + 1]; edge0++) 
    {
      int u = local_edgeList[edge0] ;
      if(u >= startv && u <=endv)
      {
        if ( u < v )
        {
          for (int edge1 = local_index[v - startv]; edge1 < local_index[v - startv + 1]; edge1++) 
          {
            int w = local_edgeList[edge1] ;
            if(w >= startv && w <=endv)
            {
              if ( w > v )
              {
                if (check_if_nbr(u, w, local_index, local_edgeList, startv) ){
                  triangle_count += std::fabs(1);
                }
              }
            }
            else
            {
              dest_pro = w / part_size;
              itr = send_data[dest_pro].find(w);
              vector <int> nbr_list;
              for (int t = local_index[v - startv]; t < local_index[v - startv +1]; t++)
                nbr_list.push_back(local_edgeList[t]);
              send_data[dest_pro][v] = nbr_list;
            }
          }
        }
      }
      else
      {
        dest_pro = u / part_size;
        itr = send_data[dest_pro].find(u);
        vector <int> nbr_list;
        for (int t = local_index[v - startv]; t < local_index[v - startv +1]; t++)
          nbr_list.push_back(local_edgeList[t]);
        send_data[dest_pro][v] = nbr_list;
      }
    }
  }
  all_to_all(world, send_data, receive_data);
  for (int v = startv; v <= endv; v++)
  {
    for (int edge2 = local_index[v - startv]; edge2 < local_index[v - startv + 1]; edge2++) 
    {
      int u = local_edgeList[edge2] ;
      vector <int> u_list;
      if(!(u >= startv && u <=endv))
      {
        int dest_pro = u/part_size;
        u_list = receive_data[dest_pro][u];
      }
      if ( u < v )
      {
        for (int edge3 = local_index[v - startv]; edge3 < local_index[v - startv + 1]; edge3++) 
        {
          int w = local_edgeList[edge3] ;
          vector <int> w_list;
          if(!(w >= startv && w <=endv))
          {
            int dest_pro = w/part_size;
            w_list = receive_data[dest_pro][w];
          }
          if ( w > v )
          {
            if ((u_list.size() > 0 && check_if_nbr_new(u,w,u_list,w_list)) || (w_list.size() > 0 && check_if_nbr_new(w,u,w_list,u_list))  ){
              triangle_count += std::fabs(1);
            }
          }
        }
      }
    }
  }
  triangle_count = all_reduce(world, triangle_count, std::plus<long>());
  return triangle_count;

  gettimeofday(&end, NULL);
  seconds = (end.tv_sec - start.tv_sec);
  micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
  if(my_rank==0)
  {
    printf("The iteration time = %ld micro secs.\n",micros);
    printf("The iteration time = %ld secs.\n",seconds);
  }
  delete [] local_index;
  delete [] local_rev_index;
  delete [] weight;
  delete [] local_edgeList;
  delete [] local_srcList;
}