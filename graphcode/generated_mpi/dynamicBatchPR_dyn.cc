#include"dynamicBatchPR_dyn.h"

void staticPR(Graph& g, float beta, float delta, int maxIter, 
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
    diff = 0.000000;
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      float sum = 0.000000;
      for (int nbr : g.getInNeighbors(v)) 
      {
        sum = sum + pageRank.getValue(nbr) / g.num_out_nbrs(nbr);
      }

      float val = (1 - delta) / num_nodes + delta * sum;
      diff = diff + val - pageRank.getValue(v);
      pageRank_nxt.setValue(v,val);
    }
    world.barrier();

    float diff_temp = diff;
    MPI_Allreduce(&diff_temp,&diff,1,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);

    pageRank.assignCopy(pageRank_nxt);
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}
void dynamicBatchPR_add(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, NodeProperty<bool>& modified, boost::mpi::communicator world )
{
  NodeProperty<float> pageRank_nxt;
  pageRank_nxt.attachToGraph(&g, (float)0);
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getValue(v) == true )
      {
        float sum = 0.000000;
        for (int nbr : g.getInNeighbors(v)) 
        {
          sum = sum + pageRank.getValue(nbr) / g.num_out_nbrs(nbr);
        }

        float val = (1 - delta) / g.num_nodes( ) + delta * sum;
        diff = diff + val - pageRank.getValue(v);
        pageRank_nxt.setValue(v,val);
      }
    }
    world.barrier();

    float diff_temp = diff;
    MPI_Allreduce(&diff_temp,&diff,1,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);

    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getValue(v) == true )
      {
        pageRank.setValue(v,pageRank_nxt.getValue(v));
      }
    }
    world.barrier();

    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}
void dynamicBatchPR_del(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, NodeProperty<bool>& modified, boost::mpi::communicator world )
{
  NodeProperty<float> pageRank_nxt;
  pageRank_nxt.attachToGraph(&g, (float)0);
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getValue(v) == true )
      {
        float sum = 0.000000;
        for (int nbr : g.getInNeighbors(v)) 
        {
          sum = sum + pageRank.getValue(nbr) / g.num_out_nbrs(nbr);
        }

        float val = (1 - delta) / g.num_nodes( ) + delta * sum;
        diff = diff + val - pageRank.getValue(v);
        pageRank_nxt.setValue(v,val);
      }
    }
    world.barrier();

    float diff_temp = diff;
    MPI_Allreduce(&diff_temp,&diff,1,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);

    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getValue(v) == true )
      {
        pageRank.setValue(v,pageRank_nxt.getValue(v));
      }
    }
    world.barrier();

    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}
void DynPR(Graph& g, float beta, float delta, int maxIter, 
  NodeProperty<float>& pageRank, Updates & updateBatch, int batchSize, boost::mpi::communicator world )
{
  staticPR(g,beta,delta,maxIter,pageRank, world);

  updateBatch.splitIntoSmallerBatches(batchSize);
  while(updateBatch.nextBatch())
  {
    NodeProperty<bool> modified;
    NodeProperty<bool> modified_add;
    modified.attachToGraph(&g, (bool)false);
    modified_add.attachToGraph(&g, (bool)false);
    Updates & deleteBatch = updateBatch.getCurrentDeleteBatch();
    Updates & addBatch = updateBatch.getCurrentAddBatch();
    for(Update u : updateBatch.getCurrentDeleteBatch().getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      modified.setValue(dest,true);

    }
    g.propagateNodeFlags(modified);

    updateBatch.updateCsrDel(&g);

    dynamicBatchPR_del(g,beta,delta,maxIter,pageRank,modified, world);

    for(Update u : updateBatch.getCurrentAddBatch().getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      modified_add.setValue(dest,true);

    }
    g.propagateNodeFlags(modified_add);

    updateBatch.updateCsrAdd(&g);

    dynamicBatchPR_add(g,beta,delta,maxIter,pageRank,modified_add, world);


  }

}
