#include"anupDsl.h"
// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define ENABLE_DEBUG to turn on debugging; comment it out to disable
// #define ENABLE_DEBUG

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
  label.attachToGraph(&g, 0);
  label.setValue(source,g.num_nodes( ));
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  Container<int> count;
  count.assign(g.num_nodes( ) + 2,0, world);

  count.setValue(g.get_node_owner(source), g.num_nodes(), 1) ;
  count.setValue(g.get_node_owner(sink), 0, g.num_nodes()-1) ;

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
  label.setValue(source,g.num_nodes( ));

  int excesstotal_temp = excesstotal;
  MPI_Allreduce(&excesstotal_temp,&excesstotal,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);


  bool flag = true;
 int __count = 0 ;
  do
  {
    int x = kernel_parameter;
    do
    {
      __count = 0 ;
      x = x + -1;
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (excess.getValue(v) > 0 && v != source && v != sink && label.getValue (v) < g.num_nodes()+1)
        {
          __count++ ;
          DEBUG ("Discharging %d with excess %d at height %d\n", v, excess.getValue (v), label.getValue (v)) ;
          int hh = INT_MAX;
          int h1 = INT_MAX ;
          int lv = -1;
          for (int vv : g.getNeighbors(v)) 
          {
            Edge e = g.get_edge(v, vv);
            int xxx = residual_capacity.getValue(e);
            if (label.getValue(vv) == label.getValue(v)-1 && xxx > 0 )
            {
              lv = vv;
              h1 = label.getValue (vv) ;
            }
            if (label.getValue(vv) < hh && xxx >0) {
              hh = label.getValue(vv) ;
            }
          }

          DEBUG ("Smallest Height in neighborhood = %d\n", hh) ;


          if (label.getValue(v) == h1+1 && lv != -1 )
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
              DEBUG ("Sending from %d to %d a value of %d\n", v, lv, d) ;
              excess.atomicAdd (v, -d);
              excess.atomicAdd (lv, d);
              residual_capacity.atomicAdd (forward_edge, -d);
              residual_capacity.atomicAdd (backward_edge, d);
              DEBUG ("OK Sending from %d to %d a value of %d\n", v, lv, d) ;
            }
          }
          else
          if (hh <= g.num_nodes () + 1)
          {
            if (hh == g.num_nodes() +1 ) hh-- ;
            DEBUG ("Relabeling %d from %d to %d\n", v, label.getValue (v), hh) ;
            int currLab = label.getValue(v);
            count.atomicAdd (currLab, 1);
            label.setValue(v,hh + 1);
            count.atomicAdd (hh + 1, 1);
            DEBUG ("Relabeled %d from %d to %d\n", v, currLab, label.getValue (v)) ;
          }
        }
      }
      world.barrier();
    }
    while(x > 0);
    DEBUG ("what is the gap ?\n") ;
    int gap = count.getIdx(0);
    DEBUG ("gap = %d at rank %d\n", gap, world.rank ()) ;
    if (gap >= 0 )
    {
      world.barrier();
      DEBUG ("gap = %d at rank %d from inside the barrier \n", gap, world.rank ()) ;
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (v != sink && v != source && label.getValue(v) >= gap )
        {
          DEBUG ("Sending %d to eternity\n", v) ;
          label.setValue(v,g.num_nodes( ) + 1);

        }
      }
      DEBUG ("gap = %d at rank %d about to exit the barrier \n", gap, world.rank ()) ;
      world.barrier();
      DEBUG ("gap = %d at rank %d about exited the barrier \n", gap, world.rank ()) ;
    }
    //if (excess.getValue(source) + excess.getValue(sink) == excesstotal )
    DEBUG ("count value = %d\n", __count) ;
    MPI_Allreduce(&__count,&__count,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    if (__count == 0) 
    {
      flag = false;
    }
  }
  while(flag);
  return excess.getValue (sink) ;
}
