#ifndef _READER_ 

#define _READER_ 
#include <fstream>
#include <vector>
#include <mpi.h>
#include "Distributed_Network.hpp"



char* construct_file_name (int rank) {

    char* name = (char*)malloc (sizeof(char)*8) ;
    name[7] = '\0' ;
    name[0] = 'p' ;
    name[1] = 'a' ;
    name[2] = 'r' ;
    name[3] = 't' ;
    name[4] = '_' ;

    if (rank<10) {
        name[5] = '0' ;
        name[6] = ((char)rank+48) ;
    } else {
        name[5] = (((char)rank/10)+48) ;
        name[6] = (((char)rank%10)+48) ;
    }

    return name ;
}

#endif
