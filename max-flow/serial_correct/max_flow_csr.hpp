#include <climits>
#include "graph_mpi_weight.hpp"
#include <assert.h>
#include <queue>



bool ACTIVE_VERTEX_EXISTS = true; 

class network_flow : public graph {

/*
 * network_flow type graph based on the class graph.
 * additionally stores edge capacities and current vertices.
 * Also maintains a residual height
 * Each vertex has an excess and a height associated with it.
 * */

private:

  // source and sink to be defined by user. 
  int source ;
  int sink ;
  int * heights ;
  int * current_edges ;
  int * excess ;
  int * h_offset ;
  int * csr ;
  int * capacities ;
  int * flow ;
  queue<int> active_vertices ;

public:
  
  network_flow (char* file_name, int source, int sink): graph (file_name) {

    // call all relevant methods.
    parseGraph () ;
    select_source (source) ;
    select_sink(sink) ;
    this->heights = (int*)malloc ((num_nodes()+1)*sizeof(int)) ;
    this->current_edges = (int*) malloc ((num_nodes()+1)*sizeof(int)) ;
    this->h_offset = getIndexofNodes ();
    this->csr = getEdgeList () ;
    this->capacities = getEdgeLen () ;
    this->flow = getFlow ();
    this->excess = (int*) malloc ((num_edges() + 1)*sizeof(int)) ;


    for (int i=0; i<=num_nodes();i++) {
        current_edges[i] = 0 ;
    }

    
    cerr << "csr list : \n" ;
    print_arr (this->h_offset, num_nodes()+1 ) ;
    print_arr (this->csr, num_edges() ) ;

    cerr << "capacities : \n" ;
    print_arr (this->capacities, num_edges() ) ;

    cerr << "flow : \n" ;
    print_arr (this->flow, num_edges() ) ;

    set_up_excess () ;
    set_up_heights () ;
    
    cerr << "excess : \n" ;
    print_arr (this->excess, num_nodes ()) ;
    cerr << "flow : \n" ;
    print_arr (this->flow, num_edges() ) ;

  }

  void print_arr_log (int*arr ,int n) {

    for (int i=0; i<n; i++) {
        cout << arr[i] << " " ;
    }cout << endl ;
  }

  void print_arr (int*arr ,int n) {

    for (int i=0; i<n; i++) {
        cerr << arr[i] << " " ;
    }cerr << endl ;
  }

  void select_source (int source) {
    
    // set source of the network_flow. 
    this->source = source ;
  }


  void select_sink (int sink) {

    // set sink of the network flow.
    this->sink = sink ;
  }


  void set_up_excess () {

    for (int i=0; i<=num_nodes(); i++) {
        excess[i]= 0 ;
    }

    for (int x = h_offset[source]; x<h_offset[source+1]; x++) {
        if (csr[x] != sink) 
          active_vertices.push (csr[x]) ;
        for (int y=h_offset[csr[x]]; y < h_offset[csr[x]+1]; y++) {
          if (csr[y] == source) {
            flow[x] += flow[y] ;
            excess[csr[x]] += flow[y] ;
            flow[y] = 0 ;
          }
          
        }
    }   
  }


  void set_up_heights () {

    /* 
     * Do a BFS to find the heights of all the vertices.
     * Set up current edges while at it.̦
     * */

    for (int i=0; i<num_nodes (); i++) {
      heights[i] = 0 ;
    }
    heights[source] = num_nodes () ;
  }

  

  bool relabel (const int &relabel_this_vertex) {


    cerr << "inside relabeling: " << endl ;

    cerr << "old heights" << endl ;
    print_arr (this->heights, num_nodes ()) ;
    int u = relabel_this_vertex ;

    if (u==source) return false ;
    int minim = heights[u] ; // Find a better upper limit.
    assert (u < num_nodes ()) ;

    for (int v_idx = h_offset[u] ; v_idx < h_offset[u+1]; v_idx++) {
      if (capacities[v_idx]-flow[v_idx] > 0)
        minim = min (minim, heights[csr[v_idx]]) ;
	  }

    if ( minim >= heights[source] || minim < 0 ) {
      return false ;
    }

    if (heights[u] == minim+1) {
      return false ;
    }
    heights[u] = minim+1 ;
    cerr << "new heights" << endl ;
    print_arr (this->heights, num_nodes ()) ;

    return reset_current_edge (u) ;
  }



  bool push (const int &active_vertex) {
    
	  cerr << "pushing on " << active_vertex << endl ;

    
    int v_idx = h_offset[active_vertex]+current_edges[active_vertex] ;
    int v = csr[v_idx] ;
    int u = active_vertex ;
    assert (u != sink) ;

    if (excess[u]<=0) {
      return false ;
    }

    if ((heights[u] != heights[v]+1) or (capacities[v_idx]-flow[v_idx]<=0)) {
	 
      if (!reset_current_edge (active_vertex)) {
        return false ;
      } ;
    }

    v_idx = h_offset[active_vertex]+current_edges[active_vertex] ;
    v = csr[v_idx] ;
    int u_idx = h_offset[v] ;
    for ( ; u_idx<h_offset[v+1]; u_idx++) {
      if (csr[u_idx] == u) {
        break ;
      }
    }


    assert (excess[u]>0|| !(cerr << u << " " << excess[u] << endl )) ;
    assert (u < num_nodes () ) ;
	  assert (v_idx < num_edges ()) ;
    assert (v_idx < h_offset[u+1]) ;
    assert (capacities[v_idx]-flow[v_idx]>0 || !(cerr << capacities[v_idx] << " " <<flow[v_idx] << " " << u << " <---> " << v << endl )) ;
    assert (u_idx<num_edges ()) ;
    assert (u_idx<h_offset[v+1]) ;
    assert (capacities[u_idx]-flow[u_idx] == flow[v_idx] || !(cerr << capacities[u_idx] << " " << flow[u_idx] << " " << flow[v_idx] <<" " << u_idx<< " " << v_idx<< " from "<< u << " to " << csr[v_idx] << endl )) ;

    // push flow from a particular vertex to all subsequent vertices .. 
    int curr_flow = min (excess[active_vertex], capacities[v_idx]-flow[v_idx]) ;

    cerr << "updating flow " << curr_flow <<  endl ;
    cerr << "pushing flow accross edge " << u << " " << v << " with indices "<<u_idx <<" " <<v_idx<< " " << curr_flow << endl ;
    
    cerr << " ------------------------------------- " << endl ;
    flow[v_idx] += curr_flow ;
    print_arr (this->flow, num_edges ()) ;


    flow[u_idx] -= curr_flow ;      
    assert (flow[u_idx]>=0) ;
    print_arr (this->flow, num_edges ()) ;

    print_arr (this->excess, num_nodes ()) ;
    excess[u] -= curr_flow ;
    excess[v] += curr_flow ;
    print_arr (this->excess, num_nodes ()) ;
    cerr << " -------------------------------------- " << endl ;
     if (excess[v] > 0 && curr_flow > 0 && v != sink) {
        active_vertices.push (v) ;
    }
    if (excess[u]>0) {
      push (u) ;
    }
    return false ;
  }

  bool reset_current_edge (const int &active_vertex) {

    cerr << "changing edges" << endl ;
    int pointer = 0  ;
	
    for (int v_idx = h_offset[active_vertex]+pointer; v_idx < h_offset[active_vertex+1]; v_idx++) {
      assert (h_offset[active_vertex]+pointer == v_idx) ;
      if (heights[csr[v_idx]] == heights[active_vertex]-1 && capacities[v_idx]-flow[v_idx]>0) {

        current_edges[active_vertex] = pointer;
        cerr << "new edge " << active_vertex << " <----> " << csr[v_idx] << endl ;
        assert (capacities[h_offset[active_vertex]+current_edges[active_vertex]] > flow [h_offset[active_vertex]+current_edges[active_vertex]]) ;
        return true ;
      }
      pointer++ ;
    }
    return relabel(active_vertex) ;
  }

  void push_and_relabel () {

    int current_vertex = active_vertices.front () ;
    active_vertices.pop () ;

    while (push (current_vertex));
    print_arr (this->flow, num_edges()) ;

    if (active_vertices.size () == 0) {
      ACTIVE_VERTEX_EXISTS=false ;
    }
  }

  void print_result () {

  long long max_flow = 0 ;
  print_arr (this->flow, num_edges()) ;
  print_arr (this->excess, num_nodes ()) ;

  for (int i=0; i<num_edges() ; i++) {
    if (csr[i] == source) {
      max_flow += flow[i] ;				
    }
  }
      cout << "max_flow = " << excess[sink] << endl ; 
  }
};