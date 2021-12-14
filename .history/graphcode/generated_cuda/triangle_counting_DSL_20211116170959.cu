#include"triangle_counting_DSL.h"

__global__ void Compute_TC_kernel(graph& g)

{
  long triangle_count = 0;
  unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);
  {
    for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
    {int u = g.edgeList[edge] ;
      if (u < v ){
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int w = g.edgeList[edge] ;
          if (w > v ){
            if (g.check_if_nbr(u, w) ){
              triangle_count = triangle_count+ 1;
            }
          }
        }
      }
    }
  }

}