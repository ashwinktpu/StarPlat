#include"triangle_counting_dsl.h"

auto Compute_TC(graph& g)
{
  long triangle_count = 0;
  #pragma acc data copyin(g)
  {
    #pragma acc data copyin(g.indexofNodes[0:g.num_nodes()+1], g.edgeList[0:g.num_edges()+1]) copy(triangle_count)
    {
      #pragma acc parallel loop reduction(+ : triangle_count)
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {
          int u = g.edgeList[edge] ;
          if (u < v )
            {
            for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
            {
              int w = g.edgeList[edge] ;
              if (w > v )
                {
                if (g.check_if_nbr(u, w) )
                  {
                  triangle_count = triangle_count + 1;
                }
              }
            }
          }
        }
      }
    }
  }
  return triangle_count;

}
