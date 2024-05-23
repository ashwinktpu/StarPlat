#include"anupDslAnalysis.h"
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define ENABLE_DEBUG to turn on debugging; comment it out to disable
// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
    #define DEBUG_LOG(fmt, ...) fprintf(stderr, "RANK = %d , DEBUG:  %s:%d:%s():  " fmt, \
        world.rank (), __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...) // Define it as an empty macro when debugging is disabled
#endif

#endif // DEBUG_H

int do_max_flow(Graph& g, int source, int sink, int kernel_parameter
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

 	int numDischarged = 0;
	int toDischarge = 0 ;
  do
  {
		DEBUG_LOG ("HERE\n") ;
    int x = kernel_parameter;
    world.barrier();
		if (!toDischarge) {
		for (int v = g.start_node(); v <= g.end_node(); v ++) 
		{
			if (excess.getValue(v) > 0 && v != source && v != sink && label.getValue(v) < g.num_nodes()+1)
			{
				g.frontier_push(v, world);

			}
		}
		toDischarge = 1;
		}
		world.barrier();
		numDischarged=0 ;

		DEBUG_LOG ("count down to kernel shut down : %d rank : %d\n", x, world.rank ()) ;


		while (!g.frontier_empty(world) ){
			int v = g.frontier_pop_local (world) ;
			int d, __d, _d ;
			int _t1 , hh;
			int l_v, l_vv ;
			int currLab ; 
			if (v == -1) goto exitLoop ;
			DEBUG_LOG ("Discharging %d with excess %d at height %d by rank %d \n", v, excess.getValue(v), label.getValue(v), world.rank ()) ;
			__d = excess.getValue (v) ;
			while (__d > 0) {
				_t1 = 0 ;
				hh = INT_MAX;
				d = __d ;
				for (int vv : g.getNeighbors(v)) 
				{
					vv = g.get_other_vertex(v, _t1);
					Edge forward_edge = g.get_edge_i(v, _t1);
					DEBUG_LOG ("Trying %d in neighborhood of %d at height %d residual_graph %d at rank %d\n", vv, v, label.getValue (vv), residual_capacity.getValue (forward_edge), world.rank () ) ;
					_d = residual_capacity.getValue (forward_edge) ;
					if (_d > 0 )
					{
						l_vv = label.getValue(vv) ;
						l_v = label.getValue (v) ;
						if (l_vv == l_v - 1 )
						{
							Edge backward_edge = g.get_edge(vv, v);
							d = std::min (d, _d) ;
							DEBUG_LOG ("Pushing %d from %d to %d\n", d, v, vv) ;
							excess.atomicAdd (v, -d);
							excess.atomicAdd (vv, d);
							residual_capacity.atomicAdd (forward_edge, -d);
							residual_capacity.atomicAdd (backward_edge, d);
							__d-=d ;
							_d-=d ;
							numDischarged = ( numDischarged + 1) ;
							if (d > 0 && vv != source && vv != sink && l_vv < g.num_nodes () + 1)
							{
								DEBUG_LOG ("%d is pushing %d into frontier\n", v, vv) ;
								g.frontier_push(vv, world);
							}
						}
						else
						{
							hh = std::min(hh,l_vv);
							DEBUG_LOG ("Could not find anywhere to push %d, relabeling to %d + 1\n",v,hh) ;
						}
					}
					DEBUG_LOG ("Came out Trying %d in neighborhood of %d at height %d residual_graph %d at rank %d\n", vv, v, label.getValue (vv), residual_capacity.getValue (forward_edge), world.rank () ) ;
					_t1++;
				}

				DEBUG_LOG ("Rank %d Got out of neighbour iter\n", world.rank () ) ;

				if (__d > 0 && hh <= g.num_nodes( ) + 1 )
				{
					hh = std::min (hh, g.num_nodes()) ;
					DEBUG_LOG ("Rank %d : Relabeling %d from %d to %d\n", world.rank () ,v, label.getValue(v), hh+1) ;
					currLab = label.getValue(v);
					if (currLab == hh+1 || hh >= g.num_nodes ()) goto exitLoop;
					count.atomicAdd (currLab, -1);
					label.setValue(v,hh + 1);
					count.atomicAdd (hh + 1, 1);
					DEBUG_LOG ("Rank %d : Relabelled %d to %d\n", world.rank(), v , label.getValue(v) ) ;
				}
			}
			DEBUG_LOG (" x = %d\n", x) ;
exitLoop : x = x + -1;
			world.barrier () ;
			if (x == 0) {
				x = kernel_parameter ;
				int gap = count.getIdx(0);
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
			}
		}
		int numDischarged_temp = numDischarged;
		MPI_Allreduce(&numDischarged_temp,&numDischarged,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
		DEBUG_LOG ("numdischarged = %d\n", numDischarged) ;

		if (numDischarged == 0 )
		{
			flag = false;
		}
  }
  while(flag);return excess.getValue(sink);

}
