#include"bc_dslV2.h"

void Compute_BC(graph g,float* BC,std::vector<int> sourceSet)
{
  int my_rank,np,part_size,startv,endv;
  struct timeval start, end, start1, end1;
  long seconds,micros;
  mpi::communicator world;
  my_rank = world.rank();
  np = world.size();
  int *index,*rev_index, *all_weight,*edgeList, *srcList;
  int *local_index,*local_rev_index, *weight,*local_edgeList, *local_srcList;
  int num_nodes, actual_num_nodes;
  int dest_pro;
  if(my_rank == 0)
  {
    gettimeofday(&start, NULL);
    g.parseGraph();
    num_nodes = g.num_nodes();
    actual_num_nodes = g.ori_num_nodes();
    all_weight = g.getEdgeLen();
    edgeList = g.getEdgeList();
    srcList = g.getSrcList();
    index = g.getIndexofNodes();
    rev_index = g.rev_indexofNodes;
    part_size = g.num_nodes()/np;
    MPI_Bcast (&num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&actual_num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    local_rev_index = new int[part_size+1];
    for(int i=0;i<part_size+1;i++) {
      local_index[i] = index[i];
      local_rev_index[i] = rev_index[i];
    }
    int num_ele = local_index[part_size]-local_index[0];
    weight = new int[num_ele];
    for(int i=0;i<num_ele;i++)
    weight[i] = all_weight[i];
    local_edgeList = new int[num_ele];
    for(int i=0;i<num_ele;i++)
    local_edgeList[i] = edgeList[i];
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
    MPI_Bcast (&num_nodes,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&actual_num_nodes,1,MPI_INT,0,MPI_COMM_WORLD); 
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
  BC = new float[part_size];
  for (int t = 0; t < part_size; t++) 
  {
    BC[t] = 0;
  }
  double* sigma=new double[part_size];
  float* delta=new float[part_size];
  for (int i = 0; i < sourceSet.size(); i++)
  {
    int src = sourceSet[i];
    for (int t = 0; t < part_size; t++) 
    {
      delta[t] = 0;
    }
    for (int t = 0; t < part_size; t++) 
    {
      sigma[t] = 0;
    }
    if(src >= startv && src <= endv)
      sigma[src - startv]  = 1;
    int phase = 0 ;
    vector <int> active;
    vector<int> active_next;
    vector <vector<int>> p (part_size);
    int* d = new int[part_size];
    for (int t = 0; t < part_size; t++)
    {
      d[t] = -1;
    }
    active.push_back(src);
    if(src >= startv && src <= endv)
    {
      d[src - startv] = 0;
    }
    while(is_finished(startv,endv,active))
    {
      vector < vector <int> > send_data(np);
      vector < vector <int> > receive_data(np);
      vector < vector <float> > send_data_float(np);
      vector < vector <float> > receive_data_float(np);
      vector < vector <double> > send_data_double(np);
      vector < vector <double> > receive_data_double(np);
      while (active.size() > 0)
      {
        int v = active.back();
        active.pop_back();
        if(v >=startv && v<= endv)
        {
          for (int j = local_index[v - startv]; j < local_index[v - startv + 1]; j++)
          {
            int w = local_edgeList[j];
            if(w >= startv && w <= endv)
            {
              if (d[w-startv] < 0)
              {
                active_next.push_back(w);
                d[w-startv] = d[v-startv] + 1;
              }
              if (d[w-startv] == d[v-startv] + 1)
                {
                  p[w-startv].push_back(v);
                  if(w >= startv && w <= endv)
                    sigma[w - startv]  = sigma[w-startv] + sigma[v-startv];
                }
              }
              else
              {
                dest_pro = w / part_size;
                send_data[dest_pro].push_back(d[v-startv]);
                send_data[dest_pro].push_back(w);
                send_data[dest_pro].push_back(v);
                send_data_double[dest_pro].push_back(sigma[v-startv]);
              }
            }
          }
        }
        all_to_all(world, send_data_double, receive_data_double);
        all_to_all(world, send_data, receive_data);
        for(int t=0;t<np;t++)
        {
          if(t != my_rank)
          {
            for (int x=0; x < receive_data[t].size();x+=3)
            {
              int d_v = receive_data[t][x];
              int w = receive_data[t][x+1];
              int v = receive_data[t][x+2];
              int y_ = x/3;
              int z_ = x/3;
              double sigma_v = receive_data_double[t][z_+0];
              if (d[w-startv] < 0 )
              {
                active_next.push_back(w);
                d[w-startv] = d_v + 1;
              }
              if (d[w-startv] == d_v+1)
              {
                p[w-startv].push_back(v);
                sigma[w-startv] = sigma[w-startv] + sigma_v;
              }
            }
          }
        }
        active.swap(active_next);
        active_next.clear();
        vector <int> ().swap(active_next);
        MPI_Barrier(MPI_COMM_WORLD);
        send_data.clear();
        vector <vector <int> >().swap(send_data);
        receive_data.clear();
        vector <vector <int> >().swap(receive_data);
        send_data_float.clear();
        vector <vector <float> >().swap(send_data_float);
        receive_data_float.clear();
        vector <vector <float> >().swap(receive_data_float);
        send_data_double.clear();
        vector <vector <double> >().swap(send_data_double);
        receive_data_double.clear();
        vector <vector <double> >().swap(receive_data_double);
        phase = phase + 1 ;
      }
      phase = phase -1 ;
      bool* modified = new bool[part_size];
      for (int t = 0; t < part_size; t++)
      {
        modified[t] = false;
      }
      MPI_Barrier(MPI_COMM_WORLD);
      while (phase > 0)
      {
        vector <vector <int> > send_data(np);
        vector <vector <int> > receive_data(np);
        vector <vector <float> > send_data_float(np);
        vector <vector <float> > receive_data_float(np);
        vector <vector <double> > send_data_double(np);
        vector <vector <double> > receive_data_double(np);
        for(int v=startv; v<=endv; v++)
        {
          if(d[v-startv] == phase)
          {
            modified[v-startv] = true;
            for (int j = local_index[v - startv]; j < local_index[v - startv + 1]; j++)
            {
              int w = local_edgeList[j];
              if(w >= startv && w <= endv)
              {
                if(d[w - startv] == d[v - startv] + 1)
                {
                    if(v >= startv && v <= endv)
                      delta[v - startv]  = delta[v-startv] + ( sigma[v-startv] / sigma[w-startv])  * ( 1 + delta[w-startv]) ;
                }
              }
            }
          }
          if(d[v-startv] == (phase+1))
          {
            for (int j=0;j<p[v-startv].size();j++)
            {
              int w = p[v-startv][j];
              if(!(w >= startv && w <= endv))
              {
                dest_pro = w / part_size;
                send_data[dest_pro].push_back(v);
                send_data[dest_pro].push_back(w);
                send_data_float[dest_pro].push_back(delta[v-startv]);
                send_data_double[dest_pro].push_back(sigma[v-startv]);
              }
            }
          }
        }
        all_to_all(world, send_data_float, receive_data_float);
        all_to_all(world, send_data_double, receive_data_double);
        all_to_all(world, send_data, receive_data);
        for(int t=0;t<np;t++)
        {
          if(t != my_rank)
          {
            for (int x=0; x < receive_data[t].size();x+=2)
            {
              int w = receive_data[t][x];
              int v = receive_data[t][x+1];
              int y_ = x/2;
              int z_ = x/2;
              float delta_w = receive_data_float[t][y_+0];
              double sigma_w = receive_data_double[t][z_+0];
              delta[v-startv] = delta[v-startv] + ( sigma[v-startv] / sigma_w)  * ( 1 + delta_w) ;
            }
          }
        }
        for (int v=startv;v<=endv;v++)
        {
          if( v != src && modified[v-startv] == true)
          {
            modified[v - startv] = false;
            if(v >= startv && v <= endv)
              BC[v - startv]  = BC[v-startv] + delta[v-startv];
          }
        }
        phase--;
        MPI_Barrier(MPI_COMM_WORLD);
        send_data.clear();
        vector <vector <int> >().swap(send_data);
        receive_data.clear();
        vector <vector <int> >().swap(receive_data);
        send_data_float.clear();
        vector <vector <float> >().swap(send_data_float);
        receive_data_float.clear();
        vector <vector <float> >().swap(receive_data_float);
        send_data_double.clear();
        vector <vector <double> >().swap(send_data_double);
        receive_data_double.clear();
        vector <vector <double> >().swap(receive_data_double);
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
    float* final_BC;
    if (my_rank == 0)
    {
        final_BC = new float [num_nodes];
        gather(world, BC, part_size, final_BC, 0);
        for (int t = 0; t < actual_num_nodes; t++)
          cout << "BC[" << t << "] = " << final_BC[t] << endl;
    }
    else
    {
        gather(world, BC, part_size, final_BC, 0);
    }
    MPI_Finalize();
}