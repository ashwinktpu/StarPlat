#include"anupDsl.h"

void do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world )
{
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
  // if(world.rank() == g.get_node_owner(source))
  // {
    label.setValue(source,g.num_nodes( ));
  // }
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  EdgeProperty<int> residual_capacity = g.weights;
  // residual_capacity.attachToGraph(&g, (int)g.getEdgeLen( ));
  int excesstotal = 0;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (v == source )
    {
      for (int vv : g.getNeighbors(v)) 
      {
        int x = source;
        Edge forward_edge = g.get_edge(x, vv);
        Edge backward_edge = g.get_edge(vv, x);
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
  {
    if (excess.getValue(v) > 0 && v != source && v != sink )
    {
      int x = kernel_parameter;
      do
      {
        x = x + -1;
        int hh = INT_MAX;
        int lv = -1;
        for (int vv : g.getNeighbors(v)) 
        {
          Edge e = g.get_edge(v, vv);
          int xxx = residual_capacity.getValue(e);
          if (label.getValue(vv) < hh && xxx > 0 )
          {
            lv = vv;
            hh = label.getValue(vv);
          }
        }


        if (label.getValue(v) > hh && lv != -1 )
        {
          Edge current_edge = g.get_edge(v, lv);
          int cec = residual_capacity.getValue(current_edge);
          if (excess.getValue(v) > 0 && cec > 0 )
          {
            Edge forward_edge = g.get_edge(v, lv);
            Edge backward_edge = g.get_edge(lv, v);
            int fec = residual_capacity.getValue(forward_edge);
            int bec = residual_capacity.getValue(backward_edge);
            int d = fec;
            if (excess.getValue(v) < fec )
            {
              d = excess.getValue(v);
            }
            excess.atomicAdd (v, -d);
            excess.atomicAdd (lv, d);
            residual_capacity.atomicAdd (forward_edge, -d);
            residual_capacity.atomicAdd (backward_edge, d);
          }
        }
        else
        if (lv != -1 )
        {
          label.setValue(v,hh + 1);
        }
      }
      while(x > 0);}
  }
  world.barrier();


  if (excess.getValue(source) + excess.getValue(sink) == excesstotal )
  {
    flag = false;
  }
}
while(flag);
}
