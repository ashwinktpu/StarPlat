#include "graph_mpi_weight.hpp"
#include <assert.h>
#include <queue>


struct vertex_properties ;
struct edge_elements ;


struct edge_elements {


  // struct to store entities relevant to an edge for the max flow problem.
  int flow ; // current flow value through the edge.
  int capacity ; // capacity of the edge.
  vertex_properties *source; // vertex type struct containing the source vertex of the edge.
  vertex_properties *destination ; // vertex type struct containing the destination vertex of the edge.

};

struct vertex_properties {

  
  // struct to store all information relevant to a vertex.
  bool active ; // whether the vertex is active or not,
  int excess ; // amount of excess flow passing through the vertex.
  int vertex_id ; // vertex_id of the vertex.
  int height ; // height of the vertex.
  edge_elements *current_edge ; // current selected edge through which flow is being passed through the vertex.

  bool operator< (const vertex_properties &other)  {
    
    // for storing the vertices in an associative container. To change to Hash type container.
    return vertex_id < other.vertex_id ;  
  }
  
  // for use in visited array. 
  int operator[] (vertex_properties a) {

    return a.vertex_id ;
  }
} ;



bool operator< (const vertex_properties &a, const vertex_properties &b) {

  return a.vertex_id < b.vertex_id ;
}



class network_flow : public graph {

/*
 * network_flow type graph based on the class graph.
 * additionally stores edge capacities and current vertices.
 * Also maintains a residual height
 * Each vertex has an excess and a height associated with it.
 * */

private:

  // source and sink to be defined by user. 
  vertex_properties source ;
  vertex_properties sink ;
  map <vertex_properties, vector<edge_elements> > adj;

public:


  void select_source (vertex_properties source) {
    
    // set source of the network_flow. 
    this->source = source ;
  }


  void select_sink (vertex_properties sink) {

    // set sink of the network flow.
    this->sink = sink ;
  }


  void set_up_excess () {


    // check whether source and sink are set or not.
    assert (source.vertex_id != -1) ;
    assert (sink.vertex_id != -1) ;
    

    // set excess of source to be the sum of all capacities going out of the source.. 
    source.excess = 0;
  }

  void initialize () {
    /*
     * initialize the adjacency list of the array and create the max flow.*/
    
    map <int, vector<edge>> adj_list = getEdges();   

    // TO:DO change find a better way to represent the adjacency list. 
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
    // End To:DO

    set_up_heights () ;
    set_up_excess () ;
  }

  void set_up_heights () {

    /* 
     * Do a BFS to find the heights of all the vertices.
     * */
    queue <vertex_properties> q;
    q.push (source) ;
    vector<vertex_properties> visited (num_nodes ()) ;



    while (!q.empty ()) {
      
      vertex_properties u = q.front () ;
      

      for (auto v_edge : adj[u]) {

        // v is an edge element.
        // Therefore, must retreive the destination vertex from it.
        vertex_properties v = *(v_edge.destination) ;

        
        if (!visited[v]) { 


          // Cannot index into I guess . 
          q.push (v) ;
          visited[v]=true ;
        } 
      }
    }
  }
};
































  
  
  
  
