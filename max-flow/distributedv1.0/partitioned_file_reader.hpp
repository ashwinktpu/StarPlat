#ifndef _READER_ 

#define _READER_ 
#include <fstream>
#include <vector>
#include <mpi.h>
#include "graph_mpi_weight.hpp"
#include "max_flow_csr.hpp"
#include "rma_datatype/rma_datatype.h"



char* construct_file_name (int rank) {

    char name[8] ;
    name[7] = '\0' ;
    name[0] = 'p' ;
    name[1] = 'a' ;
    name[2] = 'r' ;
    name[3] = 't' ;
    name[4] = '_' ;

    if (rank<10) {
        name[5] = '0' ;
        name[6] = ((char)rank+48) ;
    } else {
        name[5] = (((char)rank/10)+48) ;
        name[6] = (((char)rank%10)+48) ;
    }
    return name ;
}

Network_flow read_current_file (MPI_Comm communicator, int source, int sink, bool &ACTIVE_VERTEX_EXISTS_GLOBAL) {

    int rank, size ;

    MPI_Comm_rank (communicator, &rank) ;
    MPI_Comm_size (communicator, &size) ;

    char* file_name = construct_file_name (rank) ;
    
    cerr << "opening up file " << file_name << endl ;
    Network_flow partial_network(file_name, source, sink) ;

    int total_nodes ;
    int total_edges ;
    int local_nodes = partial_network.num_nodes () ;
    int local_edges = partial_network.num_edges () ;
    
    MPI_Reduce (&local_nodes, &total_nodes, 1, MPI_INT, MPI_SUM, 0, communicator) ;
    MPI_Reduce (&local_edges, &total_edges, 1, MPI_INT, MPI_SUM, 0, communicator) ;
    MPI_Bcast (&total_nodes, 1, MPI_INT, 0, communicator) ;
    MPI_Bcast (&total_edges, 1, MPI_INT, 0, communicator) ;

    partial_network.set_global_nodes (total_nodes) ;
    partial_network.set_global_edges (total_edges) ;

    Rma_Datatype global_heights ;
    Rma_Datatype global_excess ;

}

void initialize_heights (MPI_Comm communicator, int source, Networkflow curr_network) {

    int rank, size ;
    rank = MPI_Comm_rank (communicator, &rank) ;
    size = MPI_Comm_size (communicator, &size) ;

    int * local_heights = (int*)malloc (curr_network.local_num_nodes()*sizeof(int));
    Rma_Datatype heights ;

    heights.create_window (local_heights, num_nodes, communicator, MPI_INFO_NULL) ;
    heights.get_lock (source_process, lock, false) ;
    heights.put_data (source_process, source, 1, lock) ;

    // lock obtained and initialized to 0.
}

void initialize_excesses (MPI_Comm communicator) {

    // fill this up.
}

pair<int,int> find_source_sink ()  {


    // How to search for this ?? Use Atharva's code for reference.
}
#endif