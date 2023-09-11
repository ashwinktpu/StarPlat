#include <fstream>
#include "max_flow.hpp"
#include <boost/graph/adjacency_list.hpp>


using namespace std;


int main (int argc, char ** argv) {


  char* file_name = argv[1];
  vertex_properties source ;
  source.vertex_id=1 ;
  vertex_properties sink ;
  sink.vertex_id = 2 ;
  network_flow g (file_name, source, sink);
  g.print_heights() ;


  // start the actual max_flow.
  cout << "updated version\n" ;

  g.print_active_vertices() ;
  while (ACTIVE_VERTEX_EXISTS) {

    cout << "doing push_and_relabel\n" ;
    g.push_and_relabel () ;
  }
  return 0;

}
