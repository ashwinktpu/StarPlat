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
        log_message ("from process = " + to_string (from_process) + "to_process = " + to_string (to_process)+"offset = " + to_string(local_offset) + " " + to_string (value)) ;
        // call accumulate from 
	int result = 0;
        MPI_Get_accumulate (&value, 1, MPI_INT, &result, 1, MPI_INT, to_process, 1, 1, MPI_INT, MPI_SUM, arr) ;
	log_message ("result of accumulation " + to_string (value) + " == " +to_string (result)) ;
    }
    // MPI_Waitall (requests.size (), requests.data (), MPI_STATUSES_IGNORE) ;
    MPI_Win_fence (0, arr) ;
    MPI_Barrier (MPI_COMM_WORLD) ;
}

void synchronize_queue (vector<vector<int> > &syncer, queue<int> &active_vertices, MPI_Win &arr) {

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
    MPI_Barrier (MPI_COMM_WORLD) ;

    MPI_Win_fence (0, arr) ;
    int value = 1 ;
    int comp_val = 0 ;
    for (auto &sync_this:syncer) {

	int from_process = sync_this[0] ;
	int to_process = sync_this[1] ;
	int offset = sync_this[2] ;
	int result ;
	
	MPI_Compare_and_swap (&value, &comp_val, &result, MPI_INT, to_process, offset, arr) ;	
	value++ ;
    }
    MPI_Win_fence (0, arr) ;

    MPI_Barrier(MPI_COMM_WORLD);
}


#endif
