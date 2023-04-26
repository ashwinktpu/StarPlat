#include"sssp_dsl.h"

__global__ void Compute_SSSP_kernel(graph& g,int src)

{
  int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
    modified_nxt[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  while ( !finished )
  {
    finished = true;
    unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
    unsigned int v =id
    {
      if (modified[v] == true ){
        for (int edge = gpu_OA[id]; edge < gpu_OA[id+1]; edge ++) 
        {int nbr = g.edgeList[edge] ;
          int e = edge;
           int dist_new = dist[v] + weight[e];
          bool modified_new = true;
          if(dist[nbr] > dist_new)
          {
            int oldValue = dist[nbr];
            atomicMin(&dist[nbr],dist_new);
            if(oldValue > dist[nbr])
            {
              modified_nxt[nbr] = modified_new;
              finished = false ;
            }
          }
        }
      }
    }
  }

}