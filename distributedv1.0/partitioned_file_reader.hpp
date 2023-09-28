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
    int total_internal ;

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

    return partial_network ;
}


Rma_Datatype <int> initialize_excesses (MPI_Comm communicator, Network_flow &total_network, int source_process) {
    
    Rma_Datatype<int> excess ;
    int* excess_arr = (int*) malloc (sizeof (int) * total_network.get_global_vertices ()) ;
    excess.create_window (excess_arr, total_network.get_global_vertices (), sizeof(int), MPI_COMM_WORLD) ;
    int outgoing_caps = 0 ;
    for (auto &v:total_network.getNeighbours (total_network.get_source ())) {
        outgoing_caps += v.weight ;
    }
    locktype lock=EXCLUSIVE_LOCK ;
    excess.get_lock (source_process, lock) ;
    excess.put_data (source_process, excess_arr, outgoing_caps, 1, lock) ;
    excess.unlock (source_process, lock) ;

    return excess ;
}

Rma_Datatype<int> initialize_heights (MPI_Comm communicator, Network_flow &total_network, int source_process) {

    Rma_Datatype<int> heights ;
    int* heights_arr = (int*) malloc (sizeof (int) * total_network.get_global_vertices ()) ;
    heights.create_window (heights_arr, total_network.get_global_vertices (), sizeof(int), MPI_COMM_WORLD) ;
    locktype lock = EXCLUSIVE_LOCK;
    heights.get_lock (source_process, lock) ;
    heights.put_data (source_process, heights_arr, total_network.get_source (), 1, lock) ;
    heights.unlock (source_process, lock) ;
    
    return heights ;
}

MPI_Status remove_excess (MPI_Win &excess) {
    MPI_Win_free (&excess) ;
}

int find_source (Network_flow &total_newtork)  {

    // I am assuming user has selected source such as indegree is 0 
    // And sink such that outdegree is 0 as per properties of these nodes.
    // sink can be in multiple processes and source can be in only one process.
    int rank, size ; 
    MPI_Comm_size (MPI_COMM_WORLD, &size) ;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    int *pooler = (int*) malloc (sizeof(int) * size) ;
    memset (pooler, 0, sizeof(int)*size) ;
    if (total_newtork.exists_here (total_newtork.get_source ())) {
        pooler[rank]=1 ;
    }

    int process_num = -1 ;
    int check_sum = 0 ;
    for (int i=0; i<size; i++) {
        check_sum += pooler[i] ;
        process_num = i ;
    }
    

    for (int i=0; i<size; i++) {
        cerr << pooler[i] << " " ;
    }cerr << endl ;

    assert (check_sum == 1) ;
    assert (process_num != -1) ;

    return process_num ;
}
#endif
