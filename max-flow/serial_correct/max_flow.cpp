#include "max_flow_csr.hpp"
#include <fstream>



int32_t  main (int argc, char ** argv) {


  std::ofstream cerrToNull("/dev/null");
  std::streambuf* cerrBuffer = std::cerr.rdbuf(cerrToNull.rdbuf());

  char* file_name = argv[1];
  network_flow g (file_name, 0, 5);


  // start the actual max_flow.
  while (ACTIVE_VERTEX_EXISTS) {

    g.push_and_relabel () ;
  }
    std::cerr.rdbuf(cerrBuffer);

  g.print_result () ;
  return 0;

}
