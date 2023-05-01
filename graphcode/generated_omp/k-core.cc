#include"k-core.h"

void k_core(graph& g , int k)
{
  int kcore_count = 0;
  int update_count = 1;
  int* indeg=new int[g.num_nodes()];
  int* indeg_prev=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    indeg[u] = g.common_nbrscount(u,u);
    indeg_prev[u] = indeg[u];
  }
  #pragma omp parallel for
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    if (indeg_prev[u] >= k )
      {
      int f = u;
      for (int edge = g.indexofNodes[u]; edge < g.indexofNodes[u+1]; edge ++) 
      {int v = g.edgeList[edge] ;
        if (indeg_prev[v] < k && indeg[f] >= k )
          {
          indeg[f]--;
        }
      }
    }
  }
  #pragma omp parallel for
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    if (indeg_prev[u] < k )
      {
      indeg[u] = 0;
    }
  }
  do
  {
    update_count = 0;
    #pragma omp parallel for
    for (int u = 0; u < g.num_nodes(); u ++) 
    {
      if (indeg[u] < k && indeg[u] != 0 )
        {
        int f = u;
        indeg[u] = 0;
        for (int edge = g.indexofNodes[u]; edge < g.indexofNodes[u+1]; edge ++) 
        {int v = g.edgeList[edge] ;
          if (indeg[v] >= k )
            {
            indeg[v]--;
            update_count++;
          }
        }
      }
    }
  }
  while(update_count > 0);
  #pragma omp parallel for
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    if (indeg[u] >= k )
      {
      kcore_count++;
    }
  }

}
