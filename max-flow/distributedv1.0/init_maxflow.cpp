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

int source_process, sink_process ;


int main (int argc, char** argv) {

  int status = MPI_Init (&argc, &argv) ;

  assert (status == MPI_SUCCESS) ;
  int source = 0, sink = 50 ;
  Network_flow curr_network = read_current_file (MPI_COMM_WORLD, source, sink) ;
  tie (source_process, sink_process) = find_source_sink () ;
  initialize_heights (MPI_COMM_WORLD, source_process) ;
  initialize_excesses (MPI_COMM_WORLD) ;



  MPI_Finalize () ;
}
