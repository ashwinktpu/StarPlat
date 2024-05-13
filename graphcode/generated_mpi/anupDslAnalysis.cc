#include"anupDslAnalysis.h"

void do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world )
{
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
  label.setValue(source,g.num_nodes( ));
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  Container<int> count;
  count.assign(g.num_nodes( ) + 2,0, world);

  EdgeProperty<int> residual_capacity;
  residual_capacity.attachToGraph(&g, (int)0);
  residual_capacity = g.weights;
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
        excesstotal = ( excesstotal + d) ;
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
    int numDischarged = 0;
    int x = kernel_parameter;
    do
    {
      numDischarged = 0;
      x = x + -1;
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (excess.getValue(v) > 0 && v != source && v != sink && label.getValue(v) < g.num_nodes( ) + 1 )
        {
          int hh = INT_MAX;
          int lv = -1;
          numDischarged = ( numDischarged + 1) ;
          int _t1 = 0 ;

          for (int vv : g.getNeighbors(v)) 
          {
            int vv = g.get_other_vertex(v, _t1);
            Edge forward_edge = g.get_edge_i(v, _t1);
            if (residual_capacity.getValue(forward_edge) > 0 )
            {
              if (label.getValue(vv) == label.getValue(v) - 1 )
              {
                Edge backward_edge = g.get_edge(vv, v);
                int d = residual_capacity.getValue(forward_edge);
                if (excess.getValue(v) < d )
                {
                  d = excess.getValue(v);
                }
                excess.atomicAdd (v, -d);
                excess.atomicAdd (vv, d);
                residual_capacity.atomicAdd (forward_edge, -d);
                residual_capacity.atomicAdd (backward_edge, d);
              }
              else
              {
                hh = min(hh,label.getValue(vv), world, world);
              }
            }
            _t1++;
          }


          if (excess.getValue(v) > 0 && hh < g.num_nodes( ) + 1 )
          {
            int currLab = label.getValue(v);
            count.atomicAdd (currLab, -1);
            label.setValue(v,hh + 1);
            count.atomicAdd (hh + 1, 1);
          }
        }
      }
      world.barrier();

      int numDischarged_temp = numDischarged;
      MPI_Allreduce(&numDischarged_temp,&numDischarged,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);


    }
    while(x > 0);int gap = count.getIdx(0, world);
    if (gap > 0 )
    {
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (v != sink && v != source && label.getValue(v) >= gap )
        {
          label.setValue(v,g.num_nodes( ) + 1);
        }
      }
      world.barrier();


    }
    if (numDischarged == 0 )
    {
      flag = false;
    }
  }
  while(flag);
}
