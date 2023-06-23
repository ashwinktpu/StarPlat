#include"PageRankDSLV2.h"

void Compute_PR(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, boost::mpi::communicator world )
{
  float num_nodes = (float)g.num_nodes( );
  NodeProperty<float> pageRank_nxt;
  pageRank.attachToGraph(&g, (float)1 / num_nodes);
  pageRank_nxt.attachToGraph(&g, (float)0);
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      float sum = 0.000000;
      for (int nbr : g.getInNeighbors(v)) 
      {
        sum = sum + pageRank.getValue(nbr) / g.num_out_nbrs(nbr);
      }

      float val = (1 - delta) / num_nodes + delta * sum;
      pageRank_nxt.setValue(v,val);
    }
    world.barrier();

    pageRank = pageRank_nxt;
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}
