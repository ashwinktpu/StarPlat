/*
 * This file is for initializing preflow 
 * Source will be assigned a height of \V\ (long long)
 * It will also be assigned excess of the summation of outgoing capacities
 *
 * v 1.0 Repartitioning (dynamic load balancing) and usage of augmented parts to be considered.*/


#include "partitioned_file_reader.hpp"
#include "macro_defs.hpp"
#include <mpi.h>
#include <assert.h>
#include "max_flow_csr.hpp"
#include "pushers.hpp"
#include "relabelers.hpp" 

int source_process, sink_process ;
bool ACTIVE_VERTEX_EXISTS_GLOBAL ;

int main (int argc, char** argv) {

  int status = MPI_Init (&argc, &argv) ;

  assert (status == MPI_SUCCESS) ;

  int source, sink, source_process, sink_process ;

  printf ("Enter source and sink nodes \n") ;
  scanf ("%d %d ", &source, &sink) ;

  Network_flow curr_network = read_current_file (MPI_COMM_WORLD, source, sink, ACTIVE_VERTEX_EXISTS_GLOBAL) ;

  tie (source_process, sink_process) = find_source_sink (curr_network) ;

  while (ACTIVE_VERTEX_EXISTS_GLOBAL) {
    
  }
  MPI_Finalize () ;
}
