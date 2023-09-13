#include "graph_mpi_weight.hpp"
#include <assert.h>
#include <queue>


bool ACTIVE_VERTEX_EXISTS = true; 


struct vertex_properties ;
struct edge_elements ;


struct edge_elements {


  // struct to store entities relevant to an edge for the max flow problem.
  int flow ; // current flow value through the edge.
  int capacity ; // capacity of the edge.
  int residual ; // To represent the residual graph.
  vertex_properties *source; // vertex type struct containing the source vertex of the edge.
  vertex_properties *destination ; // vertex type struct containing the destination vertex of the edge.

};

struct vertex_properties {

  
  // struct to store all information relevant to a vertex.
  bool active ; // whether the vertex is active or not,
  int excess ; // amount of excess flow passing through the vertex.
  int vertex_id ; // vertex_id of the vertex.
  

  int get_height (int* heights) {
    // get the heights.
    return heights[vertex_id] ;
  }

  void set_height (int* heights, int val) {
    // set the heights.

    cout << vertex_id << " accessing into heights array" << endl ;
    heights[vertex_id]=val ;
  }

  void set_current_edge (edge_elements** current_edges, edge_elements* current_edge) {
    // set current edge for the vertex

    current_edges[vertex_id] = current_edge ;
  }


  edge_elements* get_current_edge (edge_elements** current_edges) {

    // get the current edge for this vertex.
    return current_edges[vertex_id] ;
  }

  bool operator< (const vertex_properties &other)  {
    
    // for storing the vertices in an associative container. To change to Hash type container.
    return vertex_id < other.vertex_id ;  
  }
  
  // for use in visited array. 
  int operator[] (vertex_properties a) {

    // 
    return a.vertex_id ;
  }
} ;



bool operator< (const vertex_properties &a, const vertex_properties &b) {

  // overload comparator so that vertex_properties may be used in the map.
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
  int * heights ;
  edge_elements** current_edges ;
  set<vertex_properties> active_vertices ;

  // The adjacency list of the graph.
  // TO:DO - Change to CSR/ unordered_map post discussion.
  map <vertex_properties, vector<edge_elements> > adj;

public:
  


  network_flow (char* file_name, vertex_properties source, vertex_properties sink): graph (file_name) {

    // call all relevant methods.
    parseGraph () ;
    select_source (source) ;
    select_sink(sink) ;
    this->heights = (int*)malloc ((num_nodes()+1)*sizeof(int)) ;
    cerr << "size of heights " << num_nodes() + 1 << endl ;
    this->current_edges = (edge_elements**)malloc ((num_nodes()+1)*sizeof(edge_elements*)) ;
    initialize() ;
    cout << " inside network_flow's constructor " << endl ;
  }


  void select_source (vertex_properties source) {
    
    // set source of the network_flow. 
    this->source = source ;
  }


  void select_sink (vertex_properties sink) {

    // set sink of the network flow.
    this->sink = sink ;
  }


  void initialize () {
    /*
     * initialize the adjacency list of the array and create the max flow.*/
    

    // retreive edge list from the parsed graph. It is actually sent back as an adjacency list for some reason.
    map <int, vector<edge>> adj_list = getEdges();   

    cerr << "heights indexing to 1" << 1 << " " <<heights[1] << endl ;
    cerr<<"beginning the actual process of initialization. Edges obtained\n" ;
    // TO:DO change find a better way to represent the adjacency list. 

    cerr << adj_list.size() << " for reference \n " ;
    for (auto edge:adj_list) {
    
      // for every vertex in adjacency list (confusion caused because of naming conventions prior).
      edge_elements edge_elements_new;
      
      cerr << "hello world \n" ;
      cerr << "now processing edge : " ;
      cerr << edge.first << endl ;

      // Make the vertex properties struct for current vertex. Initialize the the members of the struct.
      vertex_properties this_vertex;
      this_vertex.vertex_id = edge.first;
      this_vertex.active = true;
      this_vertex.excess = 0;
      
      cerr << "trying to set vertiecs :  " <<endl ;
      this_vertex.set_height(heights, 0) ;
      cerr << "succesful set : " << endl ;
      // The particular vertex. 
      cout << this_vertex.vertex_id << " : " ;

      // Create empty vector just in case this is not supported.
      if (adj.find (this_vertex) == adj.end ()) {
        adj[this_vertex] = {};
      }

      // Fill up the adjacency list (vector) for the current vertex.
      for (auto edge_props:edge.second) {


        // create a destination vertex, initialize all its members.
        vertex_properties *destination = new vertex_properties ();
        destination->excess = 0 ;
        destination->active = true ;
        destination->vertex_id = edge_props.destination ;

      cerr << "trying to set vertiecs :  " <<endl ;
        destination->set_height(heights, 0) ;

      cerr << "succesful set : " << endl ;

        // initialize the edge struct and all its members.
        edge_elements_new.flow = 0 ;
        edge_elements_new.capacity = edge_props.weight ;
        edge_elements_new.destination = destination ;

        cout << destination->vertex_id << " " ;

        // Add the edge to the adjacency matrix.
        // a copy is being pushed .
        adj[this_vertex].push_back (edge_elements_new) ;
         
      }
      cout << endl ;
    }

    cerr << "structure for graph created succesfully\n" ;
    // End To:DO

    // initialize the heights and the excesses.
    set_up_heights () ;
    set_up_excess () ;

  }

  void set_up_excess () {

    
    // set up the excesses. initialize source to be sum of all capacities outgoing and all others to 0.
    for (auto v_edge:adj[source]) {
      vertex_properties v= *v_edge.destination ;
      source.excess += v_edge.capacity ;
    }

    // all else is already at 0 ;
  }




  void set_up_heights () {

    /* 
     * Do a BFS to find the heights of all the vertices.
     * Set up current edges while at it.̦
     * */


    queue <vertex_properties*> q;
    q.push (&sink) ;
    vector<int> visited (num_nodes (), 0) ;

    cerr << q.size () << endl ;
    cerr << "inside the BFS function, initialization for BFS successful\n" ;
    // Run the BFS for loop.
    
    active_vertices.insert (sink) ;
    
    while (!q.empty()) {
    
      vertex_properties* u = q.front();
      q.pop();
      visited[u->vertex_id] = 1;

      active_vertices.insert (*u) ;

      for (auto &v_edge : adj[*u]) {

          // Get the destination vertex from the edge element by reference.
          vertex_properties* v = v_edge.destination;

          // Check if the destination vertex has been visited.
          if (!visited[v->vertex_id]) {

              // Increment the height of the destination vertex.
              v->set_height(heights, u->get_height(heights)+1);

              // Push the destination vertex into the queue.
              q.push(v);
              visited[v->vertex_id] = true;

            v_edge.destination = v ;
            v->set_current_edge(current_edges, &adj[*u][0]) ;
            cerr << "set height to " << v_edge.destination->get_height(heights)<<endl ;
          }
      }
    }
  }

  

  bool relabel (vertex_properties relabel_this_vertex) {
    
    // relabel the passed vertex.. Change it's height to the lowest height among it's adjacent vertics increased by 1
    //
    int minim = 1000000000 ; // Find a better upper limit.
    for (auto edge : adj[relabel_this_vertex]) {

      minim = min (minim, edge.destination->get_height(heights)+1) ;
    }
    relabel_this_vertex.set_height(heights, minim) ;

    if (minim == 1000000000) {
      return false ;
    }
    return true ;
  }



  bool push (vertex_properties active_vertex) {

    cout << "manual printing : "; 
    cout << (*current_edges[0]).destination->vertex_id <<endl ;
    cout << (*current_edges[1]).destination->vertex_id <<endl ;
    cout << (*current_edges[2]).destination->vertex_id <<endl ;
    cout << (*current_edges[3]).destination->vertex_id <<endl ;


    // print_current_edges () ;
    cout << "working with active vertex : " << active_vertex.vertex_id << endl ;
    // check whether the current edge is still a valid edge to push heights to.
    cout << active_vertex.get_height(heights) << " height of current vertex" << endl ;

    // location of error....
    cout << "current vertex " << (*active_vertex.get_current_edge(current_edges)).destination->vertex_id << endl; 
    cerr <<"succesful access" << endl ;
    if (active_vertex.get_height(heights) != ((*active_vertex.get_current_edge(current_edges)).destination->get_height(heights)+1)) {

      cout << "readjusting current edge "  <<endl ;
      // if not reset the current edge for the current vertex.
      reset_current_edge (active_vertex) ;
    }
    // push flow from a particular vertex to all subsequent vertices .. 
    int curr_flow = min (active_vertex.excess, active_vertex.get_current_edge(current_edges)->flow) ;

    cerr << "updating flow " << endl ;

    // update all relevant values
    active_vertex.get_current_edge(current_edges)->residual += curr_flow ;
    active_vertex.get_current_edge(current_edges)->flow -= curr_flow ;      
    active_vertex.excess -= curr_flow ;


    cout << "push successful from active vertex: " << active_vertex.vertex_id << endl ;

    if (active_vertex.excess == 0 and active_vertex.get_height(heights) <= 0) {
      return false ;
    }
    // TO:DO 
    // When to Deactivate the Vertex ?? 
    return true ;
  }



  void reset_current_edge (vertex_properties active_vertex) {


    // search for edges leading to vertices with height exactly lesser by 1.
    cout << "started readjusting current edge for active vertex " << active_vertex.vertex_id << endl ;
    int success = false ;
    for (auto v_edge:adj[active_vertex]) {

      // check whether v_edge satisfies the property.
      if (v_edge.destination->get_height(heights) == active_vertex.get_height(heights)+1) {

        // update the active vertex's edge.
        active_vertex.set_current_edge(current_edges, &v_edge) ;
        // cerr << active_vertex.get_current_edge(current_edges)->destination->vertex_id << " this the new current edge's destination vertex" << endl ;
        success = true ;
        return ;
      }
    }

    cout << "successful readjusting current edge for active vertex " << active_vertex.vertex_id << endl ;

    // if control reaches here, relabelling is needed .
    if (!success)
      relabel(active_vertex) ;
  }

  void print_heights () {

    cerr << "graph heights \n" ;
    for (auto it:adj) {
      vertex_properties temp = it.first ;
      cerr << temp.vertex_id << " at height " << temp.get_height(this->heights) << " : " ;

      for (auto it1: it.second) {
        cerr << it1.destination->vertex_id << " " ;
      }
      cerr << endl ;
    }
  }


  void print_active_vertices () {

    cout << "active_vertices" << endl ;

    for (auto it:active_vertices) {
      cout << it.vertex_id << " ";
    }cout << endl ;
  }

  void push_and_relabel () {

    cout << " preflow push relable \n" ;
    // select an active vertex.
    vertex_properties current_vertex = *active_vertices.begin () ;
    // push flow to its current edge if possible
    while (push (current_vertex));
    // if not possible relabel
    active_vertices.erase (active_vertices.begin ()) ;

    if (active_vertices.size () == 0) {

      ACTIVE_VERTEX_EXISTS=false ;
    }
  }

  void print_current_edges () {

    cout << " current edges : " ;

    for (int i=0; i<num_nodes(); i++) {
      edge_elements curr_edge = *current_edges[i] ;
      cout << curr_edge.destination->vertex_id << " " ;
    }    cout << endl ;
  }
};
 
  
