#include "sssp.h" 
void Compute_SSSP(graph g,int* weight,int src){

  int my_rank,np,part_size,startv,endv;
  int max_degree=g.max_degree();
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Request request;
  part_size = g.num_nodes()/np;
  
  if(my_rank != np-1)
  {
  startv = my_rank*part_size;
  endv = startv + (part_size-1);
  //printf("My rank %d and start_vertex %d and end vertex %d\n",my_rank,startv,endv);
  }
  else
  {
  startv = my_rank*part_size;
  endv = g.num_nodes()-1;  
  //printf("My rank %d and start_vertex %d and end vertex %d\n",my_rank,startv,endv);
  }
  int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  while (is_finished (startv,endv,modified) )
  {
    for (int v = startv; v <= endv; v ++) 
    {
        int *count = new int[np];
        int *pos = new int[np];
        int *send_data = new int[np*4*max_degree];
        int dest_pro;
        for (int tem=0; tem<np; tem++)
        {
            count[tem]=0;
            pos[tem]=1;
            send_data[tem*4*max_degree]=0;
        }
        MPI_Win window;
        MPI_Win_create(send_data, (sizeof(int)*4*max_degree*np), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
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
                    send_data[dest_pro*4*max_degree] = count[dest_pro];
                    send_data[dest_pro*4*max_degree+p] =  dist[v] ;
                    send_data[dest_pro*4*max_degree+(p+1)] = nbr;
                    send_data[dest_pro*4*max_degree+(p+2)] = 
                    dist[v] ;
                    send_data[dest_pro*4*max_degree+(p+3)] = wt;
                    pos[dest_pro] = pos[dest_pro]+4;
                }
            }
        }
        MPI_Win_fence(0, window);
        for(int t=0;t<np;t++)
        {
            int amount;
            MPI_Get(&amount, 1, MPI_INT, t, (my_rank*4*max_degree), 1, MPI_INT, window);
            if(amount!=0)
            {
                int *buffer = new int [4*amount];
                MPI_Get(buffer, (amount*4), MPI_INT, t, (my_rank*4*max_degree+1), (amount*4), MPI_INT, window);
                for(int k=0;k<(amount*4);k+=4)
                {
                    int dist_v = buffer[k],nbr = buffer[k+1],src = buffer[k+2],wt = buffer[k+3];
                    if (dist[nbr] >dist_v + wt )
                    {
                        dist[nbr]  =dist_v + wt ;
                        modified[nbr]  =true;
                    }
                }
            }
        }
        MPI_Win_fence(0, window);
        delete [] send_data;
        delete [] count;
        delete [] pos;
    }
  }
  for(int i=startv;i<=endv;i++)
  //cout<<"The distance of the vertex "<<i<< " is "<<dist[i] <<endl;
  cout<<i<<" "<<dist[i] <<endl;
  MPI_Finalize();
}

int main()
{ 

 
   graph G("oregon1_010526.txt");
   G.parseGraph();
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=0;
  //printf("The number of nodes is %d\n",G.num_nodes());
  //printf("The number of edges is %d\n",G.num_edges());
  //printf("The maximum degree of the graph is %d\n",G.max_degree());
  //for(int i=0;i<G.num_nodes()+1;i++)
  //{
  //	std::cout<<G.indexofNodes[i]<< " ";
  //}
  //printf("\nEdgedetails\n");
  //for(int i=0;i<G.num_edges();i++)
  //{
  //	std::cout<<G.edgeList[i]<< " ";
  //}
  /*
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
   Compute_SSSP(G,edgeLen,src);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  
  




  return 0;
}

