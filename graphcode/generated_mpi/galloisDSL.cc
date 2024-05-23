#include"galloisDSL.h"

void maxFlow(Graph& g, int source, int sink, boost::mpi::communicator world )
{
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
  label.setValue(source,g.num_nodes( ));
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  EdgeProperty<int> residual_capacity;
  residual_capacity.attachToGraph(&g, (int)0);
  residual_capacity = g.weights;
  Container<int> count;
  count.assign(g.num_nodes( ) + 2,0);

  world.barrier();
  int _t1 = 0 ;

  if ( source != -1 && world.rank () == g.get_node_owner (source) )
   { for (int v:g.getNeighbors(source))
    {
      v_leader_rank = world.rank();
      v = g.get_other_vertex(source, _t1);
      Edge forward = g.get_edge_i(source, _t1);
      if (residual_capacity.getValue(forward) > 0 )
      {
        Edge backward = g.get_edge(v, source);
        excess.atomicAdd (v, residual_capacity.getValue(forward));
        residual_capacity.setValue(forward,0);
        g.frontier_push(v);

      }
      _t1++;
    }

  }
  world.barrier () ;

  g.frontier_push(source);

  int activate = 10000;
  while (!g.frontier_empty( ) ){
    int u = g.frontier_pop( );
    int newLabel = g.num_nodes( ) + 1;
    for (int v : g.getNeighbors(u)) 
    {
      newLabel = std::min(newLabel,label.getValue(v));
    }


    count.atomicAdd (u.label, 1);
    label.setValue(u,newLabel + 1);
    count.atomicAdd (u.label, 1);
    world.barrier();
    int _t1 = 0 ;

    if ( u != -1 && world.rank () == g.get_node_owner (u) )
     { for (int v:g.getNeighbors(u))
      {
        v_leader_rank = world.rank();
        v = g.get_other_vertex(u, _t1);
        Edge forward = g.get_edge_i(u, _t1);
        if (label.getValue(u) == label.getValue(v) + 1 )
        {
          Edge backward = g.get_edge(v, u);
          int d = std::min(forward.residue,excess.getValue(u));
          if (d > 0 )
          {
            excess.atomicAdd (u, -d);
            excess.atomicAdd (v, d);
            residue.atomicAdd (forward, -d);
            residue.atomicAdd (backward, d);
            if (d > 0 && v != source && v != sink )
            {
              g.frontier_push(u);

            }
          }
        }
        _t1++;
      }

    }
    world.barrier () ;

    if (excess.getValue(u) > 0 && label.getValue(u) < g.num_nodes( ) + 1 )
    {
      g.frontier_push(u);

    }
    if (activate-- == 0 )
    {
      int gap = count.getIdx(0);
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (label.getValue(v) >= gap )
        {
          label.setValue(v,g.num_nodes( ) + 1);
        }
      }

    }
    world.barrier () ;

    activate = 1000;
  }
}

}
