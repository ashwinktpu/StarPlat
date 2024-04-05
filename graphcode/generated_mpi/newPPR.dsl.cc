#include"newPPR.dsl.h"

void do_max_flow(Graph& g, int source, int sink, EdgeProperty<int>& residual_capacity
  , boost::mpi::communicator world )
{
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
//  if(world.rank() == g.get_node_owner(source))
 // {
    label.setValue(source,g.num_nodes( ));
  //}
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  NodeProperty<int> curr_edge;
  curr_edge.attachToGraph(&g, (int)0);
  world.barrier();
  if ( world.rank () == g.get_node_owner (v) )
   { 
    for (int v:g.neigbors(source))
    {
      Edge forward_edge = g.get_edge(source, v);
      Edge backward_edge = g.get_edge(v, source);
      int d = residual_capacity.getValue(forward_edge);
      excess.setValue(v,d);
      int temp = 0 ;
      temp = residual_capacity.getValue(forward_edge) - d;
      residual_capacity.setValue(forward_edge,temp);
      temp = residual_capacity.getValue(backward_edge) + d;
      residual_capacity.setValue(backward_edge,temp);
    }

  }
  world.barrier ()

  bool flag = false ;
  do
  {
    flag = false;
    int flag_leader_rank = -1 ;

    world.barrier();
    if (world.rank () == g.get_node_owner (u) ) 
     { 
       for (int u = g.start_node(); u <= g.end_node(); u ++) 
      {
        if (u != source && u != sink && excess.getValue(u) > 0 )
        {
          flag_leader_rank = world.rank();
          flag = true;
          do
          {
            for (int v : g.getNeighbors(u)) 
            {
              Edge current_edge = g.get_edge(u, v);
              if (excess.getValue(u) > 0 && residual_capacity.getValue(current_edge) > 0 )
              {
                Edge forward_edge = g.get_edge(u, v);
                Edge backward_edge = g.get_edge(v, u);
                int d = 0 ;
                if (excess.getValue(u) < residual_capacity.getValue(forward_edge) )
                {
                  d = excess.getValue(u);
                }
                else
                {
                  d = residual_capacity.getValue(forward_edge);
                }
                int temp = 0 ;
                temp = (excess.getValue(u) - d);
                excess.setValue(u,temp);
                temp = (excess.getValue(v) + d);
                excess.setValue(v,temp);
                temp = residual_capacity.getValue(forward_edge) - d;
                residual_capacity.setValue(forward_edge,temp);
                temp = residual_capacity.getValue(backward_edge) + d;
                residual_capacity.setValue(backward_edge,temp);
              }
            }


            if (excess.getValue(u) > 0 )
            {
              int new_label = INT_MAX;
              for (int v : g.getNeighbors(u)) 
              {
                Edge residual = g.get_edge(u, v);
                if (residual_capacity.getValue(residual) > 0 )
                {
                  if (new_label < v )
                  {
                    new_label = v;
                  }
                }
              }


              if (new_label < INT_MAX )
              {
                label.setValue(u,new_label + 1);
              }
            }
          }
          while(excess.getValue(u) > 0);}
      }
      int flag_leader_rank_temp = flag_leader_rank;
      MPI_Allreduce(&flag_leader_rank_temp,&flag_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      MPI_Bcast(&flag,1,MPI_C_BOOL,flag_leader_rank,MPI_COMM_WORLD);



    }
    world.barrier () ;

  }
  while(flag);
}
