#include "graph_mpi_weight.hpp"

class network_flow : public graph {

public:
  network_flow (char* text_path) : graph (text_path) {}
} ;

int main (int argc, char** argv) {
 
  char* file_name = argv[1] ;
  network_flow n_g (file_name) ;
  cout << "testing \n" ;
}
