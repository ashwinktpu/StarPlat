#include"sssp_dslV2.h"
 
void Compute_SSSP(graph g,int*& dist,int*& parent, int src)
{
  int my_rank,np,part_size,startv,endv;

  mpi::communicator world;
  my_rank = world.rank();
  np = world.size();

  int max_degree,num_nodes;
  int *index,*weight,*edgeList;
  int *local_index,*local_weight,*local_edgeList;

  if(my_rank == 0)
  {
    g.parseGraph();

  	max_degree=g.max_degree();
  	weight = g.getEdgeLen();
    edgeList = g.getEdgeList();
  	index = g.getIndexofNodes();
    num_nodes = g.num_nodes();
    part_size = g.num_nodes()/np;
    MPI_Bcast (&max_degree,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    for(int i=0;i<part_size+1;i++)
      local_index[i] = index[i];

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
      int start = index[pos];
      int end = index[pos+part_size];
      int count = end - start;
      MPI_Send (weight+start,count,MPI_INT,i,1,MPI_COMM_WORLD);
      MPI_Send (edgeList+start,count,MPI_INT,i,2,MPI_COMM_WORLD);
    }
    delete [] weight;
    delete [] edgeList;
    delete [] index;
  }
  else
  {
    MPI_Bcast (&max_degree,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&num_nodes,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    int num_ele = local_index[part_size]-local_index[0];
    local_weight = new int[num_ele];
    MPI_Recv (local_weight,num_ele,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_edgeList = new int[num_ele];
    MPI_Recv (local_edgeList,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
  startv = my_rank*part_size;
  endv = startv+part_size-1;

  int dest_pro;
  // TODO check correctness of below code
  // dynamic alloc of mem inside a function
  dist = new int[part_size];
  parent = new int[part_size];
  bool* modified=new bool[part_size];
  bool* modified_nxt=new bool[part_size];

  for (int t = 0; t < part_size; t ++) 
  {
    dist[t] = INT_MAX;
    parent[t] = -1;
    modified[t] = false;
    modified_nxt[t] = false;
  }
  if(src >= startv && src <=endv) {
    modified[src-startv] = true;
    dist[src-startv] = 0;
  }
  bool finished = false;
  int num_iter=0;
  MPI_Barrier(MPI_COMM_WORLD);
  while ( is_finished(startv,endv,modified)  )
  {
    num_iter++;
    vector < map<int, pair<int, int> > > send_data(np);
    vector < map<int, pair<int, int> > > receive_data(np);
    std::map<int, pair<int, int> >::iterator itr;
    for (int v = startv; v <= endv; v++) 
    {
      if ( modified [v-startv] == true )
      {
        modified [v-startv] = false;
        // for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        // {
        int local_v = v - my_rank*part_size; 
          //printf("[%d] my globalv = %d, localv = %d\n",my_rank,v,local_v);
        for (int j = 0; j < local_index[local_v+1]-local_index[local_v]; j++)
        {
          int begin = local_index[0];
          int nbr = local_edgeList[local_index[local_v]-begin+j];
          int eweight = local_weight[local_index[local_v]-begin+j];
          if(nbr >= startv && nbr <=endv)
          {
            // int e = edge;
            int dist_new = dist[v-startv] + local_weight[local_index[local_v]-begin+j];
            bool modified_new = true;
            int oldValue = dist[nbr-startv];
            if (dist[nbr-startv] > dist_new)
            {
              dist[nbr-startv] = dist_new;
              parent[nbr-startv] = v;
            }
            if(oldValue > dist[nbr-startv])
            {
                modified_nxt[nbr-startv] = modified_new;
                finished = false;
            }
          }
          else
          {
            dest_pro = nbr / part_size;
            itr = send_data[dest_pro].find(nbr);
            if (itr != send_data[dest_pro].end()) {
              int temp_dist = send_data[dest_pro][nbr].first;
              if(temp_dist > dist[v-startv] + eweight) {
                // send_data[dest_pro][nbr] = {dist[v-startv] + eweight, v};
                itr->second = {dist[v-startv] + eweight, v};
              }
            }
            else
              send_data[dest_pro][nbr] = {dist[v-startv] + eweight, v};
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
          int dist_new = x.second.first;
          int nbr_v = x.second.second;
          bool modified_new = true;
          int nbr = x.first;
          if (dist[nbr-startv] > dist_new)
          {
            dist[nbr-startv] = dist_new;
            parent[nbr-startv] = nbr_v;
            modified_nxt[nbr-startv] = modified_new;
          }
        }
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int v = 0; v < part_size; v ++) 
    { 
        modified[v] =  modified_nxt[v];
        modified_nxt[v] = false;
    }
    send_data.clear();
    receive_data.clear();
  }
  delete modified;
  delete modified_nxt;
}

void Compute_dynamicSSSPAdd(graph& g, int* dist, int* parent, bool* modified)
{
  int my_rank,np,part_size,startv,endv;

  mpi::communicator world;
  my_rank = world.rank();
  np = world.size();

  int max_degree,num_nodes;
  int *index,*weight,*edgeList;
  int *local_index,*local_weight,*local_edgeList;

  if(my_rank == 0)
  {
  	max_degree=g.max_degree();
  	weight = g.getEdgeLen();
    edgeList = g.getEdgeList();
  	index = g.getIndexofNodes();
    num_nodes = g.num_nodes();
    part_size = g.num_nodes()/np;
    MPI_Bcast (&max_degree,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    for(int i=0;i<part_size+1;i++)
      local_index[i] = index[i];

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
      int start = index[pos];
      int end = index[pos+part_size];
      int count = end - start;
      MPI_Send (weight+start,count,MPI_INT,i,1,MPI_COMM_WORLD);
      MPI_Send (edgeList+start,count,MPI_INT,i,2,MPI_COMM_WORLD);
    }
    delete [] weight;
    delete [] edgeList;
    delete [] index;
  }
  else
  {
    MPI_Bcast (&max_degree,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&num_nodes,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    int num_ele = local_index[part_size]-local_index[0];
    local_weight = new int[num_ele];
    MPI_Recv (local_weight,num_ele,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_edgeList = new int[num_ele];
    MPI_Recv (local_edgeList,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
  startv = my_rank*part_size;
  endv = startv+part_size-1;

  int dest_pro;
  bool* modified_nxt=new bool[part_size];

  for (int t = 0; t < part_size; t ++) 
  {
    modified_nxt[t] = false;
  }
  bool finished = false;
  int num_iter=0;

  MPI_Barrier(MPI_COMM_WORLD);
  
  while ( is_finished(startv,endv,modified)  )
  {
    num_iter++;
    vector < map<int, pair<int, int> > > send_data(np);
    vector < map<int, pair<int, int> > > receive_data(np);
    std::map<int, pair<int, int> >::iterator itr;
    for (int v = startv; v <= endv; v++) 
    {
      if ( modified [v-startv] == true )
      {
        modified [v-startv] = false;
        // for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        // {
        int local_v = v - my_rank*part_size; 
          //printf("[%d] my globalv = %d, localv = %d\n",my_rank,v,local_v);
        for (int j = 0; j < local_index[local_v+1]-local_index[local_v]; j++)
        {
          int begin = local_index[0];
          int nbr = local_edgeList[local_index[local_v]-begin+j];
          int eweight = local_weight[local_index[local_v]-begin+j];
          if(nbr >= startv && nbr <=endv)
          {
            // int e = edge;
            int dist_new = dist[v-startv] + local_weight[local_index[local_v]-begin+j];
            bool modified_new = true;
            int oldValue = dist[nbr-startv];
            if (dist[nbr-startv] > dist_new)
            {
              dist[nbr-startv] = dist_new;
              parent[nbr-startv] = v;
              modified_nxt[nbr-startv] = modified_new;
              finished = false;
            }
          }
          else
          {
            dest_pro = nbr / part_size;
            itr = send_data[dest_pro].find(nbr);
            if (itr != send_data[dest_pro].end()) {
              int temp_dist = send_data[dest_pro][nbr].first;
              if(temp_dist > dist[v-startv] + eweight) {
                itr->second = {dist[v-startv] + eweight, v};
              }
            }
            else
              send_data[dest_pro][nbr] = {dist[v-startv] + eweight, v};
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
          int dist_new = x.second.first;
          int nbr_v = x.second.second;
          bool modified_new = true;
          int nbr = x.first;
          if (dist[nbr-startv] > dist_new)
          {
            dist[nbr-startv] = dist_new;
            parent[nbr-startv] = nbr_v;
            modified_nxt[nbr-startv] = modified_new;
          }
        }
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int v = 0; v < part_size; v ++) 
    { 
        modified[v] =  modified_nxt[v];
        modified_nxt[v] = false;
    }
    send_data.clear();
    receive_data.clear();
  }
  delete modified_nxt;
}

// void Compute_dynamicSSSPDel(graph& g, int* dist, int* parent, bool* modified)
// {
//   int my_rank,np,part_size,startv,endv;

//   mpi::communicator world;
//   my_rank = world.rank();
//   np = world.size();

//   int max_degree,num_nodes;
//   int *index,*weight,*edgeList;
//   int *local_index,*local_weight,*local_edgeList;

//   if(my_rank == 0)
//   {
//     max_degree=g.max_degree();
//     weight = g.getEdgeLen();
//     edgeList = g.getEdgeList();
//     index = g.getIndexofNodes();
//     num_nodes = g.num_nodes();
//     part_size = g.num_nodes()/np;
//     MPI_Bcast (&max_degree,1,MPI_INT,my_rank,MPI_COMM_WORLD);
//     MPI_Bcast (&num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
//     MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
//     local_index = new int[part_size+1];
//     for(int i=0;i<part_size+1;i++)
//       local_index[i] = index[i];

//     int num_ele = local_index[part_size]-local_index[0];
//     local_weight = new int[num_ele];
//     for(int i=0;i<num_ele;i++)
//       local_weight[i] = weight[i];
//     local_edgeList = new int[num_ele];
//     for(int i=0;i<num_ele;i++)
//       local_edgeList[i] = edgeList[i];

//     for(int i=1;i<np;i++)
//     {
//       int pos = i*part_size;
//       MPI_Send (index+pos,part_size+1,MPI_INT,i,0,MPI_COMM_WORLD);
//       int start = index[pos];
//       int end = index[pos+part_size];
//       int count = end - start;
//       MPI_Send (weight+start,count,MPI_INT,i,1,MPI_COMM_WORLD);
//       MPI_Send (edgeList+start,count,MPI_INT,i,2,MPI_COMM_WORLD);
//     }
//     delete [] weight;
//     delete [] edgeList;
//     delete [] index;
//   }
//   else
//   {
//     MPI_Bcast (&max_degree,1,MPI_INT,0,MPI_COMM_WORLD); 
//     MPI_Bcast (&num_nodes,1,MPI_INT,0,MPI_COMM_WORLD);
//     MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);
//     local_index = new int[part_size+1];
//     MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

//     int num_ele = local_index[part_size]-local_index[0];
//     local_weight = new int[num_ele];
//     MPI_Recv (local_weight,num_ele,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//     local_edgeList = new int[num_ele];
//     MPI_Recv (local_edgeList,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//   }
//   startv = my_rank*part_size;
//   endv = startv+part_size-1;

//   int dest_pro;
//   bool* modified_nxt=new bool[part_size];
//   for (int t = 0; t < part_size; t ++) 
//   {
//     modified_nxt[t] = false;
//   }

//   bool finished = false;
//   int num_iter=0;

//   MPI_Barrier(MPI_COMM_WORLD);
  
//   while ( is_finished(startv,endv,modified)) {
//     for (int v = startv; v <= endv; v++) {
//       if (!modified[v-startv]) {
//         int parent_v = parent[v-startv];
//         if(parent_v >= startv && parent_v <= endv) {
//           if(parent_v > -1 && modified[parent_v-startv]) {
//             dist[v-startv] = INT_MAX/2;
//             modified[v-startv] = true;
//           }
//         } else {
//           dest_pro = parent_v / part_size;
//           itr = send_data[dest_pro].find(parent_v);
//           if (itr != send_data[dest_pro].end())
//             itr->second = min( send_data[dest_pro][parent_v], {0, v}]);
//           else
//             send_data[dest_pro][parent_v] = {0, v};
//         }
//       }
//     }
//     all_to_all(world, send_data, receive_data);
//     send_data.clear();
//     for(int t=0;t<np;t++)
//     {
//       if(t != my_rank)
//       {
//         for (auto x : receive_data[t])
//         {
//           int nbr_v = x.second.second;
//           int curr_v = x.first;
//           bool curr_modified = modified[curr_v - startv];
//           dest_pro = nbr_v/part_size;
//           send_data[dest_pro][nbr_v] = {curr_v, curr_modified};
//         }
//       }
//     }
//     all_to_all(world, send_data, receive_data);
//     for(int t=0;t<np;t++)
//     {
//       if(t != my_rank)
//       {
//         for (auto x : receive_data[t])
//         {
//           int curr_v = x.first;
//           int parent_v = x.second.first;
//           bool parent_modified = x.second.second;
//           if(parent_v > -1 && parent_modified) {
//             dist[curr_v-startv] = INT_MAX/2;
//             modified[curr_v-startv] = true;
//           }
//         }
//       }
//     }
//   }

//   MPI_Barrier(MPI_COMM_WORLD);

//   while ( is_finished(startv,endv,modified)  )
//   {
//     num_iter++;
//     vector < map<int, pair<int, int> > > send_data(np);
//     vector < map<int, pair<int, int> > > receive_data(np);
//     std::map<int, pair<int, int> >::iterator itr;
//     for (int v = startv; v <= endv; v++) 
//     {
//       if ( modified [v-startv] == true )
//       {
//         // for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
//         // {
//         // TODO replace with g.getInNeighbors() alternative..
//         int local_v = v - my_rank*part_size;
//         for (int j = 0; j < local_index[local_v+1]-local_index[local_v]; j++)
//         {
//           int begin = local_index[0];
//           int nbr = local_edgeList[local_index[local_v]-begin+j];
//           int eweight = local_weight[local_index[local_v]-begin+j];
//           if(nbr >= startv && nbr <=endv)
//           {
//             // int e = edge;
//             int dist_new = dist[v-startv] + eweight;
//             bool modified_new = true;
//             int oldValue = dist[nbr-startv];
//             if (dist[nbr-startv] > dist_new)
//             {
//               dist[nbr-startv] = dist_new;
//               parent[nbr-startv] = v;
//             }
//             if(oldValue > dist[nbr-startv])
//             {
//                 modified_nxt[nbr-startv] = modified_new;
//                 finished = false;
//             }
//           }
//           else
//           {
//             dest_pro = nbr / part_size;
//             itr = send_data[dest_pro].find(nbr);
//             if (itr != send_data[dest_pro].end()) {
//               int temp_dist = send_data[dest_pro][nbr].first;
//               if(temp_dist > dist[v-startv] + eweight) {
//                 // send_data[dest_pro][nbr] = {dist[v-startv] + eweight, v};
//                 itr->second = {dist[v-startv] + eweight, v};
//               }
//             }
//             else
//               send_data[dest_pro][nbr] = {dist[v-startv] + eweight, v};
//           }
//         }
//       }
//     }
//     all_to_all(world, send_data, receive_data);
//     for(int t=0;t<np;t++)
//     {
//       if(t != my_rank)
//       {
//         for (auto x : receive_data[t])
//         {
//           int dist_new = x.second.first;
//           int nbr_v = x.second.second;
//           bool modified_new = true;
//           int nbr = x.first;
//           if (dist[nbr-startv] > dist_new)
//           {
//             dist[nbr-startv] = dist_new;
//             parent[nbr-startv] = nbr_v;
//             modified_nxt[nbr-startv] = modified_new;
//           }
//         }
//       }
//     }
//     MPI_Barrier(MPI_COMM_WORLD);
//     for (int v = 0; v < part_size; v ++) 
//     { 
//         modified[v] =  modified_nxt[v];
//         modified_nxt[v] = false;
//     }
//     send_data.clear();
//     receive_data.clear();
//   }
//   delete modified;
//   delete modified_nxt;
// }



// void dynamicSSSPDel(graph& g, int* dist,int* parent, bool* modified)
// {
//     int my_rank,np,part_size,startv,endv;

//     mpi::communicator world;
//     my_rank = world.rank();
//     np = world.size();
    
//     // graph distribution

//     g.parseGraph();

//     int max_degree = g.max_degree();

//     part_size = g.num_nodes()/np;
//     startv = my_rank*part_size;
//     endv = startv + (part_size-1);

//     bool finished = false;
//     int num_iter=0;
//     bool* modified_nxt=new bool[part_size];
//     for (int t = 0; t < part_size; t++)
//     { 
//         modified_nxt[t] = false;
//     }

//     MPI_Barrier(MPI_COMM_WORLD);

//     while ( is_finished(startv,endv,modified))
//     {
//       for (int v = startv; v <= endv; v++) 
//       {
//         if (!modified[v])
//         {
//           int parent_v = parent[v];
//           if(parent_v > -1 && modified[parent_v])
//           {
//             dist[v] = INT_MAX/2;
//             modified[v] = true;
//           }
//         }
//       }
//     }

//     while( is_finished(startv,endv,modified) )
//     {
//         vector < map<int,int> > send_data(np);
//         vector < map<int,int> > receive_data(np);
//         vector < map<int,float> > send_data_float(np);
//         vector < map<int,float> > receive_data_float(np);
//         vector < map<int,double> > send_data_double(np);
//         vector < map<int,double> > receive_data_double(np);
//         std::map<int,int>::iterator itr;
//         for(int v = startv; v <= endv; v++) 
//         {
//           if (modified[v] == true)
//           {
//             for (edge e: g.getInNeighbors(v)) 
//             {
//               int nbr = e.destination;
//               int weight = e.weight;
//               // DONE how to update this so that the dist can be reduced to part_size
//               if(nbr >= startv && nbr <= endv) {
//                 int dist_new = dist[nbr] + weight;
//                 if(dist[v] > dist_new)
//                 { 
//                   dist[v] = dist_new;
//                   parent[v] = nbr;
//                   modified[v] = false;
//                 }
//               } else {
//                 dest_pro = nbr / part_size;
//                 send_data[dest_pro][nbr] = my_rank;
//               }
//             }
//           }
//         }
        
//         // first sync
//         // get data from the neighboring vertex to update current values
//         all_to_all(world, send_data, receive_data);
//         send_data.clear();
//         MPI_Barrier(MPI_COMM_WORLD);

//         for(int t=0;t<np;t++)
//         {
//           if(t != my_rank)
//           {
//             for (auto x : receive_data[t])
//             {
//               int recv_rank = x.second;
//               int nbr = x.first;
//               send_data[recv_rank][nbr] = dist[nbr];
//             }
//           }
//         }
//         MPI_Barrier(MPI_COMM_WORLD);
//         receive_data.clear();
        
//         // second sync
//         // once data from neighboring vertex is recieved, update current vertex
//         all_to_all(world, send_data, receive_data);
        
//         for(int t=0;t<np;t++)
//         {
//           if(t != my_rank)
//           {
//             for (auto x : receive_data[t])
//             {
//               int dist_nbr = x.second;
//               int nbr = x.first;
//               for(int v = startv; v <= endv; v++) 
//               {
//                 if (modified[v] == true)
//                 {
//                   for (edge e: g.getNeighbors(v)) 
//                   {
//                     int nbr2 = e.destination;
//                     int weight = e.weight;
//                     if(nbr == nbr2) {
//                       int dist_new = dist_nbr + weight;
//                       if(dist[v] > dist_new)
//                       { 
//                         dist[v] = dist_new;
//                         parent[v] = nbr;
//                         modified[v] = false;
//                       }
//                     }
//                   }
//                 }
//               }
//             }
//           }
//         }
//         MPI_Barrier(MPI_COMM_WORLD);
        
//         send_data.clear();
//         receive_data.clear();
//         send_data_float.clear();
//         receive_data_float.clear();
//         send_data_double.clear();
//         receive_data_double.clear();
//     }
// }


int main(int argc, char* argv[])
{
    struct timeval start, end;
    if(argc < 3)
    {
        printf("Execute ./a.out input_graph_file numberOfProcesses\n");
        exit(0);
    }
    int np = strtol(argv[2], NULL, 10);
    //printf("The number of process entered : %d\n",np);
    mpi::environment env(argc, argv);
    mpi::communicator world;
    int my_rank = world.rank();

    graph G(argv[1]);
    // if(my_rank == 0)
      G.parseGraph();

    int src = 1;
    int* dist;// = new int[G.num_nodes()];
    int* parent;// = new int[G.num_nodes()];
    bool* modified = new bool[G.num_nodes()];
    bool* modified_add = new bool[G.num_nodes()];

    // int* edgeLen = G.getEdgeLen();

    // Compute_SSSP(G , dist , parent, src);

    std::vector<update> updateEdges = G.parseUpdates("updates.txt");
    int batchSize = 100;
    int elements = 0;
    for(int k=0;k<updateEdges.size();k+=batchSize)
    {
        if((k+batchSize) > updateEdges.size())
            elements = updateEdges.size() - k;
        else
            elements = batchSize;

        for (int t = 0; t < G.num_nodes(); t ++) 
        { 
            modified[t] = false;
            modified_add[t] = false;
        }
        for(int i=k;i<(k+batchSize) && i<updateEdges.size();i++)
        {
          if(updateEdges[i].type=='d')
          {
            update u = updateEdges[i];
            int src = u.source;
            int dest = u.destination;
            if(parent[dest]==src) {
              dist[dest] = INT_MAX/2;
              modified[dest] = true;  
              parent[dest] = -1;
            }
          }
        }
        for(int i=k;i<(k+batchSize) && i<updateEdges.size();i++)
        {
          if(updateEdges[i].type=='a') {
            update u = updateEdges[i];
            int src = u.source;
            int dest = u.destination;
            if(dist[dest]>dist[src] + 1) {
              modified_add[dest] = true;
              modified_add[src] = true;
            }
          }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        
        // if(my_rank == 0)
        //   G.updateCSR_Del(updateEdges, k, elements);
        // dynamicSSSPDel(G, dist, parent, modified);

        // MPI_Barrier(MPI_COMM_WORLD);
        
        // if(my_rank == 0)
          G.updateCSRAdd(updateEdges, k, elements);
        // DONE distribute the modified graph from proc 0 to all processes
        MPI_Barrier(MPI_COMM_WORLD);
        // Compute_dynamicSSSPAdd(G, dist, parent, modified_add);
    }
    delete modified;
    delete modified_add;
    // final distances are in dist of proc 0

    MPI_Finalize();
    return 0;
}