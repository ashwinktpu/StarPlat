#include"dynamicBatchTCV2_dyn.h"

auto staticTC(Graph& g, boost::mpi::communicator world )
{
  long triangle_count = 0;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    for (int u : g.getNeighbors(v)) 
    {
      if (u < v )
      {
        for (int w : g.getNeighbors(v)) 
        {
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
  world.barrier();

  long triangle_count_temp = triangle_count;
  MPI_Allreduce(&triangle_count_temp,&triangle_count,1,MPI_LONG,MPI_SUM,MPI_COMM_WORLD);

  return triangle_count;

}
auto dynamicBatchTCV2_add(Graph& g, int triangle_countSent, EdgeProperty<bool>& modified, Updates & addBatch
  , boost::mpi::communicator world )
{
  long triangle_count = triangle_countSent;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;
  world.barrier();
  for(Update update : addBatch.getUpdates())
  {
    int v1 = update.source;
    int v2 = update.destination;
    for (int v3 : g.getNeighbors(v1)) 
    {
      if (v3 != v2 && v3 != v1 && v1 != v2 )
      {
        Edge e1 = g.get_edge(v1, v3);
        int newEdge = 1;
        bool isTriangle = false;
        if (modified.getValue(e1) )
        {
          newEdge = newEdge + 1;
        }
        if (g.check_if_nbr(v2, v3) )
        {
          Edge e2 = g.get_edge(v2, v3);
          isTriangle = true;
          if (modified.getValue(e2) )
          {
            newEdge = newEdge + 1;
          }
        }
        if (isTriangle )
        {
          if (newEdge == 1 )
          {
            count1 = count1 + 1;
          }
          else
          if (newEdge == 2 )
          {
            count2 = count2 + 1;
          }
          else
          {
            count3 = count3 + 1;
          }
        }
      }
    }

  }
  world.barrier();

  int count1_temp = count1;
  MPI_Allreduce(&count1_temp,&count1,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
  int count2_temp = count2;
  MPI_Allreduce(&count2_temp,&count2,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
  int count3_temp = count3;
  MPI_Allreduce(&count3_temp,&count3,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

  triangle_count = triangle_count + (count1 / 2 + count2 / 4 + count3 / 6);
  return triangle_count;

}
auto dynamicBatchTCV2_del(Graph& g, int triangle_countSent, EdgeProperty<bool>& modified, Updates & deleteBatch
  , boost::mpi::communicator world )
{
  long triangle_count = triangle_countSent;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;
  world.barrier();
  for(Update update : deleteBatch.getUpdates())
  {
    int v1 = update.source;
    int v2 = update.destination;
    for (int v3 : g.getNeighbors(v1)) 
    {
      if (v3 != v2 && v3 != v1 && v1 != v2 )
      {
        Edge e1 = g.get_edge(v1, v3);
        int newEdge = 1;
        bool isTriangle = false;
        if (modified.getValue(e1) )
        {
          newEdge = newEdge + 1;
        }
        if (g.check_if_nbr(v2, v3) )
        {
          Edge e2 = g.get_edge(v2, v3);
          isTriangle = true;
          if (modified.getValue(e2) )
          {
            newEdge = newEdge + 1;
          }
        }
        if (isTriangle )
        {
          if (newEdge == 1 )
          {
            count1 = count1 + 1;
          }
          else
          if (newEdge == 2 )
          {
            count2 = count2 + 1;
          }
          else
          {
            count3 = count3 + 1;
          }
        }
      }
    }

  }
  world.barrier();

  int count1_temp = count1;
  MPI_Allreduce(&count1_temp,&count1,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
  int count2_temp = count2;
  MPI_Allreduce(&count2_temp,&count2,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
  int count3_temp = count3;
  MPI_Allreduce(&count3_temp,&count3,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

  triangle_count = triangle_count - (count1 / 2 + count2 / 4 + count3 / 6);
  return triangle_count;

}
void DynTC(Graph& g, Updates & updateBatch, int batchSize, boost::mpi::communicator world )
{
  int triangleCount = staticTC(g, world);
  printf("triangle count : %d\n", triangleCount);
  updateBatch.splitIntoSmallerBatches(batchSize);
  while(updateBatch.nextBatch())
  {
    EdgeProperty<bool> modified_del;
    modified_del.attachToGraph(&g, (bool)false);
    Updates & deleteBatch = updateBatch.getCurrentDeleteBatch();
    Updates & addBatch = updateBatch.getCurrentAddBatch();
    for(Update u : updateBatch.getCurrentDeleteBatch().getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      for (int nbr : g.getNeighbors(src)) 
      {
        Edge e = g.get_edge(src, nbr);
        if (nbr == dest )
        {
          modified_del.setValue(e,true);
        }
      }


    }
    triangleCount = dynamicBatchTCV2_del(g,triangleCount,modified_del,deleteBatch, world);
    printf("triangle count : %d\n", triangleCount);
    updateBatch.updateCsrDel(&g);

    updateBatch.updateCsrAdd(&g);

    EdgeProperty<bool> modified_add;
    modified_add.attachToGraph(&g, (bool)false);
    for(Update u : updateBatch.getCurrentAddBatch().getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      for (int nbr : g.getNeighbors(src)) 
      {
        Edge e = g.get_edge(src, nbr);
        if (nbr == dest )
        {
          modified_add.setValue(e,true);
        }
      }


    }
    triangleCount = dynamicBatchTCV2_add(g,triangleCount,modified_add,addBatch, world);
    printf("triangle count : %d\n", triangleCount);
  }
  printf("triangle count : %d\n", triangleCount);
}

int main(int argc, char *argv[])
{
   
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    printf("program started\n"); 
    Graph graph(argv[1],world);
    world.barrier();

    Updates updateBatch(argv[2],world, &graph);

    DynTC(graph,updateBatch,100, world);
    
    world.barrier();
    return 0;
}