#ifndef __SYNCER__

#define __SYNCER__ 

#include "debug.hpp"
#include <vector>
#include <assert.h>
#include "serial_methods.hpp"



void synchronize (vector<vector<int> > &syncer, MPI_Win &arr) {

    assert (arr != MPI_WIN_NULL) ;
    int rank ; MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    int n ;
    int local_n = syncer.size () ;
    MPI_Allreduce (&local_n, &n, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
    log_message ("all processes will go through the following " + to_string (n)) ;
    // convert to local offset
    for (int i=0; i<n; i++) {

        int to_process=-1, global_offset=-1 ;
        if (i<syncer.size ()) {
            auto sync_this = syncer[i] ;
            int global_offset = sync_this[2] ;
            int to_process = sync_this[1] ;
        }

        if (rank == to_process) {
            assert (native_vertex.find (global_offset) != native_vertex.end ()) ;
            if (i<syncer.size ()) {
                auto sync_this = syncer[i] ;
                sync_this[2] = global_to_local[global_offset] ;
            }
        }
    }
    log_message ("all conversion OK") ;

    MPI_Barrier (MPI_COMM_WORLD) ;

    MPI_Win_fence (0, arr) ;
    log_message ("rma cycle has started ") ;

    // syncer has from->to and will only be there for to. 
    for (auto &sync_this:syncer) {

        int from_process = sync_this[0] ;
        int to_process = sync_this[1] ;
        int local_offset = sync_this[2] ;
        int value = sync_this[3] ;
        log_message ("to_process = " + to_string (to_process)) ;
        log_message ("from_process = " + to_string (from_process)) ;
        // call accumulate from 
        MPI_Accumulate (&local_offset, 1, MPI_INT, from_process, 1, 1, MPI_INT, MPI_SUM, arr) ;
    }
    // MPI_Waitall (requests.size (), requests.data (), MPI_STATUSES_IGNORE) ;
    MPI_Win_fence (0, arr) ;
    MPI_Barrier (MPI_COMM_WORLD) ;
}

void synchronize_queue (vector<vector<int> > &syncer, queue<int> &active_vertices) {

    int rank ; MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    std::vector<MPI_Request> requests ;
    log_message (to_string (rank)  + " :  has syncer of size " + to_string (syncer.size ())) ;

    int upper_bound ;
    int x = syncer.size () ;
    MPI_Allreduce (&x, &upper_bound, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
    log_message ("upper bound = " +to_string (upper_bound)) ;

    for (int i = 0; i <= upper_bound; i++) {
        int from_process = -1, to_process = -1, global_offset = -1;
        int sync_this[3];

        if (i < syncer.size()) {
            std::copy(syncer[i].begin(), syncer[i].end(), sync_this);
        } else {
            std::fill(sync_this, sync_this + 3, -1);
        }

        log_message("pre update  " + to_string(sync_this[1]) + " from " + to_string(sync_this[0]) +  " call from process " + to_string (rank));
        MPI_Bcast(sync_this, 3, MPI_INT, rank, MPI_COMM_WORLD);
        log_message("post update  " + to_string(sync_this[1]) + " from " + to_string(sync_this[0])+ " " + to_string (rank));

        if (rank == sync_this[1]) {
            log_message("adding active vertex " + to_string(sync_this[2]));
            active_vertices.push(sync_this[2]);
        }
    }

MPI_Barrier(MPI_COMM_WORLD);

}


#endif
