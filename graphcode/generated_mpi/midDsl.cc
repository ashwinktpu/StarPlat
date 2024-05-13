#include"midDsl.h"

void do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world )
{
  bool flag = true;
  do
  {
    do
    {
      int x = kernel_parameter;
      x = x + -1;
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (v.excess > 0 && v != source && v != sink )
        {
          int hh = INT_MAX;
          int lv = -1;
          int _t2 = -1 ;

          for (int vv : g.getNeighbors(v)) 
          _t2 ++ ;

          {
            Edge forward_edge = g.get_edge_i(v, _t2);
            if (e.residual_capacity > 0 )
            {
              if (vv.label == v.label + 1 )
              {
                Edge backward_edge = g.get_edge(vv, v);
                int d = forward_edge.residual_capacity;
                if (v.excess < d )
                {
                  d = v.excess;
                }
                excess.atomicAdd (v, -d);
                excess.atomicAdd (vv, d);
                residual_capacity.atomicAdd (forward_edge, d);
                residual_capacity.atomicAdd (backward_edge, -d);
              }
              else
              {
                hh = min(hh,vv.label, world);
              }
            }
          }


        }
      }
      world.barrier();


    }
    while(x > 0);if (source.excess + sink.excess == excesstotal )
    {
      flag = false;
    }
  }
  while(flag);
}
