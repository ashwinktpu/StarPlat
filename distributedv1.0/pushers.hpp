#ifndef _PUSHER_ 

#define _PUSHER_ 
#include "rma_datatype/rma_datatype.h"
#include <vector> 
#include <mpi.h>
#include "max_flow_csr.hpp"

bool PULSE_ONGOING ;


void start_pulse (MPI_Comm communicator, int &rank, Network_flow &curr_network) {

    PULSE_ONGOING = true ;
        curr_network.push_and_relabel () ;
        curr_network.synchronize_excess () ;
    PULSE_ONGOING = false ;
}

#endif