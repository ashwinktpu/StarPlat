#include "sssp.h" 
void Compute_SSSP(graph g,int src){
  int my_rank,np,part_size,startv,endv;
  int max_degree,num_nodes;
  int *index,*weight,*edgeList;
  int *local_index,*local_weight,*local_edgeList;
  struct timeval start, end, sinitial, einitial;
  long seconds,micros;
  
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  
  if(my_rank == 0)
  {
   gettimeofday(&start, NULL);
   g.parseGraph();
   gettimeofday(&end, NULL);
   seconds = (end.tv_sec - start.tv_sec);
   micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
   printf("The graph loading time = %ld micro secs.\n",micros);
   
   gettimeofday(&sinitial, NULL);
  	max_degree=g.max_degree();
  	weight = g.getEdgeLen();
    edgeList = g.getEdgeList();
  	index = g.getIndex();
    num_nodes = g.num_nodes();
    part_size = g.num_nodes()/np;
    MPI_Bcast (&max_degree,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    //MPI_Scatter (index,part_size+1,MPI_INT,local_index,part_size+1,MPI_INT,0,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    for(int i=0;i<part_size+1;i++)
      local_index[i] = index[i];
    /*for(int i=0;i<part_size+1;i++)
      printf("[%d] -- localindex[%d]=%d\n",my_rank,i,local_index[i]);*/
    int num_ele = local_index[part_size]-local_index[0];
    local_weight = new int[num_ele];
    for(int i=0;i<num_ele;i++)
      local_weight[i] = weight[i];
    local_edgeList = new int[num_ele];
    for(int i=0;i<num_ele;i++)
      local_edgeList[i] = edgeList[i];

    /*for(int i=0;i<num_ele;i++)
      printf("[%d] -- weight[%d]=%d\n",my_rank,i,local_weight[i]);*/
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
    delete [] index;
    delete [] weight;
    delete [] edgeList;
  }
  else
  {
    MPI_Bcast (&max_degree,1,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast (&num_nodes,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);
    local_index = new int[part_size+1];
    MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
   /* for(int i=0;i<part_size+1;i++)
      printf("[%d] -- localindex[%d]=%d\n",my_rank,i,local_index[i]);*/
    //MPI_Scatter (index,part_size+1,MPI_INT,local_index,part_size+1,MPI_INT,0,MPI_COMM_WORLD);
    int num_ele = local_index[part_size]-local_index[0];
    local_weight = new int[num_ele];
    MPI_Recv (local_weight,num_ele,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    local_edgeList = new int[num_ele];
    MPI_Recv (local_edgeList,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    /*for(int i=0;i<num_ele;i++)
      printf("[%d] -- weight[%d]=%d\n",my_rank,i,local_weight[i]);*/
  }
  MPI_Request request;
  
  //startv = my_rank*part_size;
  //endv = startv + (part_size-1);
  int *dist,*local_dist;
  bool *modified,*local_modified;
  bool finished,local_finished;
  if(my_rank == 0)
  {
    dist=new int[g.num_nodes()];
    modified=new bool[g.num_nodes()];
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      //dist[t] = INT_MAX;
     dist[t] = 10000;
      modified[t] = false;
    }
    
    modified[src] = true;
    dist[src] = 0;

    /*printf("Initial values of modifed and distance\n");
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      printf("%d ",dist[t]);
      //modified[t] = false;
    }
    printf("\n");
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      //dist[t] = INT_MAX;
      printf("%d ",modified[t]);
    }
    printf("\n");*/

    finished = false;
    local_finished = finished;
    //printf("[%d] -- The value of finished variable=%d\n",my_rank,local_finished);
    MPI_Bcast (&local_finished,1,MPI_C_BOOL,0,MPI_COMM_WORLD);
    local_modified = new bool[part_size];
    local_dist = new int[part_size];
    MPI_Scatter (modified,part_size,MPI_C_BOOL,local_modified,part_size,MPI_C_BOOL,0,MPI_COMM_WORLD);
    MPI_Scatter (dist,part_size,MPI_INT,local_dist,part_size,MPI_INT,0,MPI_COMM_WORLD);
    
    gettimeofday(&einitial, NULL);
   seconds = (einitial.tv_sec - sinitial.tv_sec);
   micros = ((seconds * 1000000) + einitial.tv_usec) - (sinitial.tv_usec);
   printf("The graph loading time = %ld micro secs.\n",micros);
   printf("The graph loading time = %ld secs.\n",seconds);
    /*   
    for(int i=0;i<part_size;i++)
    {
      local_modified[i] = modified[i];
      local_finished[i] = finished[i];
    }*/
    /*for(int i=0;i<part_size;i++)
      printf("[%d] -- modified[%d]=%d\n",my_rank,i,local_modified[i]);
    for(int i=0;i<part_size;i++)
      printf("[%d] -- distance[%d]=%d\n",my_rank,i,local_dist[i]);*/
  }
  else
  {
    //bool local_finished;
    MPI_Bcast (&local_finished,1,MPI_C_BOOL,0,MPI_COMM_WORLD);
    //printf("[%d] -- The value of finished variable=%d\n",my_rank,local_finished);
    local_modified = new bool[part_size];
    local_dist = new int[part_size];
    MPI_Scatter (modified,part_size,MPI_C_BOOL,local_modified,part_size,MPI_C_BOOL,0,MPI_COMM_WORLD);
    MPI_Scatter (dist,part_size,MPI_INT,local_dist,part_size,MPI_INT,0,MPI_COMM_WORLD);
    /*for(int i=0;i<part_size;i++)
      printf("[%d] -- modified[%d]=%d\n",my_rank,i,local_modified[i]);
    for(int i=0;i<part_size;i++)
      printf("[%d] -- distance[%d]=%d\n",my_rank,i,local_dist[i]);*/
  }
  
  int num_iter=0;
  gettimeofday(&start, NULL);
  while (is_finished(local_modified,part_size) )
  {
    num_iter++;
    for (int v = 0; v < part_size; v ++) 
    {
        //int *count = new int[np];
        //int *pos = new int[np];
        //int *send_data = new int[np*4*max_degree];
        vector <vector <int>> send_data(np);
        int dest_pro;
        /*
        for (int tem=0; tem<np; tem++)
        {
            count[tem]=0;
            pos[tem]=1;
            //send_data[tem*4*max_degree]=0;
            //send_data[
        }
        for (int tem=0; tem<np*4*max_degree; tem++)
        {
            send_data[tem]=0;
        }
        */
        //printf("check1\n");
        //MPI_Win window;
        //MPI_Win_create(send_data, (sizeof(int)*4*max_degree*np), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
        if ( local_modified[v]  == true)
        {
            local_modified[v] = false;
            for (int j = 0; j < local_index[v+1]-local_index[v]; j++)
            {
                int begin = local_index[0];
                int nbr = local_edgeList[local_index[v]-begin+j];
                int wt = local_weight[local_index[v]-begin+j];
                if (nbr >= (my_rank*part_size) && nbr < (my_rank*part_size+part_size))
                {
                    if (local_dist[nbr-my_rank*part_size] > local_dist[v]  + wt )
                    {
                        //printf("local\n");
                        //printf("The vertex %d -- neighbor %d\n",my_rank*part_size+v,nbr);
                        local_dist[nbr-my_rank*part_size]  = local_dist[v]  + wt ;
                        local_modified[nbr-my_rank*part_size]  = true;
                    }
                }
                else
                {
                    //printf("Remote\n");
                    dest_pro = nbr / part_size;
                    /*
                    count[dest_pro]=count[dest_pro]+1;
                    int p=pos[dest_pro];
                    send_data[dest_pro*4*max_degree] = count[dest_pro];
                    send_data[dest_pro*4*max_degree+p] =  local_dist[v] ;
                    send_data[dest_pro*4*max_degree+(p+1)] = nbr;
                    send_data[dest_pro*4*max_degree+(p+2)] = 
                    local_dist[v] ;
                    send_data[dest_pro*4*max_degree+(p+3)] = wt;
                    pos[dest_pro] = pos[dest_pro]+4;
                    */
                    send_data[dest_pro].push_back(local_dist[v]);
                    send_data[dest_pro].push_back(nbr);
                    send_data[dest_pro].push_back(wt);
                }
            }
        }
        /*
        for (int temp=0;temp < np;temp++)
        {
            if(temp != my_rank)
              MPI_Isend(send_data+temp*4*max_degree, 1, MPI_INT, temp, 0, MPI_COMM_WORLD, &request);
              if(send_data[temp*4*max_degree] > 0)
                MPI_Isend(send_data+(temp*4*max_degree+1), send_data[temp*4*max_degree]*4, MPI_INT, temp, 1, MPI_COMM_WORLD, &request);
        }*/
        for (int temp=0;temp<np;temp++)
        {
            if(temp != my_rank)
            {
              int amount = send_data[temp].size();
              MPI_Isend(&amount, 1, MPI_INT, temp, 0, MPI_COMM_WORLD, &request);
              if(amount > 0)
                MPI_Isend(&send_data[temp][0], amount, MPI_INT, temp, 1, MPI_COMM_WORLD, &request);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        //MPI_Win_fence(0, window);
        for(int t=0;t<np;t++)
        {
            if(t != my_rank)
            {
                int amount;
                //MPI_Get(&amount, 1, MPI_INT, t, (my_rank*4*max_degree), 1, MPI_INT, window);
                MPI_Recv(&amount, 1, MPI_INT, t, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //printf("The amount received %d\n",amount);
                if(amount>0)
                {
                    vector <int> buffer(amount);
                    MPI_Recv(&buffer[0], amount, MPI_INT, t, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    for(int k=0;k<amount;k+=3)
                    {
                        int dist_v = buffer[k],nbr = buffer[k+1],wt = buffer[k+2];
                        if (local_dist[nbr-my_rank*part_size] >dist_v + wt )
                        {
                            local_dist[nbr-my_rank*part_size]  =dist_v + wt ;
                            local_modified[nbr-my_rank*part_size]  =true;
                        }
                    }
                  //delete [] buffer;
                  buffer.clear();
                }
            }
        }
        /*
        for(int t=0;t<np;t++)
        {
            if(t != my_rank)
            {
                int amount;
                //MPI_Get(&amount, 1, MPI_INT, t, (my_rank*4*max_degree), 1, MPI_INT, window);
                MPI_Recv(&amount, 1, MPI_INT, t, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(amount>0)
                {
                    int *buffer = new int [4*amount];
                    MPI_Recv(buffer, (amount*4), MPI_INT, t, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    //MPI_Get(buffer, (amount*4), MPI_INT, t, (my_rank*4*max_degree+1), (amount*4), MPI_INT, window);
                    for(int k=0;k<(amount*4);k+=4)
                    {
                        int dist_v = buffer[k],nbr = buffer[k+1],src = buffer[k+2],wt = buffer[k+3];
                        if (local_dist[nbr-my_rank*part_size] >dist_v + wt )
                        {
                            local_dist[nbr-my_rank*part_size]  =dist_v + wt ;
                            local_modified[nbr-my_rank*part_size]  =true;
                        }
                    }
                  delete [] buffer;
                }
            }
        }
        */
        MPI_Barrier(MPI_COMM_WORLD);
        //MPI_Win_fence(0, window);
        send_data.clear();
        //delete [] send_data;
        //delete [] count;
        //delete [] pos;
    }
    
  }
  
  gettimeofday(&end, NULL);
   seconds = (end.tv_sec - start.tv_sec);
   micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
     
  MPI_Gather(local_dist,part_size,MPI_INT,dist,part_size,MPI_INT,0,MPI_COMM_WORLD);
  if(my_rank == 0)
  {
    printf("The number of iterations taken %d \n",num_iter);
    printf("The iteration time = %ld micro secs.\n",micros);
    printf("The iteration time = %ld secs.\n",seconds);
    //for(int i=startv;i<=endv;i++)
    char buf[1024];
    sprintf(buf,"output%d.txt",g.ori_num_nodes());
    FILE *fptr = fopen(buf, "w");
    if (fptr == NULL)
        {
            printf("Could not open output file to write output");
            return;
        }
    for(int i=0;i< g.ori_num_nodes();i++)
    {
        //cout<<"The distance of the vertex "<<i<< " is "<<dist[i] <<endl;
        //cout<<"dist["<<i<<"] = "<<final_dist[i] <<endl;
        fprintf(fptr,"dist[%d] = %d\n",i,dist[i]);
    }
    fclose(fptr);
    /*
    printf("The iteration time = %ld micro secs.\n",micros);
    for(int i=0;i<num_nodes;i++)
    //cout<<"The distance of the vertex "<<i<< " is "<<dist[i] <<endl;
    cout<<"dist["<<i<<"] = "<<dist[i] <<endl;
    //delete [] index;
    //delete [] weight;
    //delete [] edgeList;
    */
    delete [] dist;
    delete [] modified;
  }
  delete [] local_index;
  delete [] local_weight;
  delete [] local_edgeList;
  delete [] local_modified;
  delete [] local_dist;
  MPI_Finalize();
}

int main(int argc,char* argv[])
{ 

 
  // graph G("soc-Slashdot0811.txt");
   //graph G("sample_graph.txt");
   //G.parseGraph();
  //int* edgeLen=G.getEdgeLen();
  //int* dist=new int[G.num_nodes()+1];
  //int* parent=new int[G.num_nodes()+1];
  if(argc < 3)
   {
   	printf("Execute ./a.out input_graph_file numberOfProcesses\n");
   	exit(0);
   }
   int np = strtol(argv[2], NULL, 10);
   //printf("The number of process entered : %d\n",np);
   graph G(argv[1],np);
  int src=0;
  /*
  printf("The number of nodes is %d\n",G.num_nodes());
  printf("The number of edges is %d\n",G.num_edges());
  printf("The maximum degree of the graph is %d\n",G.max_degree());
  for(int i=0;i<G.num_nodes()+1;i++)
  {
  	std::cout<<G.indexofNodes[i]<< " ";
  }
  printf("\nEdgedetails\n");
  for(int i=0;i<G.num_edges();i++)
  {
  	std::cout<<G.edgeList[i]<< " ";
  }
  
   */           
  //for(int i=1;i<G.num_nodes()+1;i++)
  //{
  //	std::cout<<G.edge[i]<< " ";
  //}
  /*
  printf("\nNeighbordetails\n");
  for(int i=0;i<G.num_nodes();i++)
  {
  printf("The neighbors of node %d is ",i);
  for (int j = G.indexofNodes[i]; j<G.indexofNodes[i+1]; j ++)
            {
                int nbr=G.edgeList[j];
                printf("%d --> %d, ",nbr,G.edgeLen[j]);
              }
              printf("\n");
  }*/
   Compute_SSSP(G,src);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
 
  return 0;
}
