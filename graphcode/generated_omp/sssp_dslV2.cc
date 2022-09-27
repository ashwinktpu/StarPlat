#include"sssp_dslV2.h"

void Compute_SSSP(graph& g,int* dist,int* weight,int src
)
{
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
    #pragma omp parallel
    {
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        if (modified[v] == true )
          {
          for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
          {int nbr = g.edgeList[edge] ;
            int e = edge;
             int dist_new = dist[v] + weight[e];
            bool modified_new = true;
            if(dist[nbr] > dist_new)
            {
              int oldValue = dist[nbr];
              atomicMin(&dist[nbr],dist_new);
              if( oldValue > dist[nbr])
              {modified_nxt[nbr] = modified_new;
                finished = false ;
              }
            }
          }
        }
      }
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      { modified[v] =  modified_nxt[v] ;
        modified_nxt[v] = false ;
      }
    }
  }

}
