function __push__ (Graph g, node u, node v, propNode <int> excess,propEdge <int> residual_capacity) {
    edge forward_edge = g.get_edge (u, v) ;
    edge backward_edge = g.get_edge (v, u) ;
    int d = min (u.excess, forward_edge.residual_capacity) ;
    int temp ;
    temp = (u.excess - d) ;
    u.excess = temp ;
    temp = (v.excess + d) ;
    v.excess = temp ;
    temp = forward_edge.residual_capacity - d ;
    forward_edge.residual_capacity = temp ;
    temp = backward_edge.residual_capacity + d ;
    backward_edge.residual_capacity = temp ;
    
    if (v != src && v != snk) {
        g.frontier_push(v) ;
    }
}

function relabel (Graph g, node u, propEdge <int> residue, propNode <int> label, container<int> count) {
    
    int new_label = INT_MAX ;
    for (v in g.neighbors (u)) {
        edge residual_capacity = g.get_edge (u, v) ;
        if (residual_capacity.residue > 0) {
            if (new_label < v) {
               new_label = v ; 
            }
        }
    }
    
    if (new_label < INT_MAX) {
        u.label = new_label+1 ;
        updateGap (count, u.label, amount) ;
    }
}

function discharge (Graph g, node u, propNode <int> label, propNode <int> excess, propNode <int> curr_edge, propEdge <int> residue, container<int> count) {
    
    while (u.excess > 0) {
        
        for (v in g.neighbors(u)) {
                       
            edge current_edge = g.get_edge (u,v) ;
            if (u.excess > 0 && current_edge.residue > 0 && u.label == v.label+1) {
                __push__ (g, u, v, excess, residue) ;     
            }
        }
        
        if (u.excess > 0) {
            relabel (g, u, residue, label, count) ;
            fixGap (count, label) ;
        }
    }
}

function updateGap (container<int> count, int height, int amount) {
    count[height] = amount ;
} 

function fixGap (container<int> count, propNode <int> label) {
    int gap = count.getIdx (0) ;
    int t ;
    forall (v in g.nodes()) {
        if (v.label >= gap ) {
            v.label = INT_MAX ;
        }
    }  
}

function do_max_flow (Graph g, node source, node sink, propNode<int> label, propNode<int> excess, propNode<int> curr_edge, propEdge<int> residue) {

    residue=g.weights ;
    g.attachNodeProperty (label = INF) ;
    g.attachNodeProperty (excess = 0) ;
    g.attachNodeProperty (curr_edge = 0) ;
    container<int> count ;
    count.assign (g.numNodes (), 0) ;

    int temp = 0;
    int res = 0 ;

    forall (v in g.neighbors(source)) {
        edge e = g.get_edge (source, v) ;
        v.excess = e.residue; 
    }

    
    source.excess=temp ;
    source.label=g.num_nodes ();
    int x = source.label ;
    
    while (!g.frontier_empty (world)) {
        node u = g.frontier_pop_local (world) ;
        discharge (g, u, label, excess, curr_edge, residue) ;
    }
}
