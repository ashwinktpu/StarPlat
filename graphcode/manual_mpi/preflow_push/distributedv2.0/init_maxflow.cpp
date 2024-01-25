/*
 * This file is for initializing preflow 
 * Source will be assigned a height of \V\ (long long)
 * It will also be assigned excess of the summation of outgoing capacities
 *
 * v 1.0 Repartitioning (dynamic load balancing) and usage of augmented parts to be considered.*/


#include "partitioned_file_reader.hpp"
#include <mpi.h>
#include <assert.h>
#include "Distributed_Network.hpp"
#include "debug.hpp"

int source_process, sink_process ;
bool ACTIVE_VERTEX_EXISTS_GLOBAL ;

int main (int argc, char** argv) {


    log_message ("program started") ;
    double start, end ;
    int status = MPI_Init (&argc, &argv) ;
    assert (status == MPI_SUCCESS) ;

    int source, sink, source_process, sink_process ;

    source = 0, sink = 1 ;

    int p_no, size ;
    MPI_Comm_size (MPI_COMM_WORLD, &size) ;
    MPI_Comm_rank (MPI_COMM_WORLD, &p_no) ;

    log_message ("Process id = " + to_string (p_no) + "\n world size = " + to_string (size))  ;

    char* local_graph_path = construct_file_name (p_no) ;
    
    Distributed_Network max_flow_network (local_graph_path, source, sink, p_no, size) ;
    MPI_Barrier (MPI_COMM_WORLD) ;
    MPI_Barrier (MPI_COMM_WORLD) ;
    free (local_graph_path) ;
    local_graph_path = NULL ;

    MPI_Barrier (MPI_COMM_WORLD) ;
    start = MPI_Wtime () ;
    while (max_flow_network.local_max_flow ()) ;
//    max_flow_network.local_max_flow () ;
    log_message ("Got here") ;
    MPI_Barrier (MPI_COMM_WORLD) ;
    end = MPI_Wtime () ;
    max_flow_network.print_ans () ;
    // delete (&max_flow_network) ;
    MPI_Finalize () ;
    cout << "Execution time = " << end-start << endl ;
    return 0 ;
}
