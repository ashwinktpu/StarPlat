#include "graph_mpi_weight.hpp"
#include <assert.h>

struct vertex_properties ;
struct edge_elements ;


struct edge_elements {
  int flow ;
  int capacity ;
  vertex_properties *destination ;
};

struct vertex_properties {


  bool active ;
  int excess ;
  int vertex_id ;
  int height ;
  edge_elements *current_edge ;

  bool operator< (vertex_properties a, vertex_properties b) {

    return a.vertex_id < b.vertex_id ;  
  }
} ;


class network_flow : public graph {

private:
  int source ;
  int sink ;

public:


  void select_source () {}


  void select_sink () {}


  void set_up_excess () {

    assert (source != -1) ;
    assert (sink != -1) ;

  }

  void initialize () {

    
    map <int, vector<edge>> adj_list = getEdges();   

    map <vertex_properties, vector<edge_elements> > adj;
  
    for (auto edge:adj_list) {
    
      edge_elements edge_elements_new;
  
      vertex_properties this_vertex;
      this_vertex.vertex_id = edge.first;
      this_vertex.active = true;
      this_vertex.excess=0;

      if (adj.find (this_vertex) == adj.end ()) {
        adj[this_vertex] = {};
      }

     for (auto edge_props:edge.second) {


        vertex_properties destination ;
        destination.excess = 0 ;
        destination.active = true ;
        destination.vertex_id = edge_props.destination ;

        edge_elements_new.flow = 0 ;
        edge_elements_new.capacity = edge_props.weight ;
        edge_elements_new.destination = &destination ;

        adj[this_vertex].push_back (edge_elements_new) ;
        
      }
    }

    set_up_heights () ;
    set_up_excess () ;
  }

  void set_up_heights () {

    queue <vertex_properties> q;
    q.push_back (source) ;

    while (!q.empty ()) {
      
      vertex_properties u = q.front () ;
      
      for (auto it : adj[u]) {

      }
    }
  }
};
  
  
  
  
