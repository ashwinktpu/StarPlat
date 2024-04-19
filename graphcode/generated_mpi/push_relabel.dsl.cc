#include"push_relabel.dsl.h"

void checkAssert (bool cond, int u ) {

	if (!cond) {
		printf ("%d does not belong to this proces\n", u) ;
		assert (false) ;
	}
}

int src, snk ;
void push__(Graph& g, int u, int v_idx, NodeProperty<int>& excess, 
  EdgeProperty<int>& residual_capacity, boost::mpi::communicator world )
{
  int v = g.get_other_vertex (u, v_idx) ;
  Edge forward_edge = g.get_edge_i(u, v_idx);
//   printf (" Retreiving back edge %d -> %d\n", v, u) ;
  Edge backward_edge = g.get_edge(v, u);
  int d = std::min(excess.getValue(u),residual_capacity.getValue(forward_edge));
  int temp = 0 ;
  // printf ("pushing flow of value %d from %d to %d initially set to %d by process %d remainder = %d\n",d,u,v,excess.getValue (v), world.rank (), excess.getValue(u)); 
  // temp = (excess.getValue(u) - d);
  excess.atomicAdd(u,-d);
  // temp = (excess.getValue(v) + d);
  excess.atomicAdd(v,d);
  //printf ("pushed flow of value %d from %d to %d, new value = %d, we asked it to be set to %d by process %d\n",d,u,v,excess.getValue(v), temp, world.rank () ); 
  // temp = residual_capacity.getValue(forward_edge) - d;
  residual_capacity.atomicAdd(forward_edge,-d);
  // temp = residual_capacity.getValue(backward_edge) + d;
  residual_capacity.atomicAdd(backward_edge,d);
  if (excess.getValue(v) > 0 && v != src && v != snk )
  {
    g.frontier_push(v, world);

  }

}
void relabel(Graph& g, int u, EdgeProperty<int>& residue, NodeProperty<int>& label, 
  Container<int>& count, boost::mpi::communicator world )
{
   // printf ("relabeling %d\n", u) ;
  int x = label.getValue (u) ;
  int new_label = g.num_nodes() + 2;

  int vIdx = 0;
  for (int v : g.getNeighbors(u)) 
  {
    Edge residual_capacity = g.get_edge_i(u, vIdx);
  //  printf ("residual capacity at %d -> %d = %d\n", u, v, residue.getValue(residual_capacity));
    if (residue.getValue(residual_capacity) > 0)
    {
      if (new_label > label.getValue (v) )
      {
        new_label = label.getValue (v);
      }
    }
    vIdx++ ;
  }

  /*// Experiment
  for (int v : g.getNeighbors(u)) 
  {
    Edge residual_capacity = g.get_edge(u, v);
  //  printf ("residual capacity at %d -> %d = %d\n", u, v, residue.getValue(residual_capacity));
    if (residue.getValue(residual_capacity) > 0)
    {
      if (new_label > label.getValue (v) )
      {
        new_label = label.getValue (v);
      }
    }
  }

*/

  // printf ("new laebel to be assigned = %d\n", new_label+1) ;
  if (new_label < g.num_nodes()+1 && new_label + 1> x )
  { 
    int oldIdx = label.getValue(u) ;
		count.atomicAdd (oldIdx, -1) ;
		count.atomicAdd (new_label+1, 1) ;

    label.setValue (u,new_label + 1);
 //   printf ("new set to be assigned = %d\n", label.getValue (u)) ;
//		g.frontier_push (u, world);
  }

}
void discharge(Graph& g, int u, NodeProperty<int>& label, NodeProperty<int>& excess, 
  NodeProperty<int>& curr_edge, EdgeProperty<int>& residue, Container<int>& count, boost::mpi::communicator world )
{
  // printf ("discharging %d at height = %d with excess = %d \n", u, label.getValue (u), excess.getValue (u)) ;
  while (excess.getValue(u) > 0 ){
    
    for (int vIdx = curr_edge.getValue(u); vIdx < g.num_out_nbrs (u); vIdx++) {
      Edge current_edge = g.get_edge_i (u,vIdx) ;
      int v = g.get_other_vertex (u,vIdx) ;
      if (excess.getValue(u) > 0 && residue.getValue(current_edge) > 0 && label.getValue(u) == label.getValue(v) + 1 )
      {
      // printf ("Forward_edge = %d -> %d\n", u, v);
        push__ (g,u,vIdx,excess,residue,world) ;
      }
    }
      
    /* experiment : 
    for (int v : g.getNeighbors(u)) 
    {
      Edge current_edge = g.get_edge(u, v);
      if (excess.getValue(u) > 0 && residue.getValue(current_edge) > 0 && label.getValue(u) == label.getValue(v) + 1 )
      {
        push__(g,u,v,excess,residue, world);

      }
    }
    */


    if (excess.getValue(u) > 0 )
    {
      int prevValue = label.getValue (u) ;
      relabel(g,u,residue,label,count, world);
      // printf ("relabeled %d from %d to %d\n", u, prevValue, label.getValue(u)) ;
      if (label.getValue (u) == prevValue) {
//				if (prevValue <= g.num_nodes()+1) g.frontier_push (u, world) ; 
				break ;}
      curr_edge.setValue (u, 0) ;

    }
  }

}

void fixGap(Graph &g, Container<int>& count, NodeProperty<int>& label, boost::mpi::communicator world )
{
 // printf ("fixing gap \n") ;
	world.barrier () ;
  int gap = count.getIdx(0);
  if (gap == -1) return ;
  // printf ("found gap of %d\n", gap); 
  int t = 0 ;
  world.barrier();
     for (int v = g.start_node(); v <= g.end_node(); v ++) 
    {
      if (v != snk && v!= src && label.getValue(v) >= gap )
      {
    //    printf ("sent %d to unreachable\n", v) ;
        label.setValue(v,g.num_nodes()+2);
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
  // printf ("here\n");
  if ( world.rank () == g.get_node_owner (source) )
   { 
    for (int v:g.getNeighbors(source))
    {
      Edge e = g.get_edge(source, v);
      temp = residue.getValue(e) ;
      res = res + temp ;
			//printf ("setting excess value of %d to %d\n", v, temp) ;
      excess.setValue(v, temp);
			// printf ("set excess value of %d to %d\n", excess.getValue (v), temp) ;
	  if (v != snk && v != src) {
		 	//printf ("pushing %d into queue with excess %d\n", v, temp) ;
      	g.frontier_push (v, world) ;
	  }
    }
  }
  // printf ("no issue with source push\n");
  world.barrier () ;
  int activate = 0 ;
  label.setValue(source,g.num_nodes( ));
  count.setValue(g.get_node_owner(source), g.num_nodes(), 1) ;
  count.setValue(g.get_node_owner(sink), 0, g.num_nodes()-1) ;
	world.barrier () ;
  while (!g.frontier_empty(world)){
	  // printf ("going to pop\n") ;
    int u = g.frontier_pop_local(world);
	  checkAssert ((u == -1 || g.get_node_owner (u) == world.rank ()), u) ;
    if (u != -1) 
    discharge(g,u,label,excess,curr_edge,residue, count, world);
    if (activate == 0) {
    fixGap(g,count,label, world);
    activate = 100 ;}
    else {activate--;}
  //  printf ("fat barriers started\n") ;
    // excess.fatBarrier () ;
    // residue.fatBarrier () ;
    world.barrier () ;
  //  printf ("fat barriers ended\n") ;
  }

  world.barrier () ;
  int ans = excess.getValue (sink) ;
	/*if (world.rank () == 0) {
		for (int i=0;i<500; i++) {
			// printf ("excess at %d is %d at height %d\n", i, excess.getValue (i), label.getValue (i) ) ;	
		}
	}*/
  printf ("excess at sink = %d\n", ans) ;
}
