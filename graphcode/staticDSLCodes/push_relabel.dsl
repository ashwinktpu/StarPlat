function __push__ (Graph g, node u, node v_idx, propNode <int> excess,propEdge <int> residual_capacity) {
    node v = g.get_other_vertex (u, v_idx) ;
    edge forward_edge = g.get_edge (u, v) ;
    edge backward_edge = g.get_edge (v, u) ;
    int d = std::min (u.excess, forward_edge.residual_capacity) ;
    u.excess -= d ;
    v.excess += d ;
    forward_edge.residual_capacity -= d ;
    backward_edge.residual_capacity += d ;
    
    if (v != src && v != snk) {
        g.frontier_push(v) ;
    }
}

function relabel (Graph g, node u, propEdge <int> residue, propNode <int> label, container<int> count) {
    
    int x = u.label ;
    int new_label = g.num_nodes () + 2;
    for (v in g.neighbors (u)) {
        edge residual_capacity = g.get_edge (u, v) ;
        if (residual_capacity.residue > 0) {
            if (new_label < v) {
               new_label = v ; 
            }
        }
    }
    
    if (new_label < g.num_nodes () + 1 && new_label + 1 > x) {
        u.label = new_label+1 ;
        count[oldIdx] -= 1 ;
        count[new_label + 1] += 1 ;
        u.label = new_label + 1 ;
    }
}

function discharge (Graph g, node u, propNode <int> label, propNode <int> excess, propNode <int> curr_edge, propEdge <int> residue, container<int> count) {
    
    bool go = true ;
    while (u.excess > 0 && go) {
        
        for (v in g.neighbors(u)) {
          int vIdx = u.curr_edge ;
          node v = g.get_ither_vertex (u, vIdx) ;
          edge current_edge = g.get_edge_i (u, vIdx) ;
          if (u.excess > 0 && current_edge.residue > 0 && u.label == v.label+1) {
            __push__ (g, u, vIdx, excess, residue) ;
          }
        }
        
        
        if (u.excess > 0) {
          int prevValue = u.label ;
          relabel (g, u, residue, label, count) ;

          if (u.label == prevValue) {
            go = false ;
          }
          u.curr_edge = 0 ;
       }
    }
}


function fixGap (container<int> count, propNode <int> label) {
    int gap = count.getIdx (0) ;
    if (gap == -1) return 1 ;
    forall (v in g.nodes()) {
        if (v != src && v != snk && v.label >= gap ) {
            v.label = g.num_nodes () + 2 ;
        }
    }  
}

function do_max_flow (Graph g, node source, node sink, propNode<int> label, propNode<int> excess, propNode<int> curr_edge, propEdge<int> residue) {

    residue=g.weights ;
    g.attachNodeProperty (label = INF) ;
    g.attachNodeProperty (excess = 0) ;
    g.attachNodeProperty (curr_edge = 0) ;
    container<int> count ;
    count.assign (g.num_nodes (), 0) ;

    int temp = 0;
    int res = 0 ;

    forall (v in g.neighbors(source)) {
        edge e = g.get_edge (source, v) ;
        edge r_e = g.get_edge (v, source) ;
        v.excess = e.residue; 
        r_e.excess += e.residue ;
        e.residue = 0 ;
    }

    
    source.label=g.num_nodes ();
    int x = source.label ;
    
    while (!g.frontier_empty ()) {
        node u = g.frontier_pop_local () ;
        if ( u != -1 ) 
        discharge (g, u, label, excess, curr_edge, residue) ;
        if (activate == 0) {
          fixGap (g, count, label) ;
        } else {
          activate-- ;
        }
    }
}
