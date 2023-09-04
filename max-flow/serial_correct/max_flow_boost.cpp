#include <fstream>
#include "max_flow.hpp"
#include <boost/graph/adjacency_list.hpp>

using namespace std;


int main (int argc, char ** argv) {


  char* file_name = argv[1];
  network_flow g (file_name);
  g.parseGraph () ;
  g.initialize();
  return 0;
}
