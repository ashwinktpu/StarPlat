<<<<<<< HEAD
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
  bool* modified=new bool[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  int* final_dist=new int[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++)
    final_dist[t] = 0;
  gettimeofday(&start, NULL);
  while (is_finished (startv,endv,modified) )
  {
    for (int v = startv; v <= endv; v ++) 
    {
        int *count = new int[np];
        int *pos = new int[np];
        int *send_data = new int[np*3*max_degree];
        int *recv_data = new int[np*3*max_degree];
        int dest_pro;
        for (int tem=0; tem<np; tem++)
        {
            count[tem]=0;
            pos[tem]=1;
        }
        for (int tem=0; tem<np*3*max_degree; tem++)
        {
            send_data[tem]=0;
            recv_data[tem]=0;
        }
        //MPI_Win window;
        //MPI_Win_create(send_data, (sizeof(int)*4*max_degree*np), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
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
                    //send_data[dest_pro*3*max_degree+(p+2)] =  dist[v] ;
                    send_data[dest_pro*3*max_degree+(p+2)] = wt;
                    pos[dest_pro] = pos[dest_pro]+3;
                }
            }
        }
        /*
        for (int temp=0;temp<np;temp++)
        {
            if(temp != my_rank)
              MPI_Isend(send_data+temp*4*max_degree, 1, MPI_INT, temp, 0, MPI_COMM_WORLD, &request);
              if(send_data[temp*4*max_degree] > 0)
                MPI_Isend(send_data+(temp*4*max_degree+1), send_data[temp*4*max_degree]*4, MPI_INT, temp, 1, MPI_COMM_WORLD, &request);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        */
        MPI_Alltoall(send_data,3*max_degree,MPI_INT,recv_data,3*max_degree,MPI_INT,MPI_COMM_WORLD);
        //MPI_Win_fence(0, window);
        for(int t=0;t<np;t++)
        {
            if(t != my_rank)
            {
                int amount = recv_data[t*3*max_degree];
                //MPI_Get(&amount, 1, MPI_INT, t, (my_rank*4*max_degree), 1, MPI_INT, window);
                //MPI_Recv(&amount, 1, MPI_INT, t, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(amount>0)
                {
                    //int *buffer = new int [4*amount];
                    //MPI_Recv(buffer, (amount*4), MPI_INT, t, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    //MPI_Get(buffer, (amount*4), MPI_INT, t, (my_rank*4*max_degree+1), (amount*4), MPI_INT, window);
                    for(int k=1;k<(amount*3);k+=3)
                    {
                        int dist_v = recv_data[(t*3*max_degree)+k],nbr = recv_data[(t*3*max_degree)+k+1],wt = recv_data[(t*3*max_degree)+k+2];
                        if (dist[nbr] >dist_v + wt )
                        {
                            dist[nbr]  =dist_v + wt ;
                            modified[nbr]  =true;
                        }
                    }
                }
            }
        }
        //MPI_Win_fence(0, window);
        MPI_Barrier(MPI_COMM_WORLD);
        delete [] send_data;
        delete [] recv_data;
        delete [] count;
        delete [] pos;
    }
  }
  gettimeofday(&end, NULL);
  seconds = (end.tv_sec - start.tv_sec);
  micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
  MPI_Gather(dist+my_rank*part_size,part_size,MPI_INT,final_dist,part_size,MPI_INT,0,MPI_COMM_WORLD);
  if(my_rank==0)
  {
      cout<<"The iteration time = "<<micros<<" micro secs."<<endl;
      for(int i=0;i< g.num_nodes();i++)
      cout<<"dist["<<i<<"] = "<<final_dist[i] <<endl;
  }
  MPI_Finalize();
}

=======
#include"sssp.h"
#include <atomic>
#include <sys/time.h>

void Compute_SSSP(graph& g,int* weight,int* dist,int src)
{ 

    
  const int node_count=g.num_nodes();
//  printf("nodeCount %d\n",node_count);
  omp_lock_t* lock=(omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

  #pragma omp parallel for num_threads(4)
  for(int v = 0; v<g.num_nodes(); v++)
  omp_init_lock(&lock[v]);
  //int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];

  #pragma omp parallel for num_threads(4)
  for (int t = 0; t<g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
 
  }
  modified[src] = true;
  dist[src] = 0;

  std::atomic_bool finished = {false};
  int round=0;
   
  while ( !finished )
  { 
    finished=true;
    #pragma omp parallel for num_threads(4) schedule(dynamic,128)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true ){
          modified[v] = false;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int nbr = g.edgeList[edge] ;
          int e = edge;
           int dist_new = dist[v] + weight[e];
           
          if (dist[nbr] > dist_new)
          {
            bool modified_new = true;
            finished.store(false,std::memory_order_relaxed);
            gm_spinlock_acquire_for_node(nbr);
            if (dist[nbr] > dist_new)
            {
              
              dist[nbr] = dist_new;
            
              modified[nbr] = modified_new;
            }
           gm_spinlock_release_for_node(nbr);
          }
        }
      }
    }
   
  
   
  }

    printf("Rounds : %d\n",round);
  
    delete[] weight;
    delete[] modified;
    delete[] lock;

   
 

}
int main()
{ 

  
   graph G("dataRecords/cleanuwUSARoad.txt");
   G.parseGraph();
   gm_spinlock_table_init();
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=0;
 
    struct timeval T1, T2;
    gettimeofday(&T1, NULL);
   Compute_SSSP(G,edgeLen,dist,src);
   gettimeofday(&T2, NULL);
    
   printf("running time=%lf\n", (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
   char *outputfilename = "outputSG.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");

  // First crossing vehicles list
  for (int i = 0; i <= G.num_nodes(); i++)
  {
    fprintf(outputfilepointer, "%d  %d\n", i, dist[i]);
  }
  
  
  
  




  return 0;
}
>>>>>>> da5dbf845f87d4dee325ace2fe92861c865cd224
