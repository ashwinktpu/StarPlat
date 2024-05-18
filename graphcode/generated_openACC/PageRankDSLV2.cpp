#include"PageRankDSLV2.h"

void Compute_PR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)
{
  float numNodes = (float)g.num_nodes( );
  float* pageRank_nxt=new float[g.num_nodes()];

  #pragma acc data copyin(g)
  {
    #pragma acc data copyout(pageRank[0: g.num_nodes()])  copyin( numNodes)
    {
      #pragma acc parallel loop
      for (int t = 0; t < g.num_nodes(); t ++) 
      {
        pageRank[t] = 1 / numNodes;
      }
    }
  }

  int iterCount = 0;
  float diff = 0.0 ;
  #pragma acc data copyin(g)
  {
    #pragma acc data copyin(g.indexofNodes[0:g.num_nodes()+1], g.rev_indexofNodes[0:g.num_nodes()+1], g.srcList[0:g.num_edges()+1]) copyin(beta, delta, maxIter, numNodes, pageRank_nxt[0:g.num_nodes()+1], iterCount) copy(pageRank[0:g.num_nodes()+1])
    {
      do
      {
        diff = 0.000000;
        #pragma acc data copyin(g)
        {
          #pragma acc data copyout(pageRank_nxt[0:g.num_nodes()+1]) copy(diff)
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
              float val = (1 - delta) / numNodes + delta * sum;
              diff = diff + val - pageRank[v];
              pageRank_nxt[v] = val;
            }
          }
        }

        #pragma acc data copyin(pageRank_nxt[0:g.num_nodes()+1]) copyout(pageRank[0:g.num_nodes()+1]) 
        {
          #pragma acc parallel loop
          for (int node = 0; node < g.num_nodes(); node ++) 
          {
            pageRank [node] = pageRank_nxt [node] ;
          }
        }
        iterCount++;
      }
      while((diff > beta) && (iterCount < maxIter));
    }
  }
  float* answer=new float[g.num_nodes()];
  #pragma acc data copyin(pageRank[0:g.num_nodes()]) 
  {
    #pragma acc parallel loop
    for (int t = 0; t < g.num_nodes(); t++)
    {
      answer[t] = pageRank[t];
    }
  }

}
