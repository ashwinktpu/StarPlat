/* TO  DO :
 * 1. implement the logic to place an active node into another process's worklist.
 * 2. Test the constructor.    
*/

/* Concepts :
 * 1. Pulse : for every vertex, apply push/relabel steps until e(v) = 0 or d(v) increases.
 * 2. Relabel : Compute new label based on heights already present.
 * 3. 
*/

#ifndef __DISTRIBUTED_NETWORK__ 

#define __DISTRIBUTED_NETWORK__

#include "serial_methods.hpp"
// #include "distributed_methods.hpp"
// #include "max_flow_init_distributed_2_sided.hpp"
// #include "discharge.hpp"
#include "synchronizer.hpp"
#include <mpi.h>


class Distributed_Network {

    private:
        int source ;
        int sink ;
        int global_vertices ;
        int global_edges ;
        MPI_Win heights ;
        MPI_Win excess ;
        MPI_Win CAND ;
        MPI_Win work_list ;
        int *heights_buffer, *excess_buffer, *CAND_buffer, *work_list_buffer ;
        int front, back ;
        int p_no ;
        vvi modified_heights_sync_list ;
        queue<int> active_vertices ;
        int active_nodes ;
    
    public:

        void max_distributed_flow_init (const int &my_rank, const int &source, const int &sink, const int &local_vertices, const int &global_vertices, MPI_Win heights, MPI_Win excess, MPI_Win CAND, MPI_Win work_list, int *heights_buffer, int *excess_buffer, int *cand_buffer, int *work_list_buffer) ;


        Distributed_Network (char* file_name, const int &source, const int &sink, const int &my_rank, const int &size)  {
            
            
            FILE* graph_file = fileIO (file_name) ; 
            input (graph_file) ; 
            
            this->p_no = my_rank ;

            int local_vertices = v_id ;

            MPI_Allreduce (&g_vx, &global_vertices, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
            global_vertices++ ;
            MPI_Allreduce (&e, &global_edges, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;
            heights_buffer = (int*)malloc (sizeof(int)*local_vertices) ;
            excess_buffer = (int*)malloc (sizeof(int)*local_vertices) ;
            CAND_buffer = (int*)malloc (sizeof(int)*local_vertices) ;
            work_list_buffer = (int*)malloc (sizeof(int)*local_vertices) ;

            memset (heights_buffer, 0, sizeof(int)*local_vertices) ;
            memset (excess_buffer, 0, sizeof(int)*local_vertices) ;
            memset (CAND_buffer, 0, sizeof(int)*local_vertices) ;
            memset (work_list_buffer, 0, sizeof(int)*local_vertices) ;

            MPI_Barrier (MPI_COMM_WORLD) ;

            max_distributed_flow_init (my_rank, source, sink, v_id, global_vertices, heights, excess, CAND, work_list, heights_buffer, excess_buffer, CAND_buffer, work_list_buffer) ;
            log_message ("Initialization OK ")  ;
            MPI_Barrier (MPI_COMM_WORLD) ;
              for (int i=0; i< v_id; i++) {
                if (excess_buffer[i] > 0) {
                    log_message ("red alert " + to_string (i) + " " + to_string (excess_buffer[i])) ;
                }
            }
            active_nodes = 1 ;
        }

        ~Distributed_Network () {
            MPI_Win_free (&heights) ;
            MPI_Win_free (&excess) ;
            MPI_Win_free (&work_list) ;
            MPI_Win_free (&CAND) ;
        }

        bool push_distributed (const int &u, const int &v) ;
        bool relabel (const int &u) ;
        std::vector<int>  discharge (const int &u) ;

        void sync_all_heights () {

            return ;
        }

        int local_max_flow () {
            
            int did_work = 0 ;
            std::vector<std::vector<int> > altered_heights ;
            // start pulse.
            MPI_Barrier (MPI_COMM_WORLD) ;

            while (!active_vertices.empty ()) {
                altered_heights.push_back (discharge (active_vertices.front())) ;
                active_vertices.pop () ;
            }

            // end pulse.
            MPI_Barrier (MPI_COMM_WORLD) ;

            // synchronize the heights.
            if (altered_heights.size ())
                synchronize (altered_heights, heights_buffer, v_id) ;
            int locally_active = active_vertices.size () ;
            MPI_Allreduce (&active_nodes, &locally_active, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;
            // log_message ("pulse over, active nodes = " + to_string (active_nodes)) ;
            MPI_Barrier (MPI_COMM_WORLD) ;

            return active_nodes ;
        }
} ;



void Distributed_Network::max_distributed_flow_init (const int &my_rank, const int &source, const int &sink, const int &local_vertices, const int &global_vertices, MPI_Win heights, MPI_Win excess, MPI_Win CAND, MPI_Win work_list, int *heights_buffer, int *excess_buffer, int *cand_buffer, int *work_list_buffer) {

    // declare the arrays.


    MPI_Barrier (MPI_COMM_WORLD) ;
    std::vector<std::vector<int> > syncer, syncer_queue ;

    log_message ("mem init OK") ;

    if (native_vertex.find (source) != native_vertex.end ()) {
        log_message ("source is inside " + to_string (my_rank)) ;
        for (int v_idx = 0 ; v_idx < residual_graph[source].size (); v_idx++ ){
            
            int v = residual_graph[src][v_idx][0] ;
            log_message ("size comp : " + to_string (v) + " total = " + to_string (local_vertices)) ;
            residual_graph[v][residual_graph[src][v_idx][2]][1] += residual_graph[src][v_idx][1] ;
            int this_capacity = excess_buffer[v]; 
            this_capacity += residual_graph[src][v_idx][1] ;
            int to_proc = residual_graph[v][residual_graph[src][cand_buffer[src]][2]][4] ;
            
            if (to_proc == p_no) {
                
                excess_buffer[v] += this_capacity ;
            } else {
                syncer.push_back ({my_rank, residual_graph[src][v_idx][4], local_to_global[v], this_capacity}) ;
            }

            residual_graph[src][v_idx][1] = 0 ;
            if (this_capacity > 0 && v!= src && v!=snk) {
                if (native_vertex.find (v) != native_vertex.end ()) {
                    active_vertices.push (v) ;
                } else {
                    
                    syncer_queue.push_back ({p_no, to_proc, local_to_global[v]}) ;
                }
            }
        }
    }

    MPI_Barrier (MPI_COMM_WORLD) ;

    log_message ("proceeding to sync") ;
    synchronize (syncer, excess_buffer, local_vertices) ;
    synchronize_queue (syncer, active_vertices) ;

    MPI_Barrier (MPI_COMM_WORLD) ;

    for (int i=0; i< v_id; i++) {
                if (excess_buffer[i] > 0) {
                    log_message ("red alert " + to_string (i) + " " + to_string (excess_buffer[i])) ;
                }
            }
    MPI_Barrier (MPI_COMM_WORLD) ;
}

bool Distributed_Network::push_distributed (const int &u, const int &v) {

    std::vector<std::vector<ll> > syncer ;

    assert (excess_buffer[u] > 0 && (heights_buffer[u] >= heights_buffer[v] + 1) ); 
    ll delta = min ((ll)excess_buffer[u], residual_graph[u][current_arc[u]][1]) ;
    // log_message_push ("pushing flow from " + to_string(u) + " to " + to_string (v) + " of val " + to_string (delta)) ;
    // log_message_push ("heights : " + to_string (heights[u]) + " and " + to_string(heights[v])) ;
    residual_graph[u][current_arc[u]][1]-=delta ;
    log_graph (residual_graph) ;
    residual_graph[v][residual_graph[u][current_arc[u]][2]][1]+=delta ;
    excess_buffer[v]+=delta ;
    excess_buffer[u]-=delta ;

    int to_process = residual_graph[v][residual_graph[u][CAND_buffer[u]][2]][3] ;
    if (excess_buffer[v]>0 && (v != src) && (v != snk) && excess_buffer[v] == delta) {
        log_message(  "pushing " + to_string(v) + " into active "  );
        // adding back to active queue.
        if (native_vertex.find (v) != native_vertex.end ()) {
            active_vertices.push (v) ;
        } else {
            
            int to_proc = residual_graph[v][residual_graph[u][current_arc[u]][2]][4] ;
            syncer.push_back ({p_no, to_proc, local_to_global[v]}) ;
        }
    }

    if (syncer.size ())
        synchronize_queue (syncer, active_vertices) ;
    return true ;
}


bool Distributed_Network::relabel (const int &u) {

    // log_sos ("relabeling " + to_string (u) + " at height " + to_string (heights_buffer[u])) ;
    
    if (heights_buffer[u] == g_vx) return false ;

    assert (excess_buffer[u]>0 ) ;
    ll minim = INT_MAX ;
    for (int v=0; v<residual_graph[u].size (); v++) {
        if (residual_graph[u][v][1]>0 ) {
            assert (heights_buffer[u] <= heights_buffer[residual_graph[u][v][0]]) ;
            minim = min (minim, (ll)heights_buffer[residual_graph[u][v][0]]) ;
        }
    }

    heights_buffer[u] = min (minim + 1, (ll)g_vx) ;
  
    return true ;
}

std::vector<int>  Distributed_Network::discharge (const int &u) {

  

    std::vector<int > ans ;
    log_message ("discharging : " + to_string (u)) ; log_message ("having excess = " + to_string (excess_buffer[u])) ;
    while (excess_buffer[u]>0) {
        
        // log_sos (to_string (active.size ()) + " " + to_string(active.front ()) + " " + to_string (excess[active.front ()])) ;
        
        if (CAND_buffer[u] < residual_graph[u].size ()) {
            int v = residual_graph[u][CAND_buffer[u]][0];
            if (residual_graph[u][CAND_buffer[u]][1] > 0 && heights_buffer[u] >= heights_buffer[v] + 1)
                push_distributed(u, v);
            else 
                CAND_buffer[u]++;
        } else {
            bool status = relabel(u);

            if (status == false) return {} ;
            else {
                int from_process = p_no ;
                int to_process = residual_graph[u][CAND_buffer[u]][3] ;
                int offset = local_to_global[u] ;
                int value = heights_buffer[u] ;
                CAND_buffer[u] = 0;
                return {from_process, to_process, offset, value} ;
             }
        }
    }
    return {} ;
}

#endif
