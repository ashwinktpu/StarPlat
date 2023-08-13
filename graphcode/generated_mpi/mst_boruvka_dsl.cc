#include"mst_boruvka_dsl.h"

void Boruvka(Graph& g, boost::mpi::communicator world )
{
  NodeProperty<int> nodeId;
  NodeProperty<int> color;
  NodeProperty<bool> modified;
  EdgeProperty<bool> isMSTEdge;
  nodeId.attachToGraph(&g, (int)-1);
  color.attachToGraph(&g, (int)-1);
  modified.attachToGraph(&g, (bool)false);
  isMSTEdge.attachToGraph(&g, (bool)false);
  world.barrier();
  for (int u = g.start_node(); u <= g.end_node(); u ++) 
  {
    nodeId.setValue(u,u);
    color.setValue(u,u);
  }
  world.barrier();

  NodeProperty<int> minEdgeOfComp;
  NodeProperty<int> minEdge;
  bool noNewComp = false;
  while ( !noNewComp )
  {
    minEdge.attachToGraph(&g, (int)-1);
    world.barrier();
    for (int src = g.start_node(); src <= g.end_node(); src ++) 
    {
      for (int dst : g.getNeighbors(src)) 
      {
        if (color.getValue(src) != color.getValue(dst) )
        {
          Edge newEdge = g.get_edge(src, dst);
          Edge currMinEgde = minEdge.getValue(src);
          if (currMinEgde == -1 )
          {
            minEdge.setValue(src,newEdge);
          }
          else
          {
            int currMinDst = currMinEgde.data;
            if (weight.getValue(newEdge) < weight.getValue(currMinEgde) || (weight.getValue(e) == weight.getValue(minEdge) && color.getValue(dst) < color.getValue(currMinDst)) )
            {
              minEdge.setValue(src,e);
            }
          }
        }
      }

    }
    world.barrier();

    minEdgeOfComp.attachToGraph(&g, (int)-1);
    bool finishedMinEdge = false;
    while ( !finishedMinEdge )
    {
      int finishedMinEdge_leader_rank = -1 ;

      world.barrier();
      for (int u = g.start_node(); u <= g.end_node(); u ++) 
      {
        int comp = color.getValue(u);
        int minEdge = minEdgeOfComp.getValue(comp);
        int e = minEdge.getValue(u);
        if (e != -1 )
        {
          int dst = e.data;
          if (minEdge == -1 )
          {
            minEdgeOfComp.setValue(comp,e);
            finishedMinEdge_leader_rank = world.rank();
            finishedMinEdge = false;
          }
          if (minEdge != -1 )
          {
            int minDst = minEdge.data;
            if (weight.getValue(e) < weight.getValue(minEdge) || (weight.getValue(e) == weight.getValue(minEdge) && color.getValue(dst) < color.getValue(minDst)) )
            {
              minEdgeOfComp.setValue(comp,e);
              finishedMinEdge_leader_rank = world.rank();
              finishedMinEdge = false;
            }
          }
        }
      }
      world.barrier();
      int finishedMinEdge_leader_rank_temp = finishedMinEdge_leader_rank;
      MPI_Allreduce(&finishedMinEdge_leader_rank_temp,&finishedMinEdge_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      MPI_Bcast(&finishedMinEdge,1,MPI_C_BOOL,finishedMinEdge_leader_rank,MPI_COMM_WORLD);



      finishedMinEdge = modified.aggregateValue(NOT);
    }
    world.barrier();
    for (int src = g.start_node(); src <= g.end_node(); src ++) 
    {
      if (color.getValue(src) == nodeId.getValue(src) )
      {
        Edge srcMinEdge = minEdgeOfComp.getValue(src);
        if (srcMinEdge != -1 )
        {
          int dst = srcMinEdge.data;
          int dstLead = color.getValue(dst);
          Edge dstMinEdge = minEdgeOfComp.getValue(dstLead);
          if (dstMinEdge != -1 )
          {
            int dstOfDst = dstMinEdge.data;
            int dstOfDstLead = color.getValue(dstOfDst);
            if (color.getValue(src) == color.getValue(dstOfDstLead) && color.getValue(src) > color.getValue(dstLead) )
            {
              minEdgeOfComp.setValue(dstLead,-1);
            }
          }
        }
      }
    }
    world.barrier();

    world.barrier();
    for (int src = g.start_node(); src <= g.end_node(); src ++) 
    {
      if (color.getValue(src) == nodeId.getValue(src) )
      {
        Edge srcMinEdge = minEdgeOfComp.getValue(src);
        if (srcMinEdge != -1 )
        {
          isMSTEdge.setValue(srcMinEdge,true);
        }
      }
    }
    world.barrier();

    int noNewComp_leader_rank = -1 ;

    world.barrier();
    for (int src = g.start_node(); src <= g.end_node(); src ++) 
    {
      if (color.getValue(src) == nodeId.getValue(src) )
      {
        Edge srcMinEdge = minEdgeOfComp.getValue(src);
        if (srcMinEdge != -1 )
        {
          noNewComp_leader_rank = world.rank();
          noNewComp = false;
          int dst = srcMinEdge.data;
          color.setValue(src,color.getValue(dst));
        }
      }
    }
    world.barrier();
    int noNewComp_leader_rank_temp = noNewComp_leader_rank;
    MPI_Allreduce(&noNewComp_leader_rank_temp,&noNewComp_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
    MPI_Bcast(&noNewComp,1,MPI_C_BOOL,noNewComp_leader_rank,MPI_COMM_WORLD);



    bool finished = false;
    while ( !finished )
    {
      int finished_leader_rank = -1 ;

      world.barrier();
      for (int u = g.start_node(); u <= g.end_node(); u ++) 
      {
        int my_color = color.getValue(u);
        int other_color = color.getValue(my_color);
        if (my_color != other_color )
        {
          finished_leader_rank = world.rank();
          finished = false;
          color.setValue(u,other_color);
        }
      }
      world.barrier();
      int finished_leader_rank_temp = finished_leader_rank;
      MPI_Allreduce(&finished_leader_rank_temp,&finished_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      MPI_Bcast(&finished,1,MPI_C_BOOL,finished_leader_rank,MPI_COMM_WORLD);



      finished = modified.aggregateValue(NOT);
    }
    noNewComp = modified.aggregateValue(NOT);
  }

}
