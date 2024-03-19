#include"push_relabel.dsl.h"

void push__(Graph& g, int u, int v, NodeProperty<int>& excess, 
  EdgeProperty<int>& residual_capacity, boost::mpi::communicator world )
{
  Edge forward_edge = g.get_edge(u, v);
  Edge backward_edge = g.get_edge(v, u);
  int d = min(excess.getValue(u),residual_capacity.getValue(forward_edge), world);
  int temp = 0 ;
  temp = (excess.getValue(u) - d);
  if(world.rank() == g.get_node_owner(u))
  {
    excess.setValue(u,temp);
  }
  temp = (excess.getValue(v) + d);
  if(world.rank() == g.get_node_owner(v))
  {
    excess.setValue(v,temp);
  }
  temp = residual_capacity.getValue(forward_edge) - d;
  if(world.rank() == g.get_edge_owner(forward_edge))
  {
    residual_capacity.setValue(forward_edge,temp);
  }
  temp = residual_capacity.getValue(backward_edge) + d;
  if(world.rank() == g.get_edge_owner(backward_edge))
  {
    residual_capacity.setValue(backward_edge,temp);
  }
  if (v != src && v != snk )
  {
    g.frontier_push(v);

  }

}
void relabel(Graph& g, int u, EdgeProperty<int>& residue, NodeProperty<int>& label, 
  Container<int>& count, boost::mpi::communicator world )
{
  int new_label = INT_MAX;
  for (int v : g.getNeighbors(u)) 
  {
    Edge residual_capacity = g.get_edge(u, v);
    if (residue.getValue(residual_capacity) > 0 )
    {
      if (new_label < v )
      {
        new_label = v;
      }
    }
  }


  if (new_label < INT_MAX )
  {
    if(world.rank() == g.get_node_owner(u))
    {
      label.setValue(u,new_label + 1);
    }
    updateGap(count,label.getValue(u),amount, world);

  }

}
void discharge(Graph& g, int u, NodeProperty<int>& label, NodeProperty<int>& excess, 
  NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, Container<int>& count, boost::mpi::communicator world )
{
  while (excess.getValue(u) > 0 ){
    for (int v : g.getNeighbors(u)) 
    {
      Edge current_edge = g.get_edge(u, v);
      if (excess.getValue(u) > 0 && residue.getValue(current_edge) > 0 && label.getValue(u) == label.getValue(v) + 1 )
      {
        push__(g,u,v,excess,residue, world);

      }
    }


    if (excess.getValue(u) > 0 )
    {
      relabel(g,u,residue,label,count, world);

      fixGap(count,label, world);

    }
  }

}
void updateGap(Container<int>& count, int height, int amount, boost::mpi::communicator world )
{
  count[height] = amount;

}
void fixGap(Container<int>& count, NodeProperty<int>& label, boost::mpi::communicator world )
{
  int gap = count.getIdx(0);
  int t = 0 ;
  world.barrier();
  if (world.rank () == g.get_node_owner (v) ) 
   { 
     for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (label.getValue(v) >= gap )
      {
        label.setValue(v,INT_MAX);
      }
    }

  }
  world.barrier ()


}
void do_max_flow(Graph& g, int source, int sink, NodeProperty<int>& label, 
  NodeProperty<int>& excess, NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, boost::mpi::communicator world )
{
  residue = g.weights;
  label.attachToGraph(&g, (int)INT_MAX);
  excess.attachToGraph(&g, (int)0);
  curr_edge.attachToGraph(&g, (int)0);
  Container<int> count;
  count.assign(g.numNodes( ),0);

  int temp = 0;
  int res = 0;
  world.barrier();
  if ( world.rank () == g.get_node_owner (v) )
   { 
    for (int v:g.localNeighboursOnly(source))
    {
      Edge e = g.get_edge(source, v);
      excess.setValue(v,residue.getValue(e));
    }

  }
  world.barrier ()

  if(world.rank() == g.get_node_owner(source))
  {
    excess.setValue(source,temp);
  }
  if(world.rank() == g.get_node_owner(source))
  {
    label.setValue(source,g.num_nodes( ));
  }
  int x = label.getValue(source);
  while (!g.frontier_empty(world) ){
    int u = g.frontier_pop_local(world);
    discharge(g,u,label,excess,curr_edge,residue, world);

  }

}
