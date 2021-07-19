#include"sssp_dsl.h"

void Compute_SSSP(graph& g,int src)
{
  int my_rank,np,part_size,startv,endv;
  int max_degree = g.max_degree();
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
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  int num_iter=0;
  gettimeofday(&start, NULL);
  while ( is_finished(startv,endv,modified)  )
  {
    num_iter++;
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
      if (modified[v] == true ){
        modified[v] = false;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {
          int nbr = g.edgeList[edge] ;
          if(nbr >= startv && nbr <=endv)
          {
            int e = edge;
             int dist_new = dist[v] + weight[e];
            bool modified_new = true;
            if (dist[nbr] > dist_new)
            {
              dist[nbr] = dist_new;
              modified[nbr] = modified_new;
            }
          }
          else
          {
            dest_pro = nbr / part_size;
            count[dest_pro]=count[dest_pro]+1;
            int p=pos[dest_pro];
            send_data[dest_pro*3*max_degree] = count[dest_pro];
            send_data[dest_pro*3*max_degree+p] = dist[v];
            send_data[dest_pro*3*max_degree+(p+1)] = weight[e];
            send_data[dest_pro*3*max_degree+(p+2)] = nbr;
             pos[dest_pro] = pos[dest_pro]+3;
          }
        }
      }
      MPI_Alltoall(send_data,3*max_degree,MPI_INT,recv_data,3*max_degree,MPI_INT,MPI_COMM_WORLD);
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
              int dist_v = recv_data[(t*3*max_degree)+k];
              int weight_e = recv_data[(t*3*max_degree)+k+1];
              int nbr = recv_data[(t*3*max_degree)+k+2];
               int dist_new = dist_v + weight_e;
              bool modified_new = true;
              if (dist[nbr] > dist_new)
              {
                dist[nbr] = dist_new;
                modified[nbr] = modified_new;
              }
            }
          }
        }
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  delete [] send_data;
  delete [] recv_data;
  delete [] count;
  delete [] pos;
}

}