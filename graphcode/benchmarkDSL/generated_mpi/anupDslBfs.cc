#include"anupDslBfs.h"

void do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world )
{
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
  if(world.rank() == g.get_node_owner(source))
  {
    label.setValue(source,g.num_nodes( ));
  }
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  EdgeProperty<int> residual_capacity;
  residual_capacity.attachToGraph(&g, (int)g.getEdgeLen( ));
  int excesstotal = 0;
  NodeProperty<int> visit;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (v == source )
    {
      for (int vv : g.getNeighbors(v)) 
      _t1 ++ ;

      {
        Edge forward_edge = g.get_edge_i(v, _t1);
        Edge backward_edge = g.get_edge_r_i(_t1, v);
        int d = residual_capacity.getValue(forward_edge);
        excesstotal = excesstotal + d ;
      excess.atomicAdd (vv, d);
      residual_capacity.atomicAdd (forward_edge, -d);
      residual_capacity.atomicAdd (backward_edge, d);
    }


  }
}
world.barrier();

int excesstotal_temp = excesstotal;
MPI_Allreduce(&excesstotal_temp,&excesstotal,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);


bool flag = true;
do
{
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  _t1 ++ ;

  {
    Edge forward_edge = g.get_edge_i(v, _t1);
    Edge backward_edge = g.get_edge_r_i(_t1, v);
    int d = residual_capacity.getValue(forward_edge);
    excesstotal = excesstotal + d ;
  excess.atomicAdd (vv, d);
  residual_capacity.atomicAdd (forward_edge, -d);
  residual_capacity.atomicAdd (backward_edge, d);
}
world.barrier();


world.barrier();
for (int v = g.start_node(); v <= g.end_node(); v ++) 
_t1 ++ ;

{
  Edge forward_edge = g.get_edge_i(v, _t1);
  Edge backward_edge = g.get_edge_r_i(_t1, v);
  int d = residual_capacity.getValue(forward_edge);
  excesstotal = excesstotal + d ;
excess.atomicAdd (vv, d);
residual_capacity.atomicAdd (forward_edge, -d);
residual_capacity.atomicAdd (backward_edge, d);
}
world.barrier();


visit.attachToGraph(&g, (int)0);
if(world.rank() == g.get_node_owner(sink))
{
label.setValue(sink,0);
}
if(world.rank() == g.get_node_owner(sink))
{
visit.setValue(sink,1);
}
if (excess.getValue(source) + excess.getValue(sink) == excesstotal )
{
flag = false;
}
}
while(flag);
}
