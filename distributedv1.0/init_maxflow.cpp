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
//#include "pushers.hpp"
//#include "relabelers.hpp" 

using namespace std ;



int source_process, sink_process ;
bool ACTIVE_VERTEX_EXISTS_GLOBAL ;

int main (int argc, char** argv) {

  int status = MPI_Init (&argc, &argv) ;

  assert (status == MPI_SUCCESS) ;

  int source, sink, source_process, sink_process ;

  int rank, size ;

  MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;

  if (rank == 0) {

    printf ("Enter source and sink nodes \n") ;
    cin >> source >> sink ;
  }
  MPI_Barrier(MPI_COMM_WORLD) ;
  MPI_Barrier (MPI_COMM_WORLD) ;
  status = MPI_Bcast (&source, 1, MPI_INT, 0, MPI_COMM_WORLD) ;
  assert (status == MPI_SUCCESS) ;
  status = MPI_Bcast (&sink, 1, MPI_INT, 0, MPI_COMM_WORLD) ;
  cout << rank << " " << source << " ---- > " << sink << endl ;
  MPI_Barrier(MPI_COMM_WORLD) ;
  Network_flow curr_network = read_current_file (MPI_COMM_WORLD, source, sink, ACTIVE_VERTEX_EXISTS_GLOBAL) ;
 

  //tie (source_process, sink_process) = find_source_sink (curr_network) ;

  //while (ACTIVE_VERTEX_EXISTS_GLOBAL) {
    
  //}
  MPI_Finalize () ;
  return 0 ;
}
