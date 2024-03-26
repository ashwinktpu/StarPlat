#include"push_relabel.dsl.h"
int src, snk ;
void push__(Graph& g, int u, int v, NodeProperty<int>& excess, 
  EdgeProperty<int>& residual_capacity, boost::mpi::communicator world )
{
  printf ("pushing from %d to %d\n",u,v); 
  Edge forward_edge = g.get_edge(u, v);
  Edge backward_edge = g.get_edge(v, u);
  int d = std::min(excess.getValue(u),residual_capacity.getValue(forward_edge));
  int temp = 0 ;
  temp = (excess.getValue(u) - d);
  excess.setValue(u,temp);
  temp = (excess.getValue(v) + d);
  excess.setValue(v,temp);
  temp = residual_capacity.getValue(forward_edge) - d;
  residual_capacity.setValue(forward_edge,temp);
  temp = residual_capacity.getValue(backward_edge) + d;
  residual_capacity.setValue(backward_edge,temp);
  if (excess.getValue(v) > 0 && v != src && v != snk )
  {
    g.frontier_push(v, world);

  }

}
void relabel(Graph& g, int u, EdgeProperty<int>& residue, NodeProperty<int>& label, 
  Container<int>& count, boost::mpi::communicator world )
{
  printf ("relabeling %d\n", u) ;
  int new_label = g.num_nodes() + 2;
  for (int v : g.getNeighbors(u)) 
  {
    Edge residual_capacity = g.get_edge(u, v);
    if (residue.getValue(residual_capacity) > 0 )
    {
      if (new_label > label.getValue (v) )
      {
        new_label = label.getValue (v);
      }
    }
  }


  if (new_label < g.num_nodes()+1)
  { 
    int oldIdx = label.getValue(u) ;
    int oldVal = count.getValue (g.get_node_owner(u), oldIdx) ;
    count.setValue (g.get_node_owner(u), oldIdx, oldVal - 1) ;
    label.setValue (u,new_label + 1);
    int newVal = count.getValue (g.get_node_owner(u), new_label+1) ;
    count.setValue (g.get_node_owner(u), new_label+1, newVal + 1) ;
  }

}
void discharge(Graph& g, int u, NodeProperty<int>& label, NodeProperty<int>& excess, 
  NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, Container<int>& count, boost::mpi::communicator world )
{
    printf ("discharging %d at height = %d with excess = %d \n", u, label.getValue (u), excess.getValue (u)) ;
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
      int prevValue = label.getValue (u) ;
      relabel(g,u,residue,label,count, world);
      if (label.getValue (u) == prevValue) break ;


    }
  }

}

void fixGap(Graph &g, Container<int>& count, NodeProperty<int>& label, boost::mpi::communicator world )
{
  printf ("fixing gap \n") ;
  int gap = count.getIdx(0);
  if (gap == -1) return ;
  printf ("found gap of %d\n", gap); 
  int t = 0 ;
  world.barrier();
     for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (label.getValue(v) >= gap )
      {
        label.setValue(v,g.num_nodes()+1);
      }
    }

  world.barrier () ;
}

void do_max_flow(Graph& g, int source, int sink, NodeProperty<int>& label, 
  NodeProperty<int>& excess, NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, boost::mpi::communicator world )
{
  src = source, snk = sink ;
  residue = g.weights;
  label.attachToGraph(&g, (int)0);
  excess.attachToGraph(&g, (int)0);
  curr_edge.attachToGraph(&g, (int)0);
  Container<int> count;
  count.assign(g.num_nodes( )+3,0, MPI_COMM_WORLD);

  int temp = 0;
  int res = 0;
  world.barrier();
  // if ( world.rank () == g.get_node_owner (source) )
   //{ 
    for (int v:g.getNeighbors(source))
    {
      Edge e = g.get_edge(source, v);
      temp = residue.getValue(e) ;
      res = res + temp ;
      excess.setValue(v, temp);
      if (world.rank () == g.get_node_owner (v) && v != snk ) {
        g.frontier_push (v, world) ;
      }
    }
  //}
  int rank ;
  MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
  world.barrier () ;
  MPI_Barrier (MPI_COMM_WORLD) ;
  printf ("rank %d in barrier region\n", rank) ;
  count.printArr () ;
  MPI_Barrier (MPI_COMM_WORLD) ;
  world.barrier () ;

  int x = count.getValue (g.get_node_owner(sink), 0) ;
  printf ("checking for ok before set value at height 0 %d \n", x) ;

  label.setValue(source,g.num_nodes( ));
  count.setValue(g.get_node_owner(source), g.num_nodes(), 1) ;
  count.setValue(g.get_node_owner(sink), 0, g.num_nodes()-1) ;
  x = count.getValue (g.get_node_owner(sink), 0) ;
  printf ("checking for ok set value at height 0 %d \n", x) ;
  while (!g.frontier_empty(world)){
    int u = g.frontier_pop_local(world);
    if (u != -1) 
    discharge(g,u,label,excess,curr_edge,residue, count, world);
    fixGap(g,count,label, world);
    // printf ("fixed gap\n") ;
    world.barrier () ;
  }

  world.barrier () ;
  int ans = excess.getValue (sink) ;
  printf ("excess at sink = %d\n", ans) ;
}
