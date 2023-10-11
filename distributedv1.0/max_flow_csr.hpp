/*
 * Redo the entire thing without the structs.. Keep arrays [DONE]
 * Array for height, current edge (index number into adj[u]), [DONE]
 * Separate array for flow
 * Reverse csr
 * set of active vertices [DONE]
 * New Issue : csr not being generated. [RESOLVED]
 * It's going into resetting of edges needed and then it's not finding any edges.. So is it calling relabel ? Check that out.
 * If f is a preflow and d is any valid labeling for f and v is any active vertex, a push or a relabel is applicable.
 * Now what? ̰ [THINK]
 * We have a queue which maintains the active vertices. [DONE]
 */

#ifndef _MAX_FLOW_SERIAL_

#define _MAX_FLOW_SERIAL_

#include <climits>
#include "graph_mpi_weight.hpp"
#include <assert.h>
#include <queue>
bool ACTIVE_VERTEX_EXISTS = true;

class Network_flow : public graph
{

    /*
     * network_flow type graph based on the class graph.
     * additionally stores edge capacities and current vertices.
     * Also maintains a residual height
     * Each vertex has an excess and a height associated with it.
     * */

private:
    // source and sink to be defined by user.
    int source;
    int sink;
    int *heights;
    int *current_edges;
    queue<int> active_vertices;
    int *excess;
    int global_nodes;
    int global_edges;
    int rank ;
    int size ;

    // The adjacency list of the graph.
    // TO:DO - Change to CSR/ unordered_map post discussion.
    int *h_offset;
    int *csr;

    // get reverse_csr as well .
    int *r_offset;
    int *r_csr;
    int *capacities;
    int *flow;
    int *residual_flow;

    // For edge computing.
    vector<vector<int> > send_to_edge ;

public:
    Network_flow(char *file_name, int source, int sink) : graph(file_name)
    {

        // call all relevant methods.
        MPI_Comm_rank (MPI_COMM_WORLD, &rank) ;
        MPI_Comm_size (MPI_COMM_WORLD, &size) ;
        parseGraph();
        select_source(source);
        select_sink(sink);
        this->heights = (int *)malloc((num_nodes() + 1) * sizeof(int));
        this->current_edges = (int *)malloc((num_nodes() + 1) * sizeof(int));
        this->h_offset = getIndexofNodes();
        this->csr = getEdgeList();
        this->r_offset = getRevIndexofNodes();
        this->capacities = getEdgeLen();
        this->flow = (int *)malloc((num_edges() + 1) * sizeof(int));
        this->residual_flow = (int *)malloc((num_edges() + 1) * sizeof(int));
        this->excess = (int *)malloc((num_edges() + 1) * sizeof(int));
        this->r_csr = getSrcList();


        for (int i = 0; i <= num_nodes(); i++)
        {
            current_edges[i] = 0;
            heights[i] = 0;
        }

        for (int i = 0; i < num_edges(); i++)
        {
            flow[i] = 0;
            residual_flow[i] = capacities[i];
        }
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        cerr << "number of nodes in rank " << rank << " is " << num_nodes() << endl;

        set_up_excess();
        set_up_heights();
    }

    int get_residue_on_source()
    {
        if (exists_here(source))
        {
            return this->excess[get_local(source)];
        }
        else
        {
            return 0;
        }
    }

    int get_residue_on_sink()
    {
        if (exists_here(sink))
        {
            return this->excess[get_local(sink)];
        }
        else
        {
            return 0;
        }
    }

    int get_active_vertices () {
        return active_vertices.size () ;
    }

    void set_global_nodes(int global_nodes)
    {
        this->global_nodes = global_nodes;
    }

    void set_global_edges(int global_edges)
    {
        this->global_edges = global_edges;
    }

    int get_global_vertices()
    {
        return this->global_nodes;
    }

    int get_global_edges()
    {
        return this->global_edges;
    }
    void print_arr_log(int *arr, int n)
    {

        for (int i = 0; i < n; i++)
        {
            cout << arr[i] << " ";
        }
        cout << endl;
    }

    void print_arr(int *arr, int n)
    {

        for (int i = 0; i < n; i++)
        {
            cerr << arr[i] << " ";
        }
        cerr << endl;
    }

    void select_source(int source)
    {

        // set source of the network_flow.
        active_vertices.push(source);
        this->source = source;
    }

    int get_source()
    {
        return this->source;
    }

    void select_sink(int sink)
    {

        // set sink of the network flow.
        this->sink = sink;
    }

    void set_up_excess()
    {

        for (int i = 0; i <= num_nodes(); i++)
        {
            excess[i] = 0;
        }
        if (exists_here(source))
        {
            for (int x = h_offset[get_local(source)]; x < h_offset[get_local(source + 1)]; x++)
            {
                this->excess[0] += this->capacities[x];
            }
        }
    }

    void set_up_heights()
    {

        for (int i = 0; i <= num_nodes(); i++)
        {

            this->heights[i] = 0;
        }
        if (exists_here(get_local(source)))
        {
            this->heights[get_local(source)] = num_nodes();
        }
    }


    bool relabel(int relabel_this_vertex)
    {

        /*
         * Relabeling is wrong here.
         * Change it such that :
         * relabel this vertex's labels are changed to the largest possible.
         * valid labelling implies d(v) <= d(w)+1
         * relabel only when all heights of positive flow vertices are greater than current vertex.
         * if push is not applicable to v d(v)<d(w)+1 ????
         */

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

        heights[relabel_this_vertex] = minim+1 ;
    }


    bool push(int active_vertex)
    {

        // indexing is wrong current_edges[active_vertex] gives the offset
        assert (active_vertex >= 0 && active_vertex < num_nodes ()) ;
        assert (excess[active_vertex] <= 0) ;
        assert (h_offset[active_vertex] + current_edges[active_vertex] < num_edges ()) ;


        if (active_vertex == sink) {
            cerr << "hit sink " << endl;
            return false;
        }

        if (heights[active_vertex] != heights[csr[h_offset[active_vertex] + current_edges[active_vertex]]] + 1 or capacities[h_offset[active_vertex] + current_edges[active_vertex]] - flow[h_offset[active_vertex] + current_edges[active_vertex]] <= 0) {

            if (!reset_current_edge(active_vertex)) {

                return false;
            };
        }


        assert (h_offset[active_vertex] + current_edges[active_vertex] >= h_offset[active_vertex + 1]) ;
        assert (capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex] + current_edges[active_vertex]]<=0) ;
        assert(capacities[h_offset[active_vertex]+current_edges[active_vertex]]-flow[h_offset[active_vertex]+current_edges[active_vertex]] == residual_flow[h_offset[active_vertex]+current_edges[active_vertex]]) ;


        // push flow from a particular vertex to all subsequent vertices ..
        int curr_flow = min(excess[active_vertex], capacities[h_offset[active_vertex] + current_edges[active_vertex]] - flow[h_offset[active_vertex] + current_edges[active_vertex]]);

        // update flow and residual flow.
        flow[h_offset[active_vertex] + current_edges[active_vertex]] += curr_flow;

        residual_flow[h_offset[active_vertex] + current_edges[active_vertex]] -= curr_flow;

        // update excess of active vertex and current vertexclear

        excess[active_vertex] -= curr_flow;
        excess[csr[h_offset[active_vertex] + current_edges[active_vertex]]] += curr_flow;


        // add elements to the edge of the node.
        if (process_list[active_vertex] != process_list[csr[h_offset[active_vertex]+current_edges[active_vertex]]]) {
            send_to_edge.push_back ({process_list[csr[h_offset[active_vertex]]], excess[csr[h_offset[active_vertex]+current_edges[active_vertex]]], csr[h_offset[active_vertex]+current_edges[active_vertex]], get_global(csr[h_offset[active_vertex]+current_edges[active_vertex]])}) ;
        }

        if (excess[csr[h_offset[active_vertex] + current_edges[active_vertex]]] > 0 && process_list[csr[h_offset[active_vertex]]] == rank)
        {
            active_vertices.push(csr[h_offset[active_vertex] + current_edges[active_vertex]]);
        }

        if (excess[active_vertex] > 0 and heights[active_vertex] < 999999999)
        {
            active_vertices.push(active_vertex);
            return false;
        }

        return true;
    }


    bool reset_current_edge(int active_vertex)
    {

        // search for edges leading to vertices with height exactly lesser by 1.

        int success = false;
        int pointer = 0;
        for (int v = h_offset[active_vertex] + current_edges[active_vertex] + 1; v < h_offset[active_vertex + 1]; v++)
        {

            // check whether v_edge satisfies the property.

            // cerr << heights[active_vertex] << " " << heights[csr[v]];
            assert (v < num_edges ()) ;
            if (heights[csr[v]] == heights[active_vertex] - 1)
            {
                current_edges[active_vertex] = v;
                success = true;
                return true;
            }
        }

        // if control reaches here, relabelling is needed .
        if (!success)
        {
            return relabel(active_vertex);
        }

        return false;
    }

    void push_and_relabel()
    {

        cerr << " preflow push relable \n";
        // select an active vertex.
        int current_vertex = active_vertices.front();

        active_vertices.pop();

        while (push(current_vertex)) ;

        if (active_vertices.size() == 0)
        {

            ACTIVE_VERTEX_EXISTS = false;
        }
    }


    void synchronize_excess () {


        for (auto this_vertex:send_to_edge) {

            int this_excess = 0 ;
            int total_excess = 0 ;
            MPI_Group new_group ;
            MPI_Group old_group ;
            MPI_Comm new_comm ;
            vector<int> relevant_processes ;
            int *sub_processes ;

            if (exists_here( this_vertex[3])){
                this_excess = this_vertex[1] ;
                relevant_processes.push_back (this_vertex[0]) ;
            }

            copy (relevant_processes.begin (), relevant_processes.end (), sub_processes) ;
            MPI_Comm_group (MPI_COMM_WORLD, &old_group) ;
            MPI_Group_incl (old_group, relevant_processes.size (), sub_processes, &new_group) ;
            MPI_Comm_create (MPI_COMM_WORLD, new_group, &new_comm) ;
            
            MPI_Allreduce (&this_excess, &total_excess, 1, MPI_INT, MPI_SUM, new_comm) ;
            if (exists_here (this_vertex[3])) {
                excess[this_vertex[2]] = total_excess ;
            }
            MPI_Comm_free (&new_comm) ;
        }  
    }


    void print_result()
    {

        cout << "results ==========================================\n";
        cout << "max_flow = " << excess[sink] - excess[source] << endl;
        print_arr(this->flow, num_edges());
        print_arr(this->residual_flow, num_edges());
        print_arr(this->excess, num_nodes());
        print_arr(this->heights, num_nodes());
        cout << endl;
        for (int u = 0; u < num_nodes(); u++)
        {

            for (int v_index = h_offset[u]; v_index < h_offset[u + 1]; v_index++)
            {

                cout << "flow from " << u << " to " << csr[v_index] << " = " << flow[v_index] << endl;
            }
        }

        cout << "====================================================\n";
    }
};
#endif
