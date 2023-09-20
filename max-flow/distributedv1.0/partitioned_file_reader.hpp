#ifndef _READER_ 

#define _READER_ 
#include <fstream>
#include <vector>
#include <mpi.h>
#include "graph_mpi_weight.hpp"
#include "max_flow_csr.hpp"



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

Network_flow read_current_file (MPI_Comm communicator, int source, int sink) {

    int rank, size ;

    MPI_Comm_rank (communicator, &rank) ;
    MPI_Comm_size (communicator, &size) ;

    char* file_name = construct_file_name (rank) ;
    
    cerr << "opening up file " << file_name << endl ;
    Network_flow partial_network(file_name, source, sink) ;
}

void initialize_heights (MPI_Comm communicator, int source) {

    int rank, size ;
    rank = MPI_Comm_rank (communicator, &rank) ;
    size = MPI_Comm_size (communicator, &size) ;

}

void initialize_excesses (MPI_Comm communicator) {

    // fill this up.
}

pair<int,int> find_source_sink ()  {

    // fill this up.
}
#endif