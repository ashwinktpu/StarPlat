#include"anupDslAnalysis.h"



#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define ENABLE_DEBUG to turn on debugging; comment it out to disable
// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
    #define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...) // Define it as an empty macro when debugging is disabled
#endif

#endif // DEBUG_H


int do_max_flow(Graph& g, int source, int sink, int kernel_parameter
  , boost::mpi::communicator world )
{
	kernel_parameter= 12 * g.num_nodes () + g.num_edges () ;
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
  label.setValue(source,g.num_nodes( ));
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  Container<int> count;
  count.assign(g.num_nodes( ) + 2,0, world);

  EdgeProperty<int> residual_capacity;
  residual_capacity.attachToGraph(&g, 0);
  residual_capacity = g.weights ;
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
    int x = kernel_parameter;
    int numDischarged = 0 ;
    do
    {
      numDischarged = 0 ;
      x = x + -1;
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (excess.getValue(v) > 0 && v != source && v != sink && label.getValue(v) < g.num_nodes()+1)
        {
          int hh = INT_MAX;
          int lv = -1;
          int _t2 = 0 ;
          DEBUG_LOG ("Discharging %d with excess %d at height %d  \n", v, excess.getValue(v), label.getValue(v)) ;
          numDischarged++ ;

          for (int vv : g.getNeighbors(v)) 
          {
            vv = g.get_other_vertex(v, _t2);
            Edge forward_edge = g.get_edge_i(v, _t2);
            DEBUG_LOG ("Trying %d in neighborhood of %d at height %d residual_graph %d\n", vv, v, label.getValue (vv), residual_capacity.getValue (forward_edge)) ;
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
                DEBUG_LOG ("Pushing %d from %d to %d\n", d, v, vv) ;
                excess.atomicAdd (v, -d);
                excess.atomicAdd (vv, d);
                residual_capacity.atomicAdd (forward_edge, -d);
                residual_capacity.atomicAdd (backward_edge, d);
              }
              else
              {
                hh = std::min(hh,label.getValue(vv));
                DEBUG_LOG ("Could not find anywhere to push %d, relabeling to %d + 1\n",v,hh) ;
              }
            }
            _t2++;
          }


          if (excess.getValue(v) > 0 && hh < g.num_nodes( ) + 1 )
          {
            DEBUG_LOG ("Relabeling %d from %d to %d\n", v, label.getValue(v), hh+1) ;
            int currLab = label.getValue(v);
            count.atomicAdd (currLab, -1);
            label.setValue(v,hh + 1);
            count.atomicAdd (hh + 1, 1);
          }
        }
      }
      world.barrier();


    }
    while(x > 0);int gap = count.getIdx(0);
    DEBUG_LOG ("FOUND gap = %d by proc %d\n", gap, world.rank ()) ;
    if (gap > 0 )
    {
      world.barrier();
      DEBUG_LOG ("All processes started looking at gap this is process %d\n", world.rank()) ;
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (v != sink && v != source && label.getValue(v) >= gap )
        {
          label.setValue(v,g.num_nodes( ) + 1);
        }
      }
      world.barrier();


    }
    // if (excess.getValue(source) + excess.getValue(sink) == excesstotal )
    MPI_Allreduce (&numDischarged, &numDischarged, 1,MPI_INT,  MPI_SUM, MPI_COMM_WORLD) ;
    if (numDischarged == 0) 
    {
      flag = false;
    }
  }
  while(flag);
  return excess.getValue (sink) ;
}
