#include "sssp.h" 
void Compute_SSSP(graph g,int src,int* weight){
  int my_rank,np,part_size,startv,endv;
  int max_degree=g.max_degree();
  struct timeval start, end;
  long seconds,micros;
    
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Request request;
  
  part_size = g.num_nodes()/np;
  startv = my_rank*part_size;
  endv = startv + (part_size-1);
  int* dist=new int[g.num_nodes()];
  int* final_dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    //dist[t] = 10000;
    final_dist[t] = 0;
    modified[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  
  int num_iter=0;
    if(my_rank == 0)
  	printf("Initializations completed...started processing\n");
  gettimeofday(&start, NULL);
  while (is_finished (startv,endv,modified) )
  {
    num_iter++;
    for (int v = startv; v <= endv; v ++) 
    {
        int *count = new int[np];
        int *pos = new int[np];
        int *send_data = new int[np*3*max_degree];
        //int *recv_data = new int[np*3*max_degree];
        int dest_pro;
        for (int tem=0; tem<np; tem++)
        {
            count[tem]=0;
            pos[tem]=1;
            //send_data[tem*4*max_degree]=0;
            //send_data[
        }
        for (int tem=0; tem<np*3*max_degree; tem++)
        {
            send_data[tem]=0;
            //recv_data[tem]=0;
        }
        MPI_Win window;
        MPI_Win_create(send_data, (sizeof(int)*3*max_degree*np), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
        if ( modified[v]  == true)
        {
            modified[v] = 0;
            for (int j = g.indexofNodes[v]; j<g.indexofNodes[v+1]; j ++)
            {
                int nbr=g.edgeList[j];
                int wt = weight[j];
                if (nbr >= startv && nbr <= endv)
                {
                    if (dist[nbr] >dist[v]  + wt )
                    {
                        dist[nbr]  =dist[v]  + wt ;
                        modified[nbr]  =true;
                    }
                }
                else
                {
                    dest_pro = nbr / part_size;
                    count[dest_pro]=count[dest_pro]+1;
                    int p=pos[dest_pro];
                    send_data[dest_pro*3*max_degree] = count[dest_pro];
                    send_data[dest_pro*3*max_degree+p] =  dist[v] ;
                    send_data[dest_pro*3*max_degree+(p+1)] = nbr;
                    //send_data[dest_pro*3*max_degree+(p+2)] = dist[v] ;
                    send_data[dest_pro*3*max_degree+(p+2)] = wt;
                    pos[dest_pro] = pos[dest_pro]+3;
		    //printf("Data to be send from process %d to %d : %d, %d, %d\n",my_rank,dest_pro,dist[v],nbr,wt);
                }
            }
        }
        /*
        for (int temp=0;temp<np;temp++)
        {
            if(temp != my_rank)
              //MPI_Isend(send_data+temp*4*max_degree, 1, MPI_INT, temp, 0, MPI_COMM_WORLD, &request);
              //if(send_data[temp*4*max_degree] > 0)
                //MPI_Isend(send_data+(temp*4*max_degree+1), send_data[temp*4*max_degree]*4, MPI_INT, temp, 1, MPI_COMM_WORLD, &request);
        }*/
	/*
	if(my_rank == 0)
	{
		for (int tmp=0;tmp <3*np*max_degree;tmp++)
			printf("[%d] : send_data[%d] = %d\n",my_rank,tmp,send_data[tmp]);
	} */
       // MPI_Barrier(MPI_COMM_WORLD);
        //MPI_Win window;
        //MPI_Win_create(send_data, (sizeof(int)*3*max_degree*np), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
        MPI_Barrier(MPI_COMM_WORLD);
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
        MPI_Win_fence(MPI_MODE_NOPRECEDE, window);
        for(int t=0;t<np;t++)
        {
            if(t != my_rank)
            {
                int amount;
		int *buffer = new int[3*max_degree];
		MPI_Get(buffer, 3*max_degree, MPI_INT, t, (my_rank*3*max_degree), 3*max_degree, MPI_INT, window);
		//amount = buffer[0];
                //MPI_Get(&amount, 1, MPI_INT, t, (my_rank*3*max_degree), 1, MPI_INT, window);
		//	printf("[%d] Recv: Data at send_data[%d] of process %d is %d\n",my_rank,my_rank*3*max_degree,t,amount);
                //MPI_Recv(&amount, 1, MPI_INT, t, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //printf("[%d] Reached 1\n ",my_rank);
		MPI_Win_fence(0, window);
		amount = buffer[0];
		//printf("[%d] Recv: Data at send_data[%d] of process %d is %d\n",my_rank,my_rank*3*max_degree,t,amount);
                if(amount>0)
                {
                	//printf("[%d] Reached 2\n ",my_rank);
                   // int *buffer = new int [3*amount];
                    //MPI_Recv(buffer, (amount*4), MPI_INT, t, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    //MPI_Get(buffer, (amount*3), MPI_INT, t, (my_rank*3*max_degree+1), (amount*3), MPI_INT, window);
 			//MPI_Win_fence(0,window);  
                  for(int k=1;k<=(amount*3);k+=3)
                    {
                        int dist_v = buffer[k],nbr = buffer[k+1],wt = buffer[k+2];
			//printf("Data received by process %d from process %d : %d, %d, %d\n",my_rank,t,dist_v,nbr,wt);
                        //,src = buffer[k+2]
                        if (dist[nbr] >dist_v + wt )
                        {
                            dist[nbr]  =dist_v + wt ;
                            modified[nbr]  =true;
                        }
			
                    }
			//MPI_Win_fence(0,window);
                 // delete [] buffer;
                }
                //printf("[%d] Reached 3 \n",my_rank);
		delete [] buffer;
            }
            //printf("[%d] Reached 4 \n",my_rank);
        }
        
        
        MPI_Barrier(MPI_COMM_WORLD);
	MPI_Win_fence(MPI_MODE_NOSUCCEED,window);
        //MPI_Win_fence(0, window);
        delete [] send_data;
        //delete [] recv_data;
        delete [] count;
        delete [] pos;
        MPI_Win_free (&window);
    }
    
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
   gettimeofday(&start, NULL);
   G.parseGraph();
   gettimeofday(&end, NULL);
   long seconds = (end.tv_sec - start.tv_sec);
   long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
   //printf("The graph loading time = %ld micro secs.\n",micros);
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
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
   Compute_SSSP(G,src,edgeLen);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  
  




  return 0;
}
