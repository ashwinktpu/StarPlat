#include "readFile.h"

void readFile (char * fileName, int &size, std::vector<std::vector<int> > &transactions) {
    FILE * testCase = fopen (fileName, "r") ;
    fscanf (testCase, "%d", &size) ;
    int idx, val ;
    for (int transactionId = 0 ; transactionId < size ; transactionId++) {
        fscanf (testCase, "%d %d", &idx, &val) ;
        transactions.push_back ({idx, val}) ;
    }
}

void writeFile (char * fileName, std::vector<int> result ) {
    FILE * output = fopen (fileName, "r") ;
    for (auto &res:result) {

    }
}
