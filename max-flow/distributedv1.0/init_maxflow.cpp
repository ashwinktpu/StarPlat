/*
 * This file is for initializing preflow 
 * Source will be assigned a height of \V\ (long long)
 * It will also be assigned excess of the summation of outgoing capacities
 *
 * v 1.0 Repartitioning (dynamic load balancing) and usage of augmented parts to be considered.*/

#include "partitioned_file_reader.hpp"
#include "macro_defs.hpp"


int main (int argc, char** argv) {


  initiate_conversation () ;
  prepare_graph (argv[1]) ;
  long long int* heights = initiate_distributed_array (vertex_number) ;
  long long int* excesses = initiate_distributed_array (vertex_number) ;
  
}
