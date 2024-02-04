#include"push_relabel.dsl.h"

int src, snk ;

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
void relabel(Graph& g, int u, EdgeProperty<int>& residue, NodeProperty<int>& label
  , boost::mpi::communicator world )
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
  }

}
void discharge(Graph& g, int u, NodeProperty<int>& label, NodeProperty<int>& excess, 
  NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, boost::mpi::communicator world )
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
      relabel(g,u,residue,label, world);

    }
  }

}
void do_max_flow(Graph& g, int source, int sink, boost::mpi::communicator world )
{
  while (!g.frontier_empty( ) ){
    int u = g.frontier_pop( );
    assert(u != -1, world);

    discharge(u, world);

  }

}
