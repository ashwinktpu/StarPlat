#ifndef __DISTRIBUTED_METHODS__ 

#define __DISTRIBUTED_METHODS__

#include <mpi.h> 
#include "serial_methods.hpp"

void synchronize_excesses (const vvi &syncer, MPI_Win excess) {

    for (auto &curr_excess:syncer) {

        int to_process = curr_excess[0] ;
        int idx = curr_excess[1]; 
        int val = curr_excess[2] ;
        int curr_val ;
        MPI_Get (&curr_val, 1, MPI_INT, to_process, idx, 1, MPI_INT, excess) ;
        curr_val += val ;
        MPI_Put (&curr_val, 1, MPI_INT, to_process, idx, 1, MPI_INT, excess) ;
    }
}

void set_source_sink_distributed (const int &p_no, const int &source, const int &sink, MPI_Win excess) {

    int rank ; MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    vvi syncer ;

    if (global_to_local.find (source) != global_to_local.end ()) {

        for (int v_idx = 0 ; v_idx < residual_graph[source].size (); v_idx++ ){

            int v = residual_graph[src][v_idx][0] ;
            residual_graph[v][residual_graph[src][v_idx][2]][1] += residual_graph[src][v_idx][1] ;

            int this_capacity ; 
            MPI_Get (&this_capacity, 1, MPI_INT, rank, v, 1, MPI_INT, excess) ;
            this_capacity += residual_graph[src][v_idx][1] ;
            MPI_Put (&this_capacity, 1, MPI_INT, rank, v, 1, MPI_INT, excess) ;

            if (residual_graph[src][v_idx][3] != p_no) {
                syncer.push_back ({residual_graph[src][v_idx][3], v, this_capacity}) ;
            }

            residual_graph[src][v_idx][1] = 0 ;
            if (this_capacity > 0 && v!= src && v!=snk) {
                active.push (v) ;
            }
        }
    }
    synchronize_excesses (syncer, excess) ;
}

void distributed_bfs (MPI_Win heights) {
    // Implement this later.

    return ;
}

bool lowner (const int &x) {

    return global_to_local.find (x) != global_to_local.end () ;
}

void max_distributed_flow_init (const int &my_rank, const int &source, const int &sink, const int &local_vertices, const int &global_vertices, MPI_Win heights, MPI_Win excess, MPI_Win CAND, MPI_Win work_list, int *heights_buffer, int *excess_buffer, int *cand_buffer, int *work_list_buffer) {

    MPI_Win_allocate (local_vertices * sizeof (int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, heights_buffer, &heights) ;
    MPI_Win_allocate (local_vertices * sizeof (int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, excess_buffer, &excess) ;
    MPI_Win_allocate (local_vertices * sizeof (int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, cand_buffer, &CAND) ;
    MPI_Win_allocate (local_vertices * sizeof (int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, work_list_buffer, &work_list) ;

    for (int i=0; i<local_vertices; i++) {
        heights_buffer[i]=0 ;
        excess_buffer[i]=0 ;
        cand_buffer[i]=0 ;
        work_list_buffer[i]=0 ;
    }

    set_source_sink_distributed (my_rank, source, sink, excess) ;

    int send_buffer = global_vertices ;
    if (lowner (source)) {
        MPI_Put (&send_buffer,  1, MPI_INT, my_rank, global_to_local[source], 1, MPI_INT, heights) ;
    }

    distributed_bfs (heights) ;
}

#endif
