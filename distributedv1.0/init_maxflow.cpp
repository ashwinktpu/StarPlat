/*
 * This file is for initializing preflow 
 * Source will be assigned a height of \V\ (long long)
 * It will also be assigned excess of the summation of outgoing capacities
 *
 * v 1.0 Repartitioning (dynamic load balancing) and usage of augmented parts to be considered.*/


#include "partitioned_file_reader.hpp"
//#include "macro_defs.hpp"
#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "max_flow_csr.hpp"
#include "pushers.hpp"
//#include "relabelers.hpp" 

using namespace std ;



int source_process, sink_process, global_max_flow_val ;
bool ACTIVE_VERTEX_EXISTS_GLOBAL ;

int main (int argc, char** argv) {

  int status = MPI_Init (&argc, &argv) ; // initialize mpi.
  assert (status == MPI_SUCCESS) ; // Exit with failure if initialization ends.

  // some local variables which will be needed further
  int source, sink, source_process, sink_process ; 
  int rank, size ;
  global_max_flow_val=0 ;

  // attain rank.
  MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;

  if (rank == 0) {

    printf ("Enter source and sink nodes \n") ;
    cin >> source >> sink ;
  }


  MPI_Barrier(MPI_COMM_WORLD) ; // SO that other processes wait for the input to be taken, and do not have grabage as source.

  status = MPI_Bcast (&source, 1, MPI_INT, 0, MPI_COMM_WORLD) ; // Broadcast Source
  assert (status == MPI_SUCCESS) ; // want to halt of broadcast fails.
  status = MPI_Bcast (&sink, 1, MPI_INT, 0, MPI_COMM_WORLD) ; // Broadcast Sink.


  // Create Network_flow object for every process.
  Network_flow curr_network = read_current_file (MPI_COMM_WORLD, source, sink, ACTIVE_VERTEX_EXISTS_GLOBAL) ; // create Network flow class
  source_process = find_source (curr_network) ; // find the source.
  
  cerr << "found source process to be " << source_process << endl ;

  // To get the active vertices in the local partition.
  int local_active = curr_network.get_active_vertices () ;
  MPI_Barrier (MPI_COMM_WORLD) ;
  MPI_Reduce (&rank, &local_active, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) ;
  cerr << "number of active vertices : " << local_active << endl ;
  
  while (local_active) {
    
    start_pulse (MPI_COMM_WORLD, rank, curr_network) ;
    MPI_Barrier (MPI_COMM_WORLD) ; // Make sure correct labels are available to all processes.

    local_active = curr_network.get_active_vertices () ;
    MPI_Barrier (MPI_COMM_WORLD) ;
    MPI_Reduce (&rank, &local_active, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) ;

    cerr << "active vertices : " << local_active << endl ;
  }

  cerr << "all good" << endl ;

  // The following is to retreive the max flow from the processes.
  int total_source_excess, total_sink_excess ;
  total_source_excess = curr_network.get_residue_on_source () ;
  cerr << rank << " " << total_source_excess << endl ;
  MPI_Barrier (MPI_COMM_WORLD) ;
  MPI_Reduce (&rank, &total_source_excess, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) ;
  cerr << "treduction successful\n" ;
  MPI_Bcast (&total_source_excess, 1, MPI_INT, 0, MPI_COMM_WORLD) ;
  total_sink_excess = curr_network.get_residue_on_sink () ;
  MPI_Barrier (MPI_COMM_WORLD) ;
  MPI_Reduce (&rank, &total_sink_excess, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) ;
  MPI_Bcast (&total_sink_excess, 1, MPI_INT, 0, MPI_COMM_WORLD) ;
  cerr << total_sink_excess << endl ;
  cout << "maxflow value : " << total_source_excess-total_sink_excess << endl ;
  // This should print the correct value for flow in some file.
  
  MPI_Finalize () ;
  return 0 ;
}
