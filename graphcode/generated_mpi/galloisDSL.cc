#include"galloisDSL.h"
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define ENABLE_DEBUG to turn on debugging; comment it out to disable
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
    #define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: PROCNO:%d %s:%d:%s(): " fmt, \
        world.rank (),__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...) // Define it as an empty macro when debugging is disabled
#endif

#endif // DEBUG_H

int maxFlow(Graph& g, int source, int sink, boost::mpi::communicator world )
{
  NodeProperty<int> label;
  label.attachToGraph(&g, (int)0);
  label.setValue (source, g.num_nodes ()) ;
  NodeProperty<int> excess;
  excess.attachToGraph(&g, (int)0);
  EdgeProperty<int> residual_capacity;
  residual_capacity.attachToGraph(&g, (int)0);
  residual_capacity = g.weights;
  Container<int> count;
  count.assign(g.num_nodes( ) + 2,0, world);

  if (g.get_node_owner (source) == world.rank ()) {
    for (int v : g.getNeighbors (source)) {
      Edge e = g.get_edge (source,v) ;
      excess.atomicAdd (v, residual_capacity.getValue(e)) ;
      residual_capacity.setValue (e,0) ;
      Edge r_e = g.get_edge (v, source) ;
      residual_capacity.setValue (r_e, excess.getValue (v)) ;
      if (excess.getValue(v) > 0) g.frontier_push (v, world) ;
    }
  }
  world.barrier () ;


  int activate = 100;
  while (!g.frontier_empty( world) ){ //remove world from definition.
    int u = g.frontier_pop_local( world); // remove world from definition.
    int newLabel = g.num_nodes( ) + 1;
    int newLabel_leader_rank = -1 ;
    int prevVal ;
    if (u != -1) 
    prevVal = excess.getValue (u) ;


    world.barrier();
    if (u != -1 && u != source) { // add support for u = -1
    DEBUG_LOG ("Discharging %d with excess %d at height %d  \n", u, excess.getValue(u), label.getValue(u)) ;
    if ( world.rank () == g.get_node_owner (u) )
     { 

     int _t1 = 0 ;
     for (int v:g.getNeighbors(u))

      {
        Edge e = g.get_edge_i (u, _t1) ;
        v = g.get_other_vertex (u, _t1) ;
        if (residual_capacity.getValue (e) > 0) {
          newLabel_leader_rank = world.rank();
    //      DEBUG_LOG ("edge from %d at height %d to %d at height %d newLabel initally at : %d\n", u, label.getValue(u), v, label.getValue (v), newLabel) ;
          newLabel = std::min(newLabel,label.getValue(v)); // std:: not coming. Additional world
     //     DEBUG_LOG ("edge from %d at height %d to %d at height %d newLabel set to : %d\n", u, label.getValue(u), v, label.getValue (v), newLabel) ;
        }
        _t1++ ;
      }

    } // add support for u = -1 
    }
    // if (u!=-1)
    // DEBUG_LOG ("for u %d,height %d newLabel set to : %d\n", u, label.getValue(u), newLabel) ;
    int newLabel_leader_rank_temp = newLabel_leader_rank;
//    MPI_Allreduce(&newLabel_leader_rank_temp,&newLabel_leader_rank,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
//     MPI_Bcast(&newLabel,1,MPI_INT,newLabel_leader_rank,MPI_COMM_WORLD);
    MPI_Allreduce(&newLabel,&newLabel,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
    // if (u!=-1) 
    // DEBUG_LOG ("for u %d,height %d post reduction newLabel set to : %d\n", u, label.getValue(u), newLabel) ;
    world.barrier () ; // add ;.

    if (u != -1 && u != source ) {

      DEBUG_LOG ("Relabeling %d from %d to %d\n", u, label.getValue(u), newLabel+1) ;
      // count.atomicAdd (u.label, 1); // expressions not being solved inside.
      count.atomicAdd (label.getValue(u), -1); // expressions not being solved inside.
      label.setValue(u,newLabel + 1);
      // count.atomicAdd (u.label, 1);
      count.atomicAdd (label.getValue(u), 1); // expressions not being solved inside.
      DEBUG_LOG ("Relabeled %d to %d\n", u, label.getValue(u)) ;

    }
    world.barrier();
    int _t1 = 0 ;
    if (u != -1) { // add support for u = - 1.
    if ( world.rank () == g.get_node_owner (u) )
     { for (int v:g.getNeighbors(u))
      {
        v = g.get_other_vertex(u, _t1);
        Edge forward = g.get_edge_i(u, _t1);
        // double declaration of v. Why ?
        // int v = g.get_other_vertex(u, _t1);
        // implement get_edge_r_i
        // Edge backward = g.get_edge_r_i(_t1, u);
        if (label.getValue (u) == label.getValue(v) + 1) {
          Edge backward = g.get_edge(v, u);
          int d = std::min(residual_capacity.getValue (forward),excess.getValue(u));
          if (d > 0) {
          DEBUG_LOG ("Pushing %d from %d at height %d to %d at height %d\n", d, u, label.getValue (u),v,  label.getValue (v)) ;
          excess.atomicAdd (u, -d);
          excess.atomicAdd (v, d);
          residual_capacity.atomicAdd (forward, -d);
          residual_capacity.atomicAdd (backward, d);
          if (d > 0 && v != source && v != sink )
          {
            g.frontier_push(v, world); // change v to u.
            DEBUG_LOG ("pushed %d into queue\n", v) ;

          }
          }
        }
        _t1++;
      }
    } // add support for u = -1.
    }
    world.barrier () ;

    if (u!=-1 &&  u!= source && excess.getValue(u) > 0 && excess.getValue (u) != prevVal)
    {
      g.frontier_push(u, world);

    }
    DEBUG_LOG ("activation value = %d\n", activate) ;
    if (activate-- == 0 )
    {
      int gap = count.getIdx(0);
      DEBUG_LOG ("found a gap at %d\n", gap) ;
      world.barrier();
      for (int v = g.start_node(); v <= g.end_node(); v ++) 
      {
        if (label.getValue(v) >= gap )
        {
          label.setValue(v,g.num_nodes( ) + 1);
        }
      }
      world.barrier () ;
      activate=100 ;
    }
    world.barrier () ;
  }
  return excess.getValue (sink) ;
}
