#include"push_relabel.dsl.h"

void push__(Graph& g, int u, int v_idx, NodeProperty<int>& excess, 
  EdgeProperty<int>& residual_capacity, boost::mpi::communicator world )
{
  int v = g.get_other_vertex(u, v_idx);
  Edge forward_edge = g.get_edge(u, v);
  Edge backward_edge = g.get_edge(v, u);
  int d = std::min(excess.getValue(u),residual_capacity.getValue(forward_edge));
  excess.atomicAdd (u, -d);
  excess.atomicAdd (v, d);
  residual_capacity.atomicAdd (forward_edge, -d);
  residual_capacity.atomicAdd (backward_edge, d);
  if (v != src && v != snk )
  {
    g.frontier_push(v);

  }

}
void relabel(Graph& g, int u, EdgeProperty<int>& residue, NodeProperty<int>& label, 
  Container<int>& count, boost::mpi::communicator world )
{
  int x = label.getValue(u);
  int new_label = g.num_nodes( ) + 2;
  int _t1 = 0 ;

  for (int v : g.getNeighbors(u)) 
  {
    v = g.get_other_vertex(u, _t1);
    Edge residual_capacity = g.get_edge_i(u, _t1);
    if (residue.getValue(residual_capacity) > 0 )
    {
      if (new_label < v )
      {
        new_label = v;
      }
    }
    _t1++;
  }


  if (new_label < g.num_nodes( ) + 1 && new_label + 1 > x )
  {
    label.setValue(u,new_label + 1);
    count.atomicAdd (oldIdx, 1);
    count.atomicAdd (new_label + 1, 1);
    label.setValue(u,new_label + 1);
  }

}
void discharge(Graph& g, int u, NodeProperty<int>& label, NodeProperty<int>& excess, 
  NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, Container<int>& count, boost::mpi::communicator world )
{
  bool go = true;
  while (excess.getValue(u) > 0 && go ){
    for (int v : g.getNeighbors(u)) 
    {
      int vIdx = curr_edge.getValue(u);
      int v = g.get_ither_vertex(u,vIdx);
      Edge current_edge = g.get_edge_i(u, vIdx);
      if (excess.getValue(u) > 0 && residue.getValue(current_edge) > 0 && label.getValue(u) == label.getValue(v) + 1 )
      {
        push__(g,u,vIdx,excess,residue, world);

      }
    }


    if (excess.getValue(u) > 0 )
    {
      int prevValue = label.getValue(u);
      relabel(g,u,residue,label,count, world);

      if (label.getValue(u) == prevValue )
      {
        go = false;
      }
      curr_edge.setValue(u,0);
    }
  }

}
auto fixGap(Container<int>& count, NodeProperty<int>& label, boost::mpi::communicator world )
{
  int gap = count.getIdx(0);
  if (gap == -1 )
  {
    return 1;
  }
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (v != src && v != snk && label.getValue(v) >= gap )
    {
      label.setValue(v,g.num_nodes( ) + 2);
    }
  }
  world.barrier();



}
void do_max_flow(Graph& g, int source, int sink, NodeProperty<int>& label, 
  NodeProperty<int>& excess, NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, boost::mpi::communicator world )
{
  residue = g.weights;
  label.attachToGraph(&g, (int)INT_MAX);
  excess.attachToGraph(&g, (int)0);
  curr_edge.attachToGraph(&g, (int)0);
  Container<int> count;
  count.assign(g.num_nodes( ),0);

  int temp = 0;
  int res = 0;
  world.barrier();
  int _t1 = 0 ;

  if ( source != -1 && world.rank () == g.get_node_owner (source) )
   { for (int v:g.getNeighbors(source))
    {
      v_leader_rank = world.rank();
      v = g.get_other_vertex(source, _t1);
      Edge e = g.get_edge_i(source, _t1);
      v_leader_rank = world.rank();
      v = g.get_other_vertex(source, _t1);
      Edge r_e = g.get_edge_r_i(_t1, source);
      excess.setValue(v,residue.getValue(e));
      excess.atomicAdd (r_e, residue.getValue(e));
      residue.setValue(e,0);
      _t1++;
    }

  }
  world.barrier () ;

  label.setValue(source,g.num_nodes( ));
  int x = label.getValue(source);
  while (!g.frontier_empty( ) ){
    int u = g.frontier_pop_local( );
    if (u != -1 )
    {
      discharge(g,u,label,excess,curr_edge,residue, world);

    }
    if (activate == 0 )
    {
      fixGap(g,count,label, world);

    }
    else
    {
      activate--;
    }
  }

}
