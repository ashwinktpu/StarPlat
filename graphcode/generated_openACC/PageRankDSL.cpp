#include"PageRankDSL.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)
{
  float num_nodes = (float)g.num_nodes( );

  #pragma acc data copyin(g)
  {
    #pragma acc data copyout( modified[0: g.num_nodes()], modified_nxt[0: g.num_nodes()], dist[0: g.num_nodes()+1] )
    {
      #pragma acc parallel loop
      for (int t = 0; t < g.num_nodes(); t ++) 
      {
        pageRank[t] = 1 / num_nodes;
      }
    }
  }

  int iterCount = 0;
  float diff = 0.0 ;
  #pragma acc data copyin(g)
  {
    #pragma acc data copyin( g.srcList[0:g.num_edges()+1], g.indexofNodes[:g.num_nodes()+1], g.rev_indexofNodes[:g.num_nodes()+1] ) copy(pageRank[0 : g.num_nodes()]) copyin(pageRank_nxt[0 : g.num_nodes()])
    {
      do
      {
        diff = 0.000000;
        #pragma acc data copyin(g)
        {
          #pragma acc data copyin( g.indexofNodes[:g.num_nodes()+1], g.edgeList[0:g.num_edges()] )  copy(diff)
          {
            #pragma acc parallel loop reduction(+ : diff)
            for (int v = 0; v < g.num_nodes(); v ++) 
            {
              float sum = 0.000000;
              for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
              {int nbr = g.srcList[edge] ;
                sum = sum + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
              }
              float val = (1 - delta) / num_nodes + delta * sum;
              diff = diff + val - pageRank[v];
              pageRank[v] = val;
            }
          }
        }
        iterCount++;
      }
      while((diff > beta) && (iterCount < maxIter));
    }
  }

}
