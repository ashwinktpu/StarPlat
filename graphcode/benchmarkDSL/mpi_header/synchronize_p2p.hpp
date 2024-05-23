#ifndef __SYNCER__
#define __SYNCER__ 

#include <vector>
#include <mpi.h>
//#include "debug.hpp"


std::vector<int*> synchronize (std::vector<std::vector<int> > &syncer, int packet_size) {
    /* Always pass global ids
     * This function only synchronizes and returns the collective buffer in a vector of vectors. 
     * Further methods are needed to update the appropriate arrays. */

    int p_no ; int size ;
    MPI_Comm_rank (MPI_COMM_WORLD, &p_no) ;
    MPI_Comm_size (MPI_COMM_WORLD, &size) ;
    
    std::vector<int*> update_these ;

    for (int rank = 0; rank < size ; rank++) {

        int num_iter = 0 ;
        int local_n = 0 ;

        if (rank == p_no) {

            local_n = syncer.size () ;
        }

        MPI_Allreduce (&local_n, &num_iter, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
        MPI_Barrier (MPI_COMM_WORLD) ;


        for (int i=0; i<num_iter; i++) {

            if (rank == p_no) {

                auto sync_this = syncer[i] ;
        		if (sync_this.size () == 0) continue ;
                int from_process = sync_this[0] ;
                int to_process = sync_this[1] ;
                int* packet = sync_this.data ()+1 ;
	          	for (int destination = 0 ; destination < size ; destination++) {
	        	    if (destination != p_no) 
                         MPI_Send (packet, packet_size+1, MPI_INT, destination, 0, MPI_COMM_WORLD) ;
	    	    }
            } else {
                int* receipt = (int*)malloc (sizeof (int) * (packet_size+1));
                MPI_Recv (receipt, packet_size+1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
		            if (p_no == receipt[0])
                    update_these.push_back (receipt+1) ;
                else if (receipt[0] == -1) 
                    update_these.push_back (receipt+1) ;
				        else 
					          free (receipt) ;
            }
        }
    }
    return update_these ;
} 

#endif
