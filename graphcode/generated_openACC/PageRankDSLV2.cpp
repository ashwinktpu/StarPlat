#include"PageRankDSLV2.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)
{
  float num_nodes = (float)g.num_nodes( );
  float* pageRank_nxt=new float[g.num_nodes()];

  #pragma acc data copyin(g)
  {
    #pragma acc data copyout(pageRank[0: g.num_nodes()], pageRank_nxt[0: g.num_nodes()]) copyin()
    {
      #pragma acc parallel loop
      for (int t = 0; t < g.num_nodes(); t ++) 
      {
        pageRank[t] = 1 / num_nodes;
        pageRank_nxt[t] = 0;
      }
    }
  }

  int iterCount = 0;
  float diff = 0.0 ;
  #pragma acc data copyin(g)
  {
    #pragma acc data copyin(g.indexofNodes[0:g.num_nodes()+1], g.rev_indexofNodes[0:g.num_nodes()+1], g.srcList[0:g.num_edges()+1]) copyin(beta, delta, maxIter, pageRank[0:g.num_nodes()+1], num_nodes, pageRank_nxt[0:g.num_nodes()+1], iterCount)
    {
      do
      {
        diff = 0.000000;
        #pragma acc data copyin(g)
        {
          #pragma acc data  copy(diff)
          {
            #pragma acc parallel loop reduction(+ : diff)
            for (int v = 0; v < g.num_nodes(); v ++) 
            {
              float sum = 0.000000;
              for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
              {
                int nbr = g.srcList[edge] ;
                sum = sum + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
              }
              float val = (1 - delta) / num_nodes + delta * sum;
              diff = diff + val - pageRank[v];
              pageRank_nxt[v] = val;
            }
          }
        }

        #pragma acc parallel loop
        for (int node = 0; node < g.num_nodes(); node ++) 
        {
          pageRank [node] = pageRank_nxt [node] ;
        }
        iterCount++;
      }
      while((diff > beta) && (iterCount < maxIter));
    }
  }

}
