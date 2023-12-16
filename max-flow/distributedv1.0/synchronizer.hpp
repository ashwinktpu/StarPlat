#ifndef __SYNCER__

#define __SYNCER__ 

#include "debug.hpp"
#include <vector>
#include <assert.h>
#include "serial_methods.hpp"



void synchronize (vector<vector<int> > &syncer, int* arr, int n) {

    int rank ; MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    std::vector<MPI_Request> requests ;
    // log_message (to_string (rank)  + " :  has syncer of size " + to_string (syncer.size ())) ;

    for (auto &sync_this:syncer) {

        int from_process = sync_this[0] ;
        int to_process = sync_this[1] ;
        int global_offset = sync_this[2] ;
        int value = sync_this[3] ;
        log_message ("to_process = " + to_string (to_process)) ;
        log_message ("from_process = " + to_string (from_process)) ;
        
        if (rank == from_process) {
            MPI_Request request ;
            int send_packet[2] = {value, global_offset} ;
            log_message ("ready to send") ;
            // MPI_Isend (&send_packet, 2, MPI_INT, to_process, 0, MPI_COMM_WORLD, &request) ;
            MPI_Send (&send_packet, 2, MPI_INT, to_process, 0, MPI_COMM_WORLD) ;
            log_message ("packet details =======================================\n" + to_string (send_packet[0]) + "\n" + to_string (send_packet[1])) ;
            requests.push_back (request) ;
            log_message ("SEND OK") ;
        }

        if (rank == to_process) {
            MPI_Request request ;
            int received_value[2] ;
            // MPI_Irecv (&received_value, 2, MPI_INT, from_process, 0, MPI_COMM_WORLD, &request) ;
            MPI_Recv (&received_value, 2, MPI_INT, from_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
            assert (native_vertex.find (received_value[1]) != native_vertex.end ()) ;
            int local_offset = global_to_local[received_value[1]] ;
            log_message ("pushing inside " + to_string (local_offset) + " when total size of the arr = " + to_string (n) + " a value of " + to_string (received_value[0])) ;
            arr [local_offset] += received_value[1] ;
            requests.push_back (request) ;
            log_message ("Recv OK") ;
        }
    }
    // MPI_Waitall (requests.size (), requests.data (), MPI_STATUSES_IGNORE) ;
    MPI_Barrier (MPI_COMM_WORLD) ;
}


void synchronize_queue (vector<vector<int> > &syncer, queue<int> active_vertices) {

    int rank ; MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    std::vector<MPI_Request> requests ;
    log_message (to_string (rank)  + " :  has syncer of size " + to_string (syncer.size ())) ;

    for (auto &sync_this:syncer) {

        int from_process = sync_this[0] ;
        int to_process = sync_this[1] ;
        int global_offset = sync_this[2] ;
        // int value = sync_this[3] ;
        log_message ("to_process = " + to_string (to_process)) ;
        log_message ("from_process = " + to_string (from_process)) ;
        
        if (rank == from_process) {
            MPI_Request request ;
            int send_packet[1] = {global_offset} ;
            log_message ("ready to send") ;
            // MPI_Isend (&send_packet, 1, MPI_INT, to_process, 0, MPI_COMM_WORLD, &request) ;
            MPI_Send (&send_packet, 1, MPI_INT, to_process, 0, MPI_COMM_WORLD) ;
            // log_message ("packet details =======================================\n" + to_string (send_packet[0]) + "\n" + to_string (send_packet[1])) ;
            requests.push_back (request) ;
            log_message ("SEND OK") ;
        }

        if (rank == to_process) {
            MPI_Request request ;
            int received_value[2] ;
            // MPI_Irecv (&received_value, 1, MPI_INT, from_process, 0, MPI_COMM_WORLD, &request) ;
            MPI_Recv (&received_value, 1, MPI_INT, from_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
            assert (native_vertex.find (received_value[0]) != native_vertex.end ()) ;
            // int local_offset = global_to_local[received_value[1]] ;
            // log_message ("pushing inside " + to_string (local_offset) + " when total size of the arr = " + to_string (n)) ;
            active_vertices.push (global_to_local[received_value[0]]) ;
            requests.push_back (request) ;
            log_message ("Recv OK") ;
        }
    }
    // MPI_Waitall (requests.size (), requests.data (), MPI_STATUSES_IGNORE) ;
    MPI_Barrier (MPI_COMM_WORLD) ;
}


#endif
