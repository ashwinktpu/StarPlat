#ifndef _PUSHER_ 

#define _PUSHER_ 
#include "rma_datatype/rma_datatype.h"
#include <vector> 
#include <mpi.h>
#include "max_flow_csr.hpp"

bool START_PUSH ;


void push_phase (MPI_Comm communicator, int rank, Network_flow curr_network) {

    START_PUSH = true ;
    while (curr_network.push_is_possible)
        curr_network.push () ;
    START_PUSH = false ;

}


void enter_relabel_phase (MPI_Comm communicator) {


    int rank ;
    MPI_Comm_rank (communicator, &rank) ;

    START_PUSH = false ;

    int status = MPI_Bcast (&START_PUSH, 1, MPI_C_BOOL, 0, communicator) ;

    assert (status == MPI_SUCCESS) ;

} 

void relabel (MPI_Comm communicator) {

    // Create RMA window .
}

#endif