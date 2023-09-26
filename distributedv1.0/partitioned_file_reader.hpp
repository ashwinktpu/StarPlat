#ifndef _READER_ 

#define _READER_ 
#include <fstream>
#include <vector>
#include <mpi.h>
#include "graph_mpi_weight.hpp"
#include "max_flow_csr.hpp"
#include "rma_datatype/rma_datatype.h"



char* construct_file_name (int rank) {

    char* name = new char[11] ;
    name[0] = '.' ;
    name[1] = '.' ;
    name[2] = '/' ;
    name[10] = '\0' ;
    name[3] = 'p' ;
    name[4] = 'a' ;
    name[5] = 'r' ;
    name[6] = 't' ;
    name[7] = '_' ;

    if (rank<10) {
        name[9] = '\0' ;
        name[8] = ((char)rank+48) ;
    } else {
        name[8] = (((char)rank/10)+48) ;
        name[9] = (((char)rank%10)+48) ;
    }
    string temp = name ;
    return name;
}

Network_flow read_current_file (MPI_Comm communicator, int source, int sink, bool &ACTIVE_VERTEX_EXISTS_GLOBAL) {

    int rank, size ;

    MPI_Comm_rank (communicator, &rank) ;
    MPI_Comm_size (communicator, &size) ;

    cout << "for opening file by 2 " << rank << endl ;
    
    char* file_name = construct_file_name (rank) ;
    cout << "opening up file " << file_name << endl ;
    Network_flow partial_network(file_name, source, sink) ;

    int total_nodes ;
    int total_edges ;
    int local_nodes = partial_network.getDiffTotal () ;
    int local_edges = partial_network.num_edges () ;
    
    MPI_Reduce (&local_nodes, &total_nodes, 1, MPI_INT, MPI_SUM, 0, communicator) ;
    MPI_Reduce (&local_edges, &total_edges, 1, MPI_INT, MPI_SUM, 0, communicator) ;
    MPI_Bcast (&total_nodes, 1, MPI_INT, 0, communicator) ;
    MPI_Bcast (&total_edges, 1, MPI_INT, 0, communicator) ;

    partial_network.set_global_nodes (total_nodes) ;
    partial_network.set_global_edges (total_edges) ;
    cerr << "total number of vertices " << total_nodes << endl ;
    cerr << "total number of edges " << total_edges << endl ;
}


void initialize_excesses (MPI_Comm communicator) {

    // fill this up.
}

pair<int,int> find_source_sink ()  {


    // How to search for this ?? Use Atharva's code for reference.
}
#endif
