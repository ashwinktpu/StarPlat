#include "sssp_sparse.h" 
void Compute_SSSP(graph g,int src){
  int my_rank,np,part_size,startv,endv;
  int max_degree=g.max_degree();
  struct timeval start, end,start1,end1;
  long seconds,micros;
    
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Request request1;
  MPI_Request request2;

  gettimeofday(&start1, NULL);
  g.parseGraph();
  gettimeofday(&end1, NULL);
  seconds = (end1.tv_sec - start1.tv_sec);
  micros = ((seconds * 1000000) + end1.tv_usec) - (start1.tv_usec);
  if(my_rank == 0)
  {
    printf("The graph loading time = %ld secs.\n",seconds);
    printf("Calculating inter partition degree\n");
  }

  int* weight=g.getEdgeLen();

  
  part_size = g.num_nodes()/np;
  startv = my_rank*part_size;
  endv = startv + (part_size-1);
 
   int local_ipDeg=0;
   int global_ipDeg=0;
  for (int i=startv; i<=endv;i++)
  {
    for (int j = g.indexofNodes[i]; j<g.indexofNodes[i+1]; j++)
    {
      int nbr = g.edgeList[j];
      if(!(nbr >= startv && nbr <=endv))
        local_ipDeg++;
    }
  }
  printf("[%d] inter part degree %d\n",my_rank,local_ipDeg);
  MPI_Allreduce(&local_ipDeg, &global_ipDeg, 1, MPI_INT, MPI_MAX,MPI_COMM_WORLD);
  if(my_rank==0)
    printf("Global inter part degree %d\n",global_ipDeg);

  int* dist=new int[g.num_nodes()];
  int* final_dist=new int[g.num_nodes()];
  //bool* modified=new bool[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    //dist[t] = 10000;
    final_dist[t] = 0;
    //modified[t] = false;
  }
  //modified[src] = true;
  dist[src] = 0;
  //bool finished = false;
  
  vector <int> modified;
  vector <int> modified_next;
  modified.push_back(src); 
  
  
  int dest_pro;
  int num_iter=0;

  if(my_rank == 0)
  	printf("Initializations completed...started processing\n");

  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  while (is_finished (startv,endv,modified) )
  {
    //vector <vector <int>> send_data(np);
    int *count = new int[np];
        int *pos = new int[np];
        int dest_pro;
        for (int tem=0; tem<np; tem++)
        {
            count[tem]=0;
            pos[tem]=1;
            //send_data[tem*4*max_degree]=0;
            //send_data[
        }
    int* send_data = new int[np*3*global_ipDeg];
    int* recv_data = new int[np*3*global_ipDeg];
    for (int tem=0; tem<np*3*global_ipDeg; tem++)
        {
            send_data[tem]=0;
            recv_data[tem]=0;
        }
    num_iter++;
    if(my_rank==0)
        printf("At iteration %d\n",num_iter);
    //MPI_Win window;
    //MPI_Win_create(&send_data[0][0], sizeof(int)*send_data.size(), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
    /*
    int *send_bufSize = new int[np];
    int *recv_bufSize = new int[np];
    for (int i=0;i<np;i++)
    {
      send_bufSize[i] = 0;
      recv_bufSize[i] = 0;
    }*/
    while (modified.size() > 0)
    {
      
     // printf("[%d]: modified list size at beginning %ld",my_rank,modified.size());
      //printf("[%d]: modified list contains ",my_rank);
      /*
      for (int t = 0; t<modified.size();t++)
        printf("%d ",modified[t]);
      printf("\n");*/
      int v = modified.back();
      modified.pop_back();
      //printf(" [%d] i am \n",my_rank);
      if (v >= startv && v <= endv)
      {
          for (int j = g.indexofNodes[v]; j<g.indexofNodes[v+1]; j++)
          {

            //printf(" [%d] i am \n",my_rank);
            int nbr = g.edgeList[j];
            int wt = weight[j];
            if (nbr >= startv && nbr <= endv)
            {
              if (dist[nbr] >dist[v]  + wt )
              {
                dist[nbr]  =dist[v]  + wt ;
                //modified[nbr]  =true;
                modified_next.push_back(nbr);
              }
            }
            else
            {
              /*
              dest_pro = nbr / part_size;
              send_data[dest_pro].push_back(dist[v]);
              send_data[dest_pro].push_back(nbr);
              send_data[dest_pro].push_back(wt);
              */
              dest_pro = nbr / part_size;
                    count[dest_pro]=count[dest_pro]+1;
                    int p=pos[dest_pro];
                    send_data[dest_pro*3*global_ipDeg] = count[dest_pro];
                    send_data[dest_pro*3*global_ipDeg+p] =  dist[v] ;
                    send_data[dest_pro*3*global_ipDeg+(p+1)] = nbr;
                    //send_data[dest_pro*3*max_degree+(p+2)] = dist[v] ;
                    send_data[dest_pro*3*global_ipDeg+(p+2)] = wt;
                    pos[dest_pro] = pos[dest_pro]+3;
                }
            //printf("Data to be send from process %d to %d : %d, %d, %d\n",my_rank,dest_pro,dist[v],nbr,wt);
            }
          }
      //} //}
    }

    /*
   for (int temp=0;temp<np;temp++)
        {
            if(temp != my_rank)
            {
              int amount = send_data[temp].size();
              MPI_Isend(&amount, 1, MPI_INT, temp, 0, MPI_COMM_WORLD, &request1);
              if(amount > 0)
              {
                MPI_Isend(&send_data[temp][0], amount, MPI_INT, temp, 1, MPI_COMM_WORLD, &request2);
                MPI_Wait(&request2, MPI_STATUS_IGNORE);
              }
              MPI_Wait(&request1, MPI_STATUS_IGNORE);
            }
        }
   printf("[%d] before barrier1\n",my_rank);
   MPI_Barrier(MPI_COMM_WORLD);
   printf("[%d] after barrier1\n",my_rank);
   */
    MPI_Alltoall(send_data,3*global_ipDeg,MPI_INT,recv_data,3*global_ipDeg,MPI_INT,MPI_COMM_WORLD);
    //MPI_Barrier(MPI_COMM_WORLD);
        //MPI_Alltoall(send_data,3*max_degree,MPI_INT,recv_data,3*max_degree,MPI_INT,MPI_COMM_WORLD);
        /*
        for(int t=0;t<np;t++)
        {
        	if(t!=my_rank)
        	{
        		MPI_Win_fence(0, window);
        		int *recv_data = new int[3*max_degree];
        		for(int tem=0;tem<3*max_degree;tem++)
        			recv_data[tem]=0;
        		MPI_Get(recv_data, 3*max_degree, MPI_INT, t, (my_rank*3*max_degree), 3*max_degree, MPI_INT, window);
        		int amount = recv_data[0];
		        if(amount>0)
		        {
		            for(int k=1;k<=(amount*3);k+=3)
		            {
		                int dist_v = recv_data[k],nbr = recv_data[k+1],wt = recv_data[k+2];
		                //src = recv_data[(t*3*max_degree)+k+2],
		                if (dist[nbr] >dist_v + wt )
		                {
		                    dist[nbr]  =dist_v + wt ;
		                    modified[nbr]  =true;
		                }
		            }
		        }
		        MPI_Win_fence(0, window);
		        delete [] recv_data;
        	}
        }
        */
    //MPI_Win_fence(MPI_MODE_NOPRECEDE, window);
    for(int t=0;t<np;t++)
    {
      if(t != my_rank)
      {
          //int amount;
                //MPI_Get(&amount, 1, MPI_INT, t, (my_rank*4*max_degree), 1, MPI_INT, window);
          //MPI_Recv(&amount, 1, MPI_INT, t, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         //for (int i=0;i<t;i++)
          //dis += recv_bufSize[i];
		     //MPI_Get(buffer, amount, MPI_INT, t, dis, amount, MPI_INT, window);
		//amount = buffer[0];
                //MPI_Get(&amount, 1, MPI_INT, t, (my_rank*3*max_degree), 1, MPI_INT, window);
		//	printf("[%d] Recv: Data at send_data[%d] of process %d is %d\n",my_rank,my_rank*3*max_degree,t,amount);
                //MPI_Recv(&amount, 1, MPI_INT, t, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //printf("[%d] Reached 1\n ",my_rank);
	//	MPI_Win_fence(0, window);
		//amount = buffer[0];
		//printf("[%d] Recv: Data at send_data[%d] of process %d is %d\n",my_rank,my_rank*3*max_degree,t,amount);
                int amount = recv_data[t*3*global_ipDeg];
                if(amount>0)
                {
                	//printf("[%d] Reached 2\n ",my_rank);
                   // int *buffer = new int [3*amount];
                    //MPI_Recv(buffer, (amount*4), MPI_INT, t, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    //MPI_Get(buffer, (amount*3), MPI_INT, t, (my_rank*3*max_degree+1), (amount*3), MPI_INT, window);
 			//MPI_Win_fence(0,window); 
       //vector <int> buffer(amount);
       // MPI_Recv(&buffer[0], amount, MPI_INT, t, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                  for(int k=1;k<=(amount*3);k+=3)
                    {
                        //int dist_v = buffer[k],nbr = buffer[k+1],wt = buffer[k+2];
			//printf("Data received by process %d from process %d : %d, %d, %d\n",my_rank,t,dist_v,nbr,wt);
                        //,src = buffer[k+2]
                        int dist_v = recv_data[(t*3*global_ipDeg)+k],nbr = recv_data[(t*3*global_ipDeg)+k+1],wt = recv_data[(t*3*global_ipDeg)+k+2];
                        if (dist[nbr] > dist_v + wt )
                        {
                            dist[nbr] = dist_v + wt ;
                            modified_next.push_back(nbr);
                        }
			
                    }
			//MPI_Win_fence(0,window);
                 // delete [] buffer;
                 //buffer.clear();
                }
                //printf("[%d] Reached 3 \n",my_rank);
	        	//delete [] buffer;
            }
            //printf("[%d] Reached 4 \n",my_rank);
        }
        
        
        modified.swap(modified_next);
        printf("[%d] size of modified for next iteration %ld\n",my_rank,modified.size());
        printf("[%d] before barrier2\n",my_rank);
        MPI_Barrier(MPI_COMM_WORLD);
        printf("[%d] after barrier2\n",my_rank);
        //send_data.clear();
        delete [] send_data;
        delete [] recv_data;
        delete [] count;
        delete [] pos;
        //MPI_Win_free (&window);
        /*
        printf("[%d]: modified list contains ",my_rank);
      for (int t = 0; t<modified.size();t++)
        printf("%d ",modified[t]);
      printf("\n");
      printf("[%d]: modified_next list contains ",my_rank);
      for (int t = 0; t<modified_next.size();t++)
        printf("%d ",modified_next[t]);
      printf("\n");*/
        
        //modified_next.clear();  
        /*
        printf("[%d]: modified list contains ",my_rank);
      for (int t = 0; t<modified.size();t++)
        printf("%d ",modified[t]);
      printf("\n");*/
    
  }
  
  gettimeofday(&end, NULL);
   seconds = (end.tv_sec - start.tv_sec);
   micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
   
  
  MPI_Gather(dist+my_rank*part_size,part_size,MPI_INT,final_dist,part_size,MPI_INT,0,MPI_COMM_WORLD);
  if(my_rank==0)
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
        fprintf(fptr,"dist[%d] = %d\n",i,final_dist[i]);
    }
    fclose(fptr);
  }
  MPI_Finalize();
}

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
   graph G(argv[1],np);
   //graph G("soc-Slashdot0811.txt");
   //graph G("sample_graph.txt");
   /*
   gettimeofday(&start, NULL);
   G.parseGraph();
   gettimeofday(&end, NULL);
   long seconds = (end.tv_sec - start.tv_sec);
   long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
   //printf("The graph loading time = %ld micro secs.\n",micros);
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  */
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
  printf("\nNeighbordetails\n");
  for(int i=0;i<G.num_nodes();i++)
  {
  printf("The neighbors of node %d is ",i);
  for (int j = G.indexofNodes[i]; j<G.indexofNodes[i+1]; j ++)
            {
                int nbr=G.edgeList[j];
                printf("%d : %d, ",nbr,G.edgeLen[j]);
              }
              printf("\n");
  }
   */           
  //for(int i=1;i<G.num_nodes()+1;i++)
  //{
  //	std::cout<<G.edge[i]<< " ";
  //}
   Compute_SSSP(G,src);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  
  




  return 0;
}
