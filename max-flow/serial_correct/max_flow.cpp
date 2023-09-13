#include "max_flow_csr.hpp"
#include <fstream>

int main (int argc, char ** argv) {


  char* file_name = argv[1];
  network_flow g (file_name, 0, 5);


  // start the actual max_flow.
  cout << "updated version\n" ;


  while (ACTIVE_VERTEX_EXISTS) {

    // we need to push source vertex into the queue.
    g.push_and_relabel () ;
  }
  return 0;

}
