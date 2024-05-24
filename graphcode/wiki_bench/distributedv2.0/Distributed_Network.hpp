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
#include "synchronize_p2p.hpp"
#include "perform_updates.hpp"
//#include "distributed_bfs.hpp"
#include <mpi.h>
#include <set>
#include <unordered_set>
#include <unordered_map>


class Distributed_Network {

    /*
     * Distributed Network class.
     * Contains methods relevant for operations in networks.
     * TODO : Change the synchronization to RMA/Probabilistic.  
     * Change design to keep private and public separate.
     */
    public:
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
        // std::queue<int> active_vertices ;
        std::unordered_set<int> active_vertices ;
     	int active_nodes ;
        std::vector<std::vector<int> > altered_heights ;
        std::vector<std::vector<int> > syncer_active ;
        std::vector<std::vector<int> > syncer_excess ;
        std::vector<int> _count ;
        std::unordered_set<int> relabel_later ;
	    int num_times ;
        int awakening ;
        int pointer_1 ;
        int pointer_2 ;
        unordered_map<int,int> syncer_count_map ;
        unordered_set<int> next_pulse_idxs ;
    
        void update_buffer (std::vector<int*> &updates, int* buffer, unordered_map <int,int> &global_to_local) ;

        void max_distributed_flow_init (const int &my_rank, const int &source, const int &sink, const int &local_vertices, const int &global_vertices, MPI_Win heights, MPI_Win excess, MPI_Win CAND, MPI_Win work_list, int *heights_buffer, int *excess_buffer, int *cand_buffer, int *work_list_buffer) ;

        void update_gap (const int &obliterate, const int &current_gap) ;

        void fix_gaps () ;

        Distributed_Network (char* file_name, const int &source, const int &sink, const int &my_rank, const int &size)  {
            /* Constructor method. Reads the partitions and forms global and local views of the graph/network's initial state.
             * Constructs the residual graph and 
            */
            
	        num_times = 0 ;
            this->source = source ;
            this->sink = sink ;
            
            FILE* graph_file = fileIO (file_name) ; 
            input (graph_file) ; 
            
            this->p_no = my_rank ;

            int local_vertices = v_id ;

            MPI_Allreduce (&g_vx, &global_vertices, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
            global_vertices++ ;
            MPI_Allreduce (&e, &global_edges, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;
    	    log_sos ("Total vertices = " + to_string (global_vertices) ) ;


            heights_buffer = (int*)malloc (sizeof(int)*local_vertices) ;
            excess_buffer = (int*)malloc (sizeof(int)*local_vertices) ;
            CAND_buffer = (int*)malloc (sizeof(int)*local_vertices+1) ;
            work_list_buffer = (int*)malloc (sizeof(int)*local_vertices) ;

            memset (heights_buffer, 0, sizeof(int)*local_vertices) ;
            memset (excess_buffer, 0, sizeof(int)*local_vertices) ;
            memset (CAND_buffer, 0, sizeof(int)*local_vertices) ;
            memset (work_list_buffer, 0, sizeof(int)*local_vertices) ;

            MPI_Barrier (MPI_COMM_WORLD) ;

            // MPI_Win_create (heights_buffer, (sizeof(int)*local_vertices), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &heights) ;
            // MPI_Win_create (excess_buffer, (sizeof(int)*local_vertices), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &excess) ;
            // MPI_Win_create (CAND_buffer, (sizeof(int)*local_vertices), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &CAND) ;
            // MPI_Win_create (work_list_buffer, (sizeof(int)*local_vertices), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &work_list) ;

            max_distributed_flow_init (my_rank, source, sink, v_id, global_vertices, heights, excess, CAND, work_list, heights_buffer, excess_buffer, CAND_buffer, work_list_buffer) ;
            log_message ("Initialization OK ")  ;
            MPI_Barrier (MPI_COMM_WORLD) ;

            /*for (auto &it:native_vertex) {
                log_message (to_string (it) + " belongs to process " + to_string (p_no)) ;
            }*/
            MPI_Barrier (MPI_COMM_WORLD) ;

            /*for (int i=0; i<v_id; i++) {
                for (auto &it : residual_graph[i]) {
                    log_message ("edge from " + to_string (local_to_global[i]) + " to " + to_string (local_to_global[it[0]]) + " of capacity " + to_string (it[1]) + " at process " + to_string (p_no)) ;
                }
            }*/
              /*for (int i=0; i< v_id; i++) {
                if (excess_buffer[i] > 0) {
                    log_message ("red alert index = " + to_string (local_to_global[i]) + " value = " + to_string (excess_buffer[i]) + " to_process = " + to_string (p_no) + " at height " + to_string (heights_buffer[i])) ;
                    if (local_to_global[i] != source && local_to_global[i] != sink && (native_vertex.find (local_to_global[i]) != native_vertex.end ()) ) {
                        //active_vertices.push (i) ;
                        active_vertices.insert (i) ;
                    }
                }
            } */
            _count.assign (global_vertices + 10 ,0) ;
            _count[0]=1 ;
            _count[global_vertices]=1 ;
            pointer_1 = 0 ;
            pointer_2 = global_vertices ;
            awakening = 0 ;
            memcpy (work_list_buffer, heights_buffer, v_id*sizeof(int)) ;
            log_message (to_string (p_no) + " has " + to_string (active_vertices.size ())) ;
            MPI_Barrier (MPI_COMM_WORLD) ;
        }

        ~Distributed_Network () {
            free (heights_buffer) ;
            free (excess_buffer) ;
            free (CAND_buffer) ;
            free (work_list_buffer) ;
        }

        bool push_distributed (const int &u, const int &v) ;
        bool relabel (const int &u) ;
        std::vector<int>  discharge (const int &u) ;

        void sync_all_heights () {
            return ;
        }

	    void print_ans () {
	        if (native_vertex.find (sink) != native_vertex.end ()) {
			cout << excess_buffer[global_to_local[sink]] << endl ;
	        }
	    }

        int local_max_flow () {
            
            // start pulse.
            int did_work = 0 ;
            next_pulse_idxs.clear () ;
            MPI_Barrier (MPI_COMM_WORLD) ;
    	    altered_heights.clear () ;
            awakening++ ;

            while (1) {

                // clear all synchronization buffers.
		        syncer_excess.clear () ;
		        syncer_active.clear () ;

                // extract u.
                //int u = (!active_vertices.empty ()) ? active_vertices.front () : -1 ;
                int u = (!active_vertices.empty () && (next_pulse_idxs.find (u) == next_pulse_idxs.end ())) ? *active_vertices.begin () : -1 ;
		        if (!active_vertices.empty()) active_vertices.erase(u) ;
		        log_message ("popped " + to_string (p_no)) ;
                /*std::vector<int> al_heights = discharge (u) ;
                if (!al_heights.empty ()) {altered_heights.push_back (al_heights) ;}*/

                // discharge u
                discharge (u) ;
                log_message (to_string (p_no) + " exited dischaege OK") ;
                MPI_Barrier (MPI_COMM_WORLD) ;

                // synchronize the result of all pushes.
                std::vector<int*> update_residual_vector = synchronize (syncer_excess, 3) ;
                // synchronize_excess () ;
                MPI_Barrier (MPI_COMM_WORLD) ;

                // update the result of all pushes.
                update_residual (update_residual_vector, residual_graph, excess_buffer, active_vertices, source, sink, global_to_local) ;

                
		        MPI_Barrier (MPI_COMM_WORLD) ;

                // push all valid active vertices into queue.
	            /*for (int i=0; i<v_id; i++) {
		            if (excess_buffer[i]>0 && (next_pulse_idxs.find (i) == next_pulse_idxs.end ())) {
                        if (local_to_global[i] != source && local_to_global[i] != sink && (native_vertex.find (local_to_global[i]) != native_vertex.end ()) && heights_buffer[i] < global_vertices-1) {
                            log_message ("pushing " + to_string (local_to_global[i]) + " to queue of process " + to_string (p_no) + " at time " + to_string (num_times) ) ;
                        //active_vertices.push (i) ;
                            //active_vertices.insert (i) ;
                        }
		            }
                } */

                // Find how many valid active vertices are there.
                int locally_active = active_vertices.size () ;
                MPI_Allreduce (&locally_active, &active_nodes, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;
                log_message ("pulse over, active nodes = " + to_string (active_nodes)) ;
                
                MPI_Barrier (MPI_COMM_WORLD) ;

                // if there are no more, stop.
                if (active_nodes == 0) break ;

                /*int flag = (u==-1)?1:0 ;
                int decision ;
                MPI_Allreduce (&flag,&decision, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;
                MPI_Barrier (MPI_COMM_WORLD) ;
                if (decision >= 4) break ;*/
            }

            // end pulse.
            MPI_Barrier (MPI_COMM_WORLD) ;

/*            for (auto &it:update_heights_vector) {
                log_message ("heights : " + to_string (it[0]) + " "+ to_string (it[1])) ;
            }*/
            log_message (to_string (p_no) + " has " +to_string (active_vertices.size ()) + " elements in its active vertices queue ") ;

            // enter relabel phase.
            for (auto &u:relabel_later) {
                if (relabel (u)) {
                    altered_heights.push_back ({p_no, -1, local_to_global[u], heights_buffer[u]}) ;
                }
            }
            relabel_later.clear () ;
            MPI_Barrier (MPI_COMM_WORLD) ;

            // push all successfully relabeled vertices out for communication.
            std::vector<int*> update_heights_vector = synchronize (altered_heights, 2) ;
	        update_buffer (update_heights_vector, heights_buffer, global_to_local) ;
            MPI_Barrier (MPI_COMM_WORLD) ;

            // allow vertices for next round in.
            // swap (active_vertices, next_pulse_idxs) ;
            for (auto &i:next_pulse_idxs) {
                if (excess_buffer[i]>0&& local_to_global[i] != source && local_to_global[i] != sink && (native_vertex.find (local_to_global[i]) != native_vertex.end ()) && heights_buffer[i] < global_vertices-1)
                    active_vertices.insert(i) ;
            }
            next_pulse_idxs.clear () ;

            // push all viable vertices in.
	        /*for (int i=0; i<v_id; i++) {
		        if (excess_buffer[i]>0) {
                    log_message ("red alert index = " + to_string (local_to_global[i]) + " value = " + to_string (excess_buffer[i]) + " to_process = " + to_string (p_no) + " at height = " + to_string (heights_buffer[i]) ) ;
                    if (local_to_global[i] != source && local_to_global[i] != sink && (native_vertex.find (local_to_global[i]) != native_vertex.end ()) && heights_buffer[i] < global_vertices-1) {
                        log_message ("pushing " + to_string (local_to_global[i]) + " to queue of process " + to_string (p_no)) ;
                        //active_vertices.push (i) ;
                        //active_vertices.insert (i) ;
                    }
		        }
	        }*/
	        log_message ("we are returning " ) ;
            MPI_Barrier (MPI_COMM_WORLD) ;

            // apply gap relabeling heuristic.
            if (awakening == 100) {fix_gaps () ; awakening=0 ; }
            MPI_Barrier (MPI_COMM_WORLD) ;

            // if there are no new active vertices in next pulse either, stop it all.
            int locally_active = active_vertices.size () ;
            MPI_Allreduce (&locally_active, &active_nodes, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;
            log_message ("pulse over, active nodes = " + to_string (active_nodes)) ;
            
            MPI_Barrier (MPI_COMM_WORLD) ;

            num_times++ ;


/*            for (int i=0 ;i<v_id; i++) {
                if (heights_buffer[i] > 0 ) {
                    log_message ("heights updated or not at " + to_string (heights_buffer[i])+ " at process " + to_string (p_no) + " globally " + to_string (local_to_global[i])) ;
                }
            }*/

            // The stopping condition should still work.
            return active_nodes ;
    }
} ;


void Distributed_Network::fix_gaps () {
    /* For Gap relabeling. The count data structure needs to be kept up to date in the relabel function.*/
    
    // dessiminate the map.
    vector<vector<int> > syncer_count_messages ;
    for (auto &count_message:syncer_count_map) {
        if (count_message.second != 0)
        syncer_count_messages.push_back ({p_no, -1, count_message.first, count_message.second}) ; 
    }
    syncer_count_map.clear () ;

    vector<int*> update_count_requests = synchronize (syncer_count_messages, 2) ;
    update_buffer_n (update_count_requests, _count) ;

    int global_pointer_1 ;
    MPI_Allreduce (&pointer_1, &global_pointer_1, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
 //   log_sos ("global pointer 1 " + to_string (global_pointer_1)) ;

    for (pointer_2 = 0; pointer_2<global_pointer_1 ; pointer_2++) if (!_count[pointer_2]) break ;
//    log_sos ("pointer_2 = " + to_string (pointer_2)) ;
    int send_buffer = global_vertices+11;
    //if (!_count[pointer_2]) send_buffer = pointer_2 ;
    if (pointer_2 < global_pointer_1) send_buffer = pointer_2 ;
    int gap_pointer_2 ;
    MPI_Allreduce (&send_buffer, &gap_pointer_2, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD) ;
//    log_sos ("gap pointer 2" + to_string (gap_pointer_2)) ;

    bool kill = false ;
    if (gap_pointer_2 >= global_vertices-1) kill = true ;

    bool kill_global ;
    MPI_Allreduce (&kill, &kill_global, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD) ;

    MPI_Barrier (MPI_COMM_WORLD) ;
    if (kill_global) return ;

 //   log_sos ("gap = " +to_string(gap_pointer_2) + "max = " + to_string (global_pointer_1) ) ;

    for (int i=0 ; i < v_id; i++) {
        if (local_to_global[i] == sink or local_to_global[i] == source) continue ;
        int it = heights_buffer[i] ;
        if (it > gap_pointer_2) {
            heights_buffer[i]=global_vertices ;
            //log_sos ("Will fix " + to_string (local_to_global[i]) + " will relabel it to " + to_string (heights_buffer[i])) ;
        }
    }
}


void Distributed_Network::update_gap (const int &obliterate, const int &current_gap) {
    
            
    _count[obliterate]-- ;
    _count[current_gap]++ ;
    if (syncer_count_map.find (obliterate) == syncer_count_map.end ()) syncer_count_map[obliterate]=-1 ;
    else syncer_count_map[obliterate]-- ;
    if (syncer_count_map.find (current_gap) == syncer_count_map.end ()) syncer_count_map[current_gap]=1 ;
    else syncer_count_map[current_gap]++ ;

    pointer_1 = max (pointer_1, current_gap) ;

    if (obliterate == pointer_1 && count[obliterate] == 0) {
        for (; pointer_1 >= 0; pointer_1--) {
            if (count[pointer_1]) break ;
        }
    }
}


void Distributed_Network::max_distributed_flow_init (const int &my_rank, const int &source, const int &sink, const int &local_vertices, const int &global_vertices, MPI_Win heights, MPI_Win excess, MPI_Win CAND, MPI_Win work_list, int *heights_buffer, int *excess_buffer, int *cand_buffer, int *work_list_buffer) {

    // declare the arrays.


    MPI_Barrier (MPI_COMM_WORLD) ;
    std::vector<std::vector<int> > syncer, syncer_queue ;
    for (int i=0; i<local_vertices; i++) {
        if (heights_buffer[i] > 0) {
            log_message (to_string (local_to_global[i]) + " has height " + to_string (heights_buffer[i]) + " at process " + to_string (p_no) + " source = " + to_string (source)) ;
        }
    }


    if (global_to_local.find (source) != global_to_local.end ()) {
        assert (local_to_global[global_to_local[source]] == source ) ;
    	int x = global_to_local[source] ;
    	heights_buffer[x] = global_vertices ;
    } 

    for (int i=0; i<local_vertices; i++) {
        if (heights_buffer[i] > 0) {
            log_message (to_string (local_to_global[i]) + " has height " + to_string (heights_buffer[i]) + " at process " + to_string (p_no) + " source = " + to_string (source)) ;
        }
    }
    if (native_vertex.find (source) != native_vertex.end ()) {
        // log_message ("source is inside " + to_string (my_rank)) ;
        int u = global_to_local[source] ;
        for (int v_idx = 0 ; v_idx < residual_graph[u].size (); v_idx++ ){
            
            int v = residual_graph[u][v_idx][0] ;
            int to_proc = residual_graph[u][v_idx][4] ;
             log_message ("sending to : " + to_string (local_to_global[v]) + " total = " + to_string (local_vertices) + " native process = " + to_string (to_proc) + " at height " + to_string (heights_buffer[v])) ;
            if (to_proc == p_no) {
                residual_graph[v][residual_graph[u][v_idx][2]][1] += residual_graph[u][v_idx][1] ;
                int this_capacity = 0; 
                this_capacity += residual_graph[u][v_idx][1] ;
	    	    excess_buffer[v] += this_capacity ;
                residual_graph[u][v_idx][1] = 0 ;
                if (excess_buffer[v]>0 && local_to_global[v] != sink && local_to_global[v] != source && (next_pulse_idxs.find(v) == next_pulse_idxs.end())) active_vertices.insert (v) ;
            } else {

                residual_graph[v][residual_graph[u][v_idx][2]][1] += residual_graph[u][v_idx][1] ;
                int this_capacity = 0; 
                this_capacity += residual_graph[u][v_idx][1] ;
	    	    excess_buffer[v] += this_capacity ;
                residual_graph[u][v_idx][1] = 0 ;
                log_message ("here updating " + to_string (local_to_global[v]) + "to = " + to_string (excess_buffer[v])) ;
                if (excess_buffer[v]>0 && local_to_global[v] != sink && local_to_global[v] != source && (next_pulse_idxs.find(v) == next_pulse_idxs.end())) active_vertices.insert (v) ;
                syncer.push_back ({p_no, to_proc, source, local_to_global[residual_graph[u][v_idx][0]], this_capacity}) ;
            }
        }
    }


    MPI_Barrier (MPI_COMM_WORLD) ;

    log_message ("proceeding to sync") ;
    std::vector<int*> update_residual_vector = synchronize (syncer, 3) ;
    /*for (auto &it:update_residual_vector) {
        log_message (to_string (it[0]) + " : " + to_string (it[1]) + " : " + to_string (it[2])) ;
    }*/
    MPI_Barrier (MPI_COMM_WORLD) ;

    update_residual (update_residual_vector, residual_graph, excess_buffer, active_vertices, source, sink, global_to_local) ;
    synchronize_excess () ;

    MPI_Barrier (MPI_COMM_WORLD) ;
	        for (int i=0; i<local_vertices; i++) {
		        if (excess_buffer[i]>0) {
                    log_message ("right after red alert index = " + to_string (local_to_global[i]) + " value = " + to_string (excess_buffer[i]) + " to_process = " + to_string (p_no) + " at height = " + to_string (heights_buffer[i]) ) ;
		        }
	        }
    MPI_Barrier (MPI_COMM_WORLD) ;

    //distributed_bfs (heights_buffer, global_vertices, local_vertices, sink, source, my_rank) ;
}


bool Distributed_Network::push_distributed (const int &u, const int &v) {

    assert (excess_buffer[u] > 0 && (heights_buffer[u] >= heights_buffer[v] + 1) ); 
    ll delta = min ((ll)excess_buffer[u], (ll)residual_graph[u][CAND_buffer[u]][1]) ;
    int to_proc = residual_graph[u][CAND_buffer[u]][4] ;
    log_message ("pushing flow from " + to_string(local_to_global[u]) + " at " + to_string (heights_buffer[u]) +  " to " + to_string (local_to_global[v]) +" at height " + to_string (heights_buffer[v]) +" of val " + to_string (delta) + " to process " + to_string (to_proc)) ;
    // log_message_push ("heights : " + to_string (heights[u]) + " and " + to_string(heights[v])) ;

    if (native_vertex.find (local_to_global[v]) != native_vertex.end ()) {
        residual_graph[u][CAND_buffer[u]][1]-=delta ;
        residual_graph[v][residual_graph[u][CAND_buffer[u]][2]][1]+=delta ;
        excess_buffer[v]+=delta ;
        excess_buffer[u]-=delta ;
        if (excess_buffer[v]>0 && local_to_global[v] != sink && local_to_global[v] != source && (next_pulse_idxs.find(v)==next_pulse_idxs.end())) active_vertices.insert (v) ;
    } else {
        residual_graph[u][CAND_buffer[u]][1]-=delta ;
        residual_graph[v][residual_graph[u][CAND_buffer[u]][2]][1]+=delta ;
        excess_buffer[v]+=delta ;
        excess_buffer[u]-=delta ;
        log_message ("pushing into syncer" ) ;
        log_message ("pushed this : " + to_string (p_no) + " : " + to_string (to_proc) + " : " + to_string (local_to_global[u]) + " : " + to_string (local_to_global[v]) + " : " + to_string (delta)) ;
        if (excess_buffer[v]>0 && local_to_global[v] != sink && local_to_global[v] != source && (next_pulse_idxs.find(v)==next_pulse_idxs.end())) active_vertices.insert (v) ;
        syncer_excess.push_back ({p_no, to_proc, local_to_global[u], local_to_global[v], delta}) ;
    }

    return true ;
}


bool Distributed_Network::relabel (const int &u) {

    // log_sos ("relabeling " + to_string (u) + " at height " + to_string (heights_buffer[u])) ;
    
    if (heights_buffer[u] >= global_vertices) return false ;

    assert (excess_buffer[u]>0 ) ;
    ll minim = INT_MAX-10 ;
    for (int v=0; v<residual_graph[u].size (); v++) {
        log_message ("considering " + to_string (local_to_global[residual_graph[u][v][0]]) + " with capacity = " + to_string (residual_graph[u][v][1]) + " at height " + to_string (heights_buffer [residual_graph[u][v][0]])) ;
        if (residual_graph[u][v][1]>0 ) {
            log_message ("heights buffer " + to_string (u) + " = " + to_string (heights_buffer[u])) ;
            assert (heights_buffer[u] <= heights_buffer[residual_graph[u][v][0]] ) ;
            minim = min (minim, (ll)heights_buffer[residual_graph[u][v][0]]) ;
        }
    }

    int obliterate = heights_buffer[u] ;
    heights_buffer[u] = min (minim + 1, (ll)global_vertices) ;
     CAND_buffer[u]=0 ;

    update_gap (obliterate, heights_buffer[u]) ;
    log_message ("relabeled" + to_string (local_to_global[u]) + " at height " + to_string (heights_buffer[u])) ;
  
    return true ;
}

std::vector<int>  Distributed_Network::discharge (const int &u) {

  
    if (u == -1) return {} ;
    if (native_vertex.find (local_to_global[u]) == native_vertex.end ()) return {} ;

    std::vector<int > ans ;
    log_message ("discharging : " + to_string (local_to_global[u]) + " from process " + to_string (p_no)+ " having height " + to_string (heights_buffer[u])) ; log_message ("having excess = " + to_string (excess_buffer[u])) ;
    while (excess_buffer[u]>0) {
        
        // log_sos (to_string (active.size ()) + " " + to_string(active.front ()) + " " + to_string (excess[active.front ()])) ;
        
        if (CAND_buffer[u] < residual_graph[u].size ()) {
//            log_message ("OK" + to_string(p_no)) ;
            int v = residual_graph[u][CAND_buffer[u]][0];
            if (residual_graph[u][CAND_buffer[u]][1] > 0 && heights_buffer[u] >= heights_buffer[v] + 1)
                push_distributed(u, v);
            else 
                CAND_buffer[u]++;
        } else {
 //           log_message ("OK" + to_string(p_no)) ;
            log_message ("reserving for next pulse " + to_string (local_to_global[u]) + " at time " + to_string (num_times) );
            relabel_later.insert (u) ;
            next_pulse_idxs.insert (u) ;
            /*
            bool status = relabel(u);
            log_message ("OK" + to_string(p_no)) ;


            if (status == false)  {next_pulse_idxs.insert(u); return {} ;} 
            else {
                int from_process = p_no ;
                int offset = local_to_global[u] ;
                int value = heights_buffer[u] ;
                CAND_buffer[u] = 0;
                next_pulse_idxs.insert (u) ;
                log_message ("returning meaningful stuff") ;
                return {from_process, -1, offset, value} ;
             }*/
            return {} ;
        }
    }
    return {} ;
}

void Distributed_Network::update_buffer (std::vector<int*> &updates, int* buffer, unordered_map <int,int> &global_to_local) {

    for (auto &update:updates) {

        int global_offset = update[0] ;
        int new_val = update[1] ;

        log_message ("updating height to : " + to_string (new_val) + " of : " + to_string (global_offset)) ;

        if (global_to_local.find (global_offset) != global_to_local.end ()) {
            
            int local_offset = global_to_local[global_offset] ;
            int obliterate = buffer[local_offset] ;
            buffer[global_to_local[global_offset]] = new_val ; 
            update_gap (obliterate, new_val) ;
            
        }
	free (update-1) ;
	update=NULL ;
    }
} 
#endif
