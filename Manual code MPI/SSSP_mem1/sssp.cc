#include "sssp.h" 
void Compute_SSSP(graph g,int src){
  int my_rank,np,part_size,startv,endv;
  struct timeval start, end;
  long seconds,micros;
    
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Request request;
  
  gettimeofday(&start, NULL);
  g.parseGraph();
  gettimeofday(&end, NULL);
  seconds = (end.tv_sec - start.tv_sec);
  micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

  if(my_rank == 0)
    printf("Graph loading time = %ld\n micro secs.",micros);
  int *weight = new int[g.num_edges()];
  weight = g.getEdgeLen();
  int *edgeList = new int[g.num_edges()];
  edgeList = g.getEdgeList();
  int *indexofNodes = new int[g.num_nodes()+1];
  indexofNodes = g.getIndexofNodes();
  int max_degree=g.max_degree();

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
  
  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&start, NULL);
  int num_iter=0;
  while (is_finished (startv,endv,modified) )
  {
    num_iter++;
    for (int v = startv; v <= endv; v ++) 
    {
        //int *count = new int[np];
        //int *pos = new int[np];
        //int *send_data = new int[np*3*max_degree];
        //int *recv_data = new int[np*3*max_degree];
        vector <vector <int>> send_data(np);
        int dest_pro;
        //for (int tem=0; tem<np; tem++)
        //{
          //  count[tem]=0;
           // pos[tem]=1;
            //send_data[tem*4*max_degree]=0;
            //send_data[
        //}
        //for (int tem=0; tem<np*3*max_degree; tem++)
        //{
          //  send_data[tem]=0;
            //recv_data[tem]=0;
        //}
        //MPI_Win window;
        //MPI_Win_create(send_data, (sizeof(int)*4*max_degree*np), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
        if ( modified[v]  == true)
        {
            modified[v] = 0;
            for (int j = indexofNodes[v]; j<indexofNodes[v+1]; j ++)
            {
                int nbr=edgeList[j];
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
                    send_data[dest_pro].push_back(dist[v]);
                    send_data[dest_pro].push_back(nbr);
                    send_data[dest_pro].push_back(wt);
                }
            }
        }
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
        /*
        MPI_Alltoall(send_data,3*max_degree,MPI_INT,recv_data,3*max_degree,MPI_INT,MPI_COMM_WORLD);
        
        for(int t=0;t<np;t++)
        {
        	if(t!=my_rank)
        	{
        		int amount = recv_data[t*3*max_degree];
		        if(amount>0)
		        {
		            for(int k=1;k<=(amount*3);k+=3)
		            {
		                int dist_v = recv_data[(t*3*max_degree)+k],nbr = recv_data[(t*3*max_degree)+k+1],wt = recv_data[(t*3*max_degree)+k+2];
		                //src = recv_data[(t*3*max_degree)+k+2],
		                if (dist[nbr] >dist_v + wt )
		                {
		                    dist[nbr]  =dist_v + wt ;
		                    modified[nbr]  =true;
		                }
		            }
		        }
        	}
        }
        */
        
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
                        if (dist[nbr] >dist_v + wt )
                        {
                            dist[nbr]  =dist_v + wt ;
                            modified[nbr]  =true;
                        }
                    }
                  //delete [] buffer;
                  buffer.clear();
                }
            }
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        //MPI_Win_fence(0, window);
        //delete [] send_data;
        send_data.clear();
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
   //graph G("soc-Slashdot0811.txt");
   //graph G("sample_graph.txt");
   //graph G("sampleUSA.txt");
   if(argc < 3)
   {
   	printf("Execute ./a.out input_graph_file numberOfProcesses\n");
   	exit(0);
   }
   int np = strtol(argv[2], NULL, 10);
   //printf("The number of process entered : %d\n",np);
   graph G(argv[1],np);
   gettimeofday(&start, NULL);
   //printf("The graph loading time = %ld micro secs.\n",micros);
  //int* edgeLen=G.getEdgeLen();
  //int* dist=new int[G.num_nodes()+1];
  //int* parent=new int[G.num_nodes()+1];
  
  /*
  printf("The number of padded nodes is %d\n",G.num_nodes());
  printf("The number of original nodes is %d\n",G.ori_num_nodes());
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
  	if(G.indexofNodes[i+1]-G.indexofNodes[i] > 0)
  	{
  	printf("The neighbors of node %d is ",i);
	  for (int j = G.indexofNodes[i]; j<G.indexofNodes[i+1]; j ++)
		    {
		        int nbr=G.edgeList[j];
		        printf("%d : %d, ",nbr,G.edgeLen[j]);
		      }
		      printf("\n");
      }
  }
     */       
  //for(int i=1;i<G.num_nodes()+1;i++)
  //{
  //	std::cout<<G.edge[i]<< " ";
  //}
  int src=0;
   Compute_SSSP(G,src);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  return 0;
}
