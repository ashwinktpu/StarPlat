/*
 * Redo the entire thing without the structs.. Keep arrays 
 * Array for height, current edge (index number into adj[u]), 
 * Separate array for flow
 * Reverse csr 
 * set of active vertices
 * New Issue : csr not being generated.
 * It's going into resetting of edges needed and then it's not finding any edges.. So is it calling relabel ? Check that out.
 * If f is a preflow and d is any valid labeling for f and v is any active vertex, a push or a relabel is applicable.
 * Now what?
 * We have a queue which maintains the active vertices.
*/


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


    for (int i=0; i<=num_nodes();i++) {
        current_edges[i] = 0 ;
        heights[i] = 0 ;
    }

    for (int i=0; i< num_edges(); i++) {
        flow[i]=0 ;
        residual_flow[i]=capacities[i] ;
    }

    set_up_excess () ;
    cerr << "setup of excess successful\n" ;

    set_up_heights () ;

    cout << "network is as follows : \n" ;

    print_arr (this->h_offset, num_nodes()+1 ) ;
    print_arr (this->csr, num_edges() ) ;
    print_arr (this->r_offset, num_nodes()+1 ) ;
    print_arr (this->r_csr, num_edges() ) ;
    print_arr (this->capacities, num_edges() ) ;
    print_arr (this->flow, num_edges() ) ;
    print_arr (this->residual_flow, num_edges() ) ;
    print_arr (this->current_edges, num_nodes() +1) ;
    print_arr (this->heights, num_nodes()+1) ;
    cout << " inside network_flow's constructor " << endl ;
  }

  void print_arr (int*arr ,int n) {

    for (int i=0; i<n; i++) {
        cout << arr[i] << " " ;
    }cout << endl ;
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
        excess[x] += capacities[x] ;
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

    cerr << q.size () << endl ;
    cerr << "inside the BFS function, initialization for BFS successful\n" ;
    // Run the BFS for loop.
    // we'll have to do it on the reverse csr .
    
    while (!q.empty()) {
        

      int u = q.front();
      cerr << "trying for "  << u << " " << h_offset[u] << " " <<h_offset[u+1] << endl ;
      q.pop();
      visited[u] = 1;

      for (int v = r_offset[u]; v< r_offset[u+1]; v++) {

        cout << csr[v] << endl ;
        if (!visited[r_csr[v]]) {

            visited[r_csr[v]] = true ;
            heights[r_csr[v]] = heights[u]+1 ;
            cout << heights[r_csr[v]] << " has been reset \n" ;
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

    cout << "inside relabeling: " << endl ;
    int minim = 1000000000 ; // Find a better upper limit.
    for (int v = h_offset[relabel_this_vertex] ; v <= h_offset[relabel_this_vertex+1]; v++) {
      
      if ( heights[relabel_this_vertex] <= heights[csr[v]] and residual_flow[csr[v]] > 0 )
        minim = min (minim, heights[csr[v]]) ;

    }
    heights[relabel_this_vertex]=minim+1 ;

    if ( minim == 1000000000 || minim == 0 ) {

      return false ;
    }
    return true ;
  }



  bool push (int active_vertex) {

    // indexing is wrong current_edges[active_vertex] gives the offset

    if (active_vertex == sink) {
        cout << "hit sink " << endl ;
        return false ;
    }

    cerr << "pushing on vertex :  " << active_vertex << endl ;

    cerr<<"invoked push: " << heights[active_vertex] << " " << excess[active_vertex] << " " << heights[csr[h_offset[active_vertex]+current_edges[active_vertex]]] << endl ;

    
    if (heights[active_vertex] != heights[csr[h_offset[active_vertex]+current_edges[active_vertex]]]+1 or capacities[csr[h_offset[active_vertex]+current_edges[active_vertex]]]-flow[csr[h_offset[active_vertex]+current_edges[active_vertex]]]<=0) {

      cerr << "resetting of edges needed" << endl ;
      // current edge is no longer a good option
      if (!reset_current_edge (active_vertex) and excess[active_vertex]>0) {
        active_vertices.push (active_vertex) ;
        cerr << "no more edges left !! " << endl ;
        return false ;
      } ;
    }

    if (capacities[csr[h_offset[active_vertex]+current_edges[active_vertex]]]-flow[csr[h_offset[active_vertex]+current_edges[active_vertex]]]<=0) {

        if (excess[active_vertex]>0) {
            active_vertices.push (csr[h_offset[active_vertex]+current_edges[active_vertex]]) ;
        }
        return false ;
    }

    cerr << "current edge accepted" << endl ;
    // push flow from a particular vertex to all subsequent vertices .. 
    int curr_flow = min (excess[active_vertex], residual_flow[r_csr[r_offset[csr[h_offset[active_vertex]+current_edges[active_vertex]]]]]) ;

    cerr << "pushing flow from " << active_vertex << " to " << csr[h_offset[active_vertex]+current_edges[active_vertex]] << endl ;
    cerr << "updating flow " << curr_flow <<  endl ;
    cerr << "=================================================================================================\n" ;

    cerr << "prior state : " << endl ;
    print_arr (flow, num_edges()) ;
    print_arr (residual_flow, num_edges()) ;
    print_arr (excess, num_nodes()) ;
    cerr << "=================================================================================================\n" ;

    // update flow and residual flow.
    flow[csr[h_offset[active_vertex]+current_edges[active_vertex]]] += curr_flow ;

    residual_flow[r_csr[r_offset[csr[h_offset[active_vertex]+current_edges[active_vertex]]]]] -= curr_flow ;      

    // update excess of active vertex and current vertexclear


    excess[active_vertex] -= curr_flow ;
    excess[csr[h_offset[active_vertex]+current_edges[active_vertex]]] += curr_flow ;

    cerr << "current state : " << endl ;
    print_arr (flow, num_edges()) ;
    print_arr (residual_flow, num_edges()) ;
    print_arr (excess, num_nodes()) ;
    cerr << "=================================================================================================\n" ;


    if (excess[csr[h_offset[active_vertex]+current_edges[active_vertex]]] > 0) {

        active_vertices.push (csr[h_offset[active_vertex]+current_edges[active_vertex]]) ;
    }
    // skeptical about this deactivation.
    if (excess[active_vertex] > 0) {
      active_vertices.push (active_vertex) ;
      return false ;
    }
    
    return true ;
  }



  bool reset_current_edge (int active_vertex) {


    // search for edges leading to vertices with height exactly lesser by 1.

    int success = false ;
    for (int v = h_offset[active_vertex]; v < h_offset[active_vertex+1]; v++) {

      // check whether v_edge satisfies the property.
      cout << "inside finding new edge " << endl ;
      cout << heights[active_vertex] << " " << heights[csr[v]] ;
      if (heights[csr[v]] == heights[active_vertex]+1) {
        // update the active vertex's edge.
        current_edges[active_vertex] = csr[v] ;

        success = true ;
        return true ;
      }
    }

    // if control reaches here, relabelling is needed .
    if (!success){

        return relabel(active_vertex) ;
    }
    
    return false ;
  }

  void push_and_relabel () {

    cout << " preflow push relable \n" ;
    // select an active vertex.
    int current_vertex = active_vertices.front () ;

    active_vertices.pop () ;

    // push (current_vertex) ;
    while (push (current_vertex));

    cerr << "no more push operations" << endl ;
    cerr << active_vertices.size () << endl ;
    if (active_vertices.size () == 0) {

      ACTIVE_VERTEX_EXISTS=false ;
    }
  }

};
 
  