#include <vector>
#include "readFile.h"

int main (int argc, char ** argv) {
    
    int size ;
    std::vector<std::vector<int> > transactions ;
    readFile (argv[1], size, transactions) ;
        
    std::vector<int> serialArr (size, 0) ;

    for (auto &transaction:transactions) {
        int idx = transaction[0] ;
        int val = transaction[1] ;
        if (val >= 0 ) {
            serialArr[idx] = val ;
        } else {
            printf ("%d\n", serialArr[idx]) ;
        }
    }
}
