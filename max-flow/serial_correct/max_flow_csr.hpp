/*
 * Redo the entire thing without the structs.. Keep arrays 
 * Array for height, current edge (index number into adj[u]), 
 * Separate array for flow
 * Reverse csr 
 * set of active vertices
 * New Issue : csr not being generated.
 * It's going into resetting of edges needed and then it's not finding any edges.. So is it calling relabel ? Check that out.
 * If f is a preflow and d is any valid labeling for f and v is any active vertex, a push or a relabel is applicable.
 * Now what? ̰
 * We have a queue which maintains the active vertices.
*/

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
  queue<int> active_vertices ;
  int * excess ;


  // The adjacency list of the graph.
  // TO:DO - Change to CSR/ unordered_map post discussion.
  int * h_offset ;
  int * csr ;

  // get reverse_csr as well .
  int * r_offset ;
  int * r_csr ;
  int * capacities ;
  int * flow ;
  int * residual_flow ;
  int * indexer ;



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
    this->r_offset = getRevIndexofNodes () ;
    this->capacities = getEdgeLen () ;
    this->flow = (int*)malloc ((num_edges()+1)*sizeof(int)) ;
    this->residual_flow = (int*)malloc ((num_edges()+1)*sizeof(int)) ;
    this->excess = (int*) malloc ((num_edges() + 1)*sizeof(int)) ;
    this->r_csr = getSrcList () ;
	this->indexer = getEdgeMap () ;


    for (int i=0; i<=num_nodes();i++) {
        current_edges[i] = 0 ;
        heights[i] = 0 ;
    }

    for (int i=0; i< num_edges(); i++) {
        flow[i]=0 ;
        residual_flow[i]=capacities[i] ;
    }

	 cerr << "successful init" << endl ;

    set_up_excess () ;
    //cerr << "setup of excess successful\n" ;


    //cerr << "network is as follows : \n" ;
    
    cerr << "csr list : \n" ;
    print_arr (this->h_offset, num_nodes()+1 ) ;
    print_arr (this->csr, num_edges() ) ;

    cerr << "reverse csr list : \n" ;
    print_arr (this->r_offset, num_nodes()+1 ) ;
    print_arr (this->r_csr, num_edges() ) ;

    cerr << "capacities : \n" ;
    print_arr (this->capacities, num_edges() ) ;

    cerr << "flow : \n" ;
    print_arr (this->flow, num_edges() ) ;

    cerr << "residual flow : \n " ;
    print_arr (this->residual_flow, num_edges() ) ;
    print_arr (this->current_edges, num_nodes() +1) ;

    //cerr << "heights : \n" ;
    //print_arr (this->heights, num_nodes()+1) ;

    cerr << "excess : \n" ;
    print_arr (this->excess, num_nodes ()) ;

	cerr << "edgeMap : \n " ;
	print_arr (this->indexer, num_edges ()) ;
    //cerr << "heights : \n" ;
    //print_arr (this->heights, num_nodes()+1) ;
    //cerr << "excess: \n" ;
    //print_arr (this->excess, num_nodes ()+1 ) ;
    set_up_heights () ;
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
    active_vertices.push(source) ;
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

        excess[0] += capacities[x] ;
    }   
  }




  void set_up_heights () {

    /* 
     * Do a BFS to find the heights of all the vertices.
     * Set up current edges while at it.̦
     * */


    queue <int> q;
    q.push (sink) ;
    vector<int> visited (num_nodes ()+1, 0) ;

    //cerr << q.size () << endl ;
    //cerr << "inside the BFS function, initialization for BFS successful\n" ;
	while (!q.empty() ) {

	  int u = q.front () ;
	  cout << u << endl ;
	  // cout << u << endl ;
      q.pop();
      visited[u] = 1;

	  assert (u+1 < num_nodes()+1|| !(cerr << u+1 << " " << num_nodes ()<<endl));
      for (int v = r_offset[u]; v< r_offset[u+1]; v++) {

		  assert (v<num_edges()) ;
		  assert (r_csr[v] < num_nodes()) ;
        //cerr << csr[v] << endl ;
        if (!visited[r_csr[v]]) {

            visited[r_csr[v]] = true ;
            heights[r_csr[v]] = heights[u]+1 ;
        //    cerr << heights[r_csr[v]] << " has been reset \n" ;
            q.push(r_csr[v]) ;
        }
      }
    }
  }

  

  bool relabel (int relabel_this_vertex) {
    
    /*
     * Relabeling is wrong here.
     * Change it such that :
     * relabel this vertex's labels are changed to the largest possible.
     * valid labelling implies d(v) <= d(w)+1
     * relabel only when all heights of positive flow vertices are greater than current vertex.
     * if push is not applicable to v d(v)<d(w)+1 ????  
    */

 //   cerr << "inside relabeling: " << endl ;
    int minim = INT_MAX ; // Find a better upper limit.
	int relabel_next_vertex = relabel_this_vertex ;

    for (int v = h_offset[relabel_this_vertex] ; v < h_offset[relabel_this_vertex+1]; v++) {
      
      if ( heights[relabel_this_vertex] >= heights[csr[v]] and capacities[v]-flow[v] > 0 ) {

//		  cerr << "viable relabel " << relabel_this_vertex  << " " << csr[v] << " " << capacities[v] << " " << flow[v] << endl ;
       	    minim = min (minim, heights[csr[v]]) ;
	  		if (heights[csr[v]]<minim) {
				minim = heights[csr[v]] ;
				relabel_next_vertex = csr[v] ;
			}
    	}
	}

    if ( minim >= (INT_MAX-1) || minim == 0 ) {
//		cerr << "labeling failed " << endl ;
      return false ;
    }

	// cerr << " reassigning the heights " << endl ;
	// print_arr_log (heights, num_nodes()) ;
	heights[relabel_this_vertex] = minim+1 ;
	// print_arr_log (heights, num_nodes ()) ;
	// cerr << " =======================\n" << endl ;
	// print_arr_log (flow, num_edges()) ;
    return true ;
  }



  bool push (int active_vertex) {


    // indexing is wrong current_edges[active_vertex] gives the offset
    if (excess[active_vertex] <=0 ) {
        return false ;
    }
	  //cerr << "pushing on " << active_vertex << endl ;
    // indexing is wrong current_edges[active_vertex] gives the offset
    assert (excess[active_vertex]>0) ;

    if (active_vertex == sink) {
        // cerr << "hit sink " << " " << excess[active_vertex] << endl ;
        return false ;
    }

    
	assert (active_vertex < num_nodes () || !( cerr<< active_vertex << " " << num_nodes () << endl )) ; 
	assert (h_offset[active_vertex]+current_edges[active_vertex] < num_edges () || !(cerr << h_offset[active_vertex] << " " << current_edges[active_vertex] << " " << num_edges ())) ;

    if (heights[active_vertex] != heights[csr[h_offset[active_vertex]+current_edges[active_vertex]]]+1 or capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]]<=0) {
	 
		
//		cerr << capacities[h_offset[active_vertex]+current_edges[active_vertex]] << endl ;
//	 	cerr << flow[h_offset[active_vertex]+current_edges[active_vertex]] << endl ;
      // current edge is no longer a good option
    assert(capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]] == residual_flow[h_offset[active_vertex]+current_edges[active_vertex]]) ;
	  cerr << "current edge failed " << endl ;
//	  cerr << active_vertex << " " << csr[h_offset[active_vertex]+current_edges[active_vertex]] << endl ;
	  print_arr_log (csr, num_edges ()) ;
	  cerr << capacities[h_offset[active_vertex]+current_edges[active_vertex]] << " " << flow [h_offset[active_vertex]+current_edges[active_vertex]] << endl ;
	
	if (h_offset[active_vertex]+current_edges[active_vertex] == num_edges ()) {
		return false;
	}
    
	assert (active_vertex < num_nodes () || !( cerr<< active_vertex << " " << num_nodes () << endl )) ; 
	assert (h_offset[active_vertex]+current_edges[active_vertex] < num_edges () || !(cerr << active_vertex<<" "<<h_offset[active_vertex] << " " << current_edges[active_vertex] << " " << num_edges ())) ;

    if (heights[active_vertex] != heights[csr[h_offset[active_vertex]+current_edges[active_vertex]]]+1 or capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]]<=0) {
	 
      if (!reset_current_edge (active_vertex)) {

        return false ;
      } ;
    }

    if (h_offset[active_vertex]+current_edges[active_vertex] >= h_offset[active_vertex+1]) {
        return false ;
    }
    

    if (capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]]<=0) {

        // try next
        return false ;
    }



    assert(capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]] == residual_flow[h_offset[active_vertex]+current_edges[active_vertex]]) ;
    // push flow from a particular vertex to all subsequent vertices .. 
    int curr_flow = min (excess[active_vertex], capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]]) ;

    cerr << "pushing flow from " << active_vertex << " to " << csr[h_offset[active_vertex]+current_edges[active_vertex]] << endl ;
    cerr << "updating flow " << curr_flow <<  endl ;

    // update flow and residual flow.
    cerr << "pushing flow from " << active_vertex << " to " << csr[h_offset[active_vertex]+current_edges[active_vertex]] << " " << curr_flow << endl ;
    
    flow[h_offset[active_vertex]+current_edges[active_vertex]] += curr_flow ;
    print_arr (this->flow, num_edges ()) ;

    assert(r_offset[indexer[h_offset[active_vertex]+current_edges[active_vertex]]] >= 0);      
    flow[r_offset[indexer[h_offset[active_vertex]+current_edges[active_vertex]]]] -= curr_flow ;      
    print_arr (this->flow, num_edges ()) ;

    assert (flow[h_offset[active_vertex]+current_edges[active_vertex]] == capacities[h_offset[active_vertex]+current_edges[active_vertex]] - flow[r_offset[indexer[h_offset[active_vertex]+current_edges[active_vertex]]]]) ;
    // update excess of active vertex and current vertexclear

    excess[active_vertex] -= curr_flow ;
    excess[csr[h_offset[active_vertex]+current_edges[active_vertex]]] += curr_flow ;


    if (excess[csr[h_offset[active_vertex]+current_edges[active_vertex]]] > 0) {

        active_vertices.push (csr[h_offset[active_vertex]+current_edges[active_vertex]]) ;
    }

    
    if (excess[active_vertex] > 0 and heights[active_vertex] < 999999999) {

      active_vertices.push (active_vertex) ;
    }

    
    return false ;
  }



  bool reset_current_edge (int active_vertex) {


    // search for edges leading to vertices with height exactly lesser by 1.
	// do we need to change this ?
	// Is any height doable ?
	// cerr << "searching for a proper edge for vertex " <<active_vertex << endl ;

    int success = false ;
    int pointer = 0  ;
	
    for (int v = h_offset[active_vertex]; v < h_offset[active_vertex+1]; v++) {

      if (heights[csr[v]] == heights[active_vertex]-1 && capacities[v]-flow[v]>0) {

        current_edges[active_vertex] = pointer++;

	//	cerr << "found a good enough edge " << endl ;
	//	cerr << "new pointer " << pointer << endl ;
	//	cerr << "new edge " << active_vertex << " " << csr[h_offset[active_vertex]+pointer] << endl ;
        success = true ;
        return true ;
      }
      pointer++ ;
    }


    if (!success){
        
        return relabel(active_vertex) ;
    }
    return false ;
  }

  void push_and_relabel () {

    //cerr << " preflow push relable \n" ;
    // select an active vertex.
    int current_vertex = active_vertices.front () ;

    active_vertices.pop () ;

    // push (current_vertex) ;
	//cerr<< "discharging " << active_vertices.size () << " " << current_vertex << endl ; 
    while (push (current_vertex));
    print_arr_log (this->flow, num_edges()) ;
    

//	cout << active_vertices.size () << " || " ;

    if (active_vertices.size () == 0) {

      ACTIVE_VERTEX_EXISTS=false ;
    }
  }

    void print_result () {

		long long max_flow = 0 ;
    print_arr (this->flow, num_edges()) ;

		for (int i=0; i<num_edges() ; i++) {
			if (csr[i] == sink) {
				max_flow += flow[i] ;				
//				cerr << "flow from " << r_csr[i] << " to " << csr[i] << " is " << flow[i] << endl ;
			}
		}
        cout << "max_flow = " << max_flow << endl ; 
    }
};
 
  
