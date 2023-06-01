#include"v_cover.h"

void v_cover(graph& g)
{
  bool* visited=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    visited[t] = false;
  }
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    if (visited[v] == false )
      {
      for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
      {int nbr = g.edgeList[edge] ;
        if (visited[nbr] == false )
          {
          visited[nbr] = true;
          visited[v] = true;
        }
      }
    }
  }

}
