#ifndef __BFS__
#define __BFS__

#include "synchronize_p2p.hpp"
#include "perform_updates.hpp"
#include "serial_methods.hpp"
#include <queue>
#include <utility>


void distributed_bfs (int *heights, const int& global_vertices, const int& local_vertices, const int& sink, const int& source, const int& p_no) {

    log_message ("distributed bfs 1.0" ) ;

    // initialize 
    int* visited_arr = (int*) malloc (local_vertices*sizeof(int)) ;
    if (visited_arr == NULL) {
        printf ("visited array memory not allocated") ;
        MPI_Abort (MPI_COMM_WORLD, MPI_ERR_OP) ;
    }

    for (int i=0; i<local_vertices; i++) {
        heights[i]=global_vertices ;
    }

    log_message ("size of local array = " + to_string (local_vertices)) ;
    log_message ("succesful allocation for visited array") ;
    memset (visited_arr, 0, local_vertices*sizeof(int)) ;
    log_message ("succesful definition for visited array") ;

    std::queue<pair<int,int> > frontier ;
    std::vector<std::vector<int> > syncer_queue, syncer_heights ;

    if (native_vertex.find (sink) != native_vertex.end ()) {
        frontier.push (make_pair (global_to_local[sink], 0)) ;
    }

    log_message ("sink pushed inside") ;

    log_message ("Initialized bfs OK") ;
    MPI_Barrier (MPI_COMM_WORLD) ;

    // start the breadth first search.
    while (1) {

        syncer_queue.clear () ;
        // start of one phase
        int u = -1 , height ;
        if (!frontier.empty ()) {
            tie (u, height) = frontier.front (); 
            frontier.pop () ;
            heights[u] = height ;
            syncer_heights.push_back ({p_no, -1, local_to_global[u], height}) ;
        }
        
        // if (u == -1 || visited_arr[i] == 1) goto synchronizations ;
        if (!(u == -1 || visited_arr[u] == 1)) {
        
            for (auto& vertex_v : residual_graph[u]) {

                int v = vertex_v[0] ;
                int rem_cap = vertex_v[1] ;
                int offset = vertex_v[2] ;
                int direction = vertex_v[3] ;
                int to_proc = vertex_v[4] ;

                if (!visited_arr[v] && rem_cap == 0) {
                    if (to_proc != p_no) {
                        syncer_queue.push_back ({p_no, to_proc, local_to_global[v], height+1}) ;
                    } else {
                        frontier.push (make_pair (v, height+1)) ;
                    }
                }
            } 
        }
        if (u != -1) visited_arr[u]=1 ;
        // end of pulse 
	

        // synchronization phase 
        // synchronizations:
            MPI_Barrier (MPI_COMM_WORLD) ;
            std::vector<int*> update_queue = synchronize (syncer_queue, 2) ;
            std::vector<int*> update_height = synchronize (syncer_heights, 2) ;
            bfs_queue_updater (update_queue, frontier, visited_arr) ;
            update_buffer_1 (update_height, heights) ;
            syncer_queue.clear () ;
//            clear_up_updater (update_queue) ;
            MPI_Barrier (MPI_COMM_WORLD) ;

            int local_queue_size = frontier.size () ;
            int global_queue_size ;

            MPI_Allreduce (&local_queue_size, &global_queue_size, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;

            MPI_Barrier (MPI_COMM_WORLD) ;

            // check whether all should stop.
            if (global_queue_size == 0) {
                break ;
            }
	    log_message ("not stopping cause global queue size = " + to_string (global_queue_size)) ;
            
            MPI_Barrier (MPI_COMM_WORLD) ;
    }

    if (native_vertex.find (source) != native_vertex.end()) {
        heights[global_to_local[source]]=global_vertices ;
        syncer_heights.push_back ({p_no, -1, source, global_vertices}) ;
    }

    std::vector<int*> update_height = synchronize (syncer_heights, 2) ;
    update_buffer_1 (update_height, heights) ;

    MPI_Barrier (MPI_COMM_WORLD) ;

    for (int i=0; i<local_vertices; i++) {
        log_message ("bfs height of " + to_string (local_to_global[i]) + " = " +to_string (heights[i])) ;
    }
}

#endif
