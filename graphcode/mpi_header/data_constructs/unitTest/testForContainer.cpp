#include "../data_constructs.h"
#include "readFile.h"
#include <assert.h>
#include <mpi.h>

int main (int argc, char ** argv) {
    MPI_Init (&argc, &argv) ; 
    int rank ;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
    assert (argc == 2) ;
    int size ;
    std::vector<std::vector<int> > transactions ;
    if (rank == 0) readFile (argv[1], size, transactions) ;
    Container<int> parallelArray ;
    parallelArray.assign (size, 0, MPI_COMM_WORLD) ; // Expecting the constructor to take care of the size.
    for (auto &transaction:transactions) {
        int idx = transaction[0] ;
        int val = transaction[1] ;
        if (val>=0)
            parallelArray.setValue (idx, val) ;
        else 
            printf ("%d\n", parallelArray.getValue (idx)) ;
    } 
    parallelArray.printArr () ;
    MPI_Finalize () ;
}
