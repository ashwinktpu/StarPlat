#include "data_constructs/data_constructs.h"
#include "readFile.h"

int main (int argc, char ** argv) {
    MPI_Init (argc, argv) ; 
    assert (argc == 2) ;
    int size ;
    if (rank == 0) readFile (argv[1], size, transactions) ;
    Container<int> parallelArray ;
    parallelArray.assign (size) ; // Expecting the constructor to take care of the size.
    for (auto &transaction:transactions) {
        int idx = transaction[0] ;
        int val = transaction[1] ;
        if (val>=0)
            parallelArray.setValue (idx, val) ;
        else 
            printf ("%d\n", parallelArray.getValue (idx)) ;
    } 
    parallelArray.printArray () ;
    MPI_Finalize () ;
}
