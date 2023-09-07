#include <fstream>
#include "max_flow.hpp"
#include <boost/graph/adjacency_list.hpp>


using namespace std;


int main (int argc, char ** argv) {


  char* file_name = argv[1];
  network_flow g (file_name);
  cout <<"starting to parse /n" ;
  g.parseGraph () ;
  cout << "came back to main succesful\n" ;
  vertex_properties source ;
  source.vertex_id=0 ;
  g.select_source(source) ;
  vertex_properties sink ;
  sink.vertex_id = 2 ;
  g.select_sink (sink) ;

  cout << "selection of source and sink succesful\n" ;

  g.initialize() ;
  g.print_heights() ;
  return 0;
}
