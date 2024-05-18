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
  pageRank = new float[part_size];
  float num_nodes = _actual_num_nodes;
  for (int t = 0; t < part_size; t++) 
  {
    pageRank[t] = 1.000000 / num_nodes;
  }
  int iterCount = 0;
  float diff =  0.0 ;
  do
  {
    vector < vector <int> > send_data(np);
    vector < vector <int> > receive_data(np);
    vector < vector <float> > send_data_float(np);
    vector < vector <float> > receive_data_float(np);
    vector < vector <double> > send_data_double(np);
    vector < vector <double> > receive_data_double(np);
      diff = 0.000000;
    float* sum = new float[part_size];
    for(int i0=0;i0<part_size;i0++)
      sum[i0] = 0.000000;
    for (int v = startv; v <= endv; v++) 
    {
      for (int edge = local_rev_index[v-startv]; edge < local_rev_index[v-startv+1]; edge++) 
      {
        int nbr = local_srcList[edge] ;
        if (nbr >= startv && nbr <= endv)
        {
          sum[v-startv]  = sum[v-startv]  + pageRank[nbr-startv] / (local_index[nbr-startv+1]-local_index[nbr-startv]);
      }
    }
    for (int edge1 = local_index[v-startv]; edge1 < local_index[v-startv+1]; edge1++) 
    {
      int nbr = local_edgeList[edge1];
      if(!(nbr >= startv && nbr <= endv))
      {
        dest_pro = nbr / part_size;
        send_data[dest_pro].push_back(v);
        send_data[dest_pro].push_back(nbr);
        send_data[dest_pro].push_back(local_index[v-startv]);
        send_data[dest_pro].push_back(local_index[v-startv+1]);
        send_data_float[dest_pro].push_back(pageRank[v-startv]);
      }
    }
  }
  all_to_all(world, send_data_float, receive_data_float);
  all_to_all(world, send_data, receive_data);
  for(int t=0;t<np;t++)
  {
    if(t != my_rank)
    {
      for (int x=0; x < receive_data[t].size();x+=4)
      {
        int nbr = receive_data[t][x];
        int v = receive_data[t][x+1];
        int local_index_nbr = receive_data[t][x+2];
        int local_index_nbr_1 = receive_data[t][x+3];
        int y_ = x/4;
        int z_ = x/4;
        float pageRank_nbr = receive_data_float[t][y_+0];
        sum[v-startv]  = sum[v-startv]  + pageRank_nbr / (local_index_nbr_1 - local_index_nbr);
      }
    }
  }
  for (int v = startv; v <= endv; v++) 
  {
    float val = ( 1 - delta)  / num_nodes + delta * sum[v-startv] ;
    diff += std::fabs(val - pageRank[v-startv]);
    if(v >= startv && v <= endv)
      pageRank[v - startv]  = val;
  }
  diff = all_reduce(world, diff, std::plus<float>());
  iterCount++;
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
} while ( diff > beta && iterCount < maxIter);

gettimeofday(&end, NULL);
seconds = (end.tv_sec - start.tv_sec);
micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
if(my_rank==0)
{
printf("The iteration time = %ld micro secs.\n",micros);
printf("The iteration time = %ld secs.\n",seconds);
}
float* final_pageRank;
if (my_rank == 0)
{
  final_pageRank = new float [_num_nodes];
  gather(world, pageRank, part_size, final_pageRank, 0);
  for (int t = 0; t < _actual_num_nodes; t++)
    cout << "pageRank[" << t << "] = " << final_pageRank[t] << endl;
}
else
{
  gather(world, pageRank, part_size, final_pageRank, 0);
}
delete [] local_index;
delete [] local_rev_index;
delete [] weight;
delete [] local_edgeList;
delete [] local_srcList;
MPI_Finalize();
}