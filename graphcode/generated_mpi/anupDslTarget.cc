#include"anupDsl.h"


// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define ENABLE_DEBUG to turn on debugging; comment it out to disable
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
    #define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG(fmt, ...) // Define it as an empty macro when debugging is disabled
#endif

#endif // DEBUG_H

int do_max_flow(Graph& g, int source, int sink, int kernel_parameter
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
        excesstotal = excesstotal + d ;
      excess.atomicAdd (vv, d);
      residual_capacity.atomicAdd (forward_edge, -d);
      residual_capacity.atomicAdd (backward_edge, d);
    }


  }
}
world.barrier();
DEBUG ("initialization OK\n") ;

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
        DEBUG ("discharging %d from rank %d\n", v, world.rank ()) ;
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
            DEBUG ("pushing from %d to %d a value of %d  from rank %d\n", v, lv, d, world.rank ()) ;
            excess.atomicAdd (v, -d);
            excess.atomicAdd (lv, d);
            residual_capacity.atomicAdd (forward_edge, -d);
            residual_capacity.atomicAdd (backward_edge, d);
            DEBUG ("complete pushing from %d to %d a value of %d  from rank %d\n", v, lv, d, world.rank ()) ;
          }
        }
        else
        if (lv != -1 )
        {
          DEBUG ("Relabeling %d to %d from rank %d \n", v , hh+1, world.rank () ) ;
          label.setValue(v,hh + 1);
          DEBUG ("completed Relabeling %d to %d from rank %d \n", v , hh+1, world.rank () ) ;
        }

        DEBUG ("discharging %d from rank %d OK\n", v, world.rank ()) ;
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
excess.leaveAllSharedLocks () ;
label.leaveAllSharedLocks () ;
residual_capacity.leaveAllSharedLocks () ;
return excess.getValue (sink) ;
}
