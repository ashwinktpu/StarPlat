#include "max_flow_csr.hpp"
#include <fstream>

int32_t  main (long long  argc, char ** argv) {


  char* file_name = argv[1];
  network_flow g (file_name, 0, 1);


  // start the actual max_flow.
  cout << "updated version\n" ;


  while (ACTIVE_VERTEX_EXISTS) {

    // we need to push source vertex long long o the queue.
    g.push_and_relabel () ;
  }

  g.print_result () ;
  return 0;

}
