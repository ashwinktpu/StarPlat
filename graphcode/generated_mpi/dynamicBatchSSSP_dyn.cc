#include"dynamicBatchSSSP_dyn.h"

void staticSSSP(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  int src, boost::mpi::communicator world )
{
  NodeProperty<bool> modified;
  NodeProperty<bool> modified_nxt;
  dist.attachToGraph(&g, (int)INT_MAX);
  modified.attachToGraph(&g, (bool)false);
  modified_nxt.attachToGraph(&g, (bool)false);
  parent.attachToGraph(&g, (int)-1);
  if(world.rank() == g.get_node_owner(src))
  {
    modified.setValue(src,true);
  }
  if(world.rank() == g.get_node_owner(src))
  {
    dist.setValue(src,0);
  }
  bool finished = false;
  while ( !finished )
  {
    std::vector<Property *> temp_properties = {(Property*)&modified_nxt, (Property*)&parent};
    g.initialise_reduction(MPI_MIN, (Property*)&dist, temp_properties);
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getValue(v) == true )
      {
        for (int nbr : g.getNeighbors(v)) 
        {
          Edge e = g.get_edge(v, nbr);
          g.queue_for_reduction(std::make_pair(nbr,dist.getValue(v) + weight.getValue(e)), std::make_pair(nbr,true), std::make_pair(nbr,v));
        }

      }
    }
    g.sync_reduction();
    world.barrier();

    modified = modified_nxt;
    modified_nxt.attachToGraph(&g, (bool)false);
    finished = modified.aggregateValue(NOT);
  }

}
void dynamicBatchSSSP_add(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  NodeProperty<bool>& modified, boost::mpi::communicator world )
{
  NodeProperty<bool> modified_nxt;
  modified_nxt.attachToGraph(&g, (bool)false);
  bool finished = false;
  while ( !finished )
  {
    std::vector<Property *> temp_properties = {(Property*)&modified_nxt, (Property*)&parent};
    g.initialise_reduction(MPI_MIN, (Property*)&dist, temp_properties);
    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getValue(v) == true )
      {
        for (int nbr : g.getNeighbors(v)) 
        {
          Edge e = g.get_edge(v, nbr);
          g.queue_for_reduction(std::make_pair(nbr,dist.getValue(v) + weight.getValue(e)), std::make_pair(nbr,true), std::make_pair(nbr,v));
        }

      }
    }
    g.sync_reduction();
    world.barrier();

    modified = modified_nxt;
    modified_nxt.attachToGraph(&g, (bool)false);
    finished = modified.aggregateValue(NOT);
  }

}
void dynamicBatchSSSP_del(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  NodeProperty<bool>& modified, boost::mpi::communicator world )
{
  bool finished = false;
  while (!finished ){
    finished = true;
    modified.rememberHistory();
    int finished_leader_rank = -1 ;

    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getHistoryValue(v) == false )
      {
        int parent_v = parent.getValue(v);
        if (parent_v > -1 && modified.getValue(parent_v) )
        {
          dist.setValue(v,INT_MAX / 2);
          modified.setValue(v,true);
          parent.setValue(v,-1);
          finished_leader_rank = world.rank();
          finished = false;
        }
      }
    }
    world.barrier();
    int finished_leader_rank_temp = finished_leader_rank;
    MPI_Allreduce(&finished_leader_rank_temp,&finished_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
    MPI_Bcast(&finished,1,MPI_C_BOOL,finished_leader_rank,MPI_COMM_WORLD);



  }
  finished = false;
  while (!finished ){
    finished = true;
    modified.rememberHistory();
    int finished_leader_rank = -1 ;

    world.barrier();
    for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (modified.getHistoryValue(v) == true )
      {
        for (int nbr : g.getInNeighbors(v)) 
        {
          Edge e = g.get_edge(nbr, v);
          if (dist.getValue(v) > dist.getValue(nbr) + weight.getValue(e) )
          {
            dist.setValue(v,dist.getValue(nbr) + weight.getValue(e));
            parent.setValue(v,nbr);
            finished_leader_rank = world.rank();
            finished = false;
          }
        }

      }
    }
    world.barrier();
    int finished_leader_rank_temp = finished_leader_rank;
    MPI_Allreduce(&finished_leader_rank_temp,&finished_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
    MPI_Bcast(&finished,1,MPI_C_BOOL,finished_leader_rank,MPI_COMM_WORLD);



  }

}
void DynSSSP(Graph& g, NodeProperty<int>& dist, NodeProperty<int>& parent, EdgeProperty<int>& weight, 
  Updates * updateBatch, int batchSize, int src, boost::mpi::communicator world )
{
  staticSSSP(g,dist,parent,weight,src, world);

  updateBatch->splitIntoSmallerBatches(batchSize);
  while(updateBatch->nextBatch())
  {
    NodeProperty<bool> modified;
    NodeProperty<bool> modified_add;
    modified.attachToGraph(&g, (bool)false);
    modified_add.attachToGraph(&g, (bool)false);
    for(Update u : updateBatch->getCurrentDeleteBatch()->getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      if (parent.getValue(dest) == src )
      {
        dist.setValue(dest,INT_MAX / 2);
        modified.setValue(dest,true);
        parent.setValue(dest,-1);
      }

    }
    updateBatch->updateCsrDel(&g);

    dynamicBatchSSSP_del(g,dist,parent,weight,modified, world);

    for(Update u : updateBatch->getCurrentAddBatch()->getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      if (dist.getValue(dest) > dist.getValue(src) + 1 )
      {
        modified_add.setValue(dest,true);
        modified_add.setValue(src,true);
      }

    }
    updateBatch->updateCsrAdd(&g);

    dynamicBatchSSSP_add(g,dist,parent,weight,modified_add, world);


  }

}
