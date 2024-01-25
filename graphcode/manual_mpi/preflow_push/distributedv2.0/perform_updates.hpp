#ifndef __UPDATER__

#define __UPDATER__


#include <queue>
#include "Distributed_Network.hpp"
#include <vector>
#include "serial_methods.hpp"
#include "debug.hpp"



void clear_up_updater (std::vector<int*> &done_updates) {
    
    for (auto &update:done_updates) {
        free (update) ;
        update = NULL ;
    }

    done_updates.clear () ;
}

void bfs_queue_updater (std::vector<int*> &updater_queue, std::queue<pair<int,int> > &frontier, int* visited) {
    
    for (auto &update:updater_queue) {
        int global_v = update[0] ;
        int new_height = update[1] ;

        if (visited[global_to_local[global_v]] == 0)
            frontier.push (make_pair (global_to_local[global_v], new_height)) ;
	free (update-1) ;
	update=NULL ;
    }
}

void update_buffer_1 (std::vector<int*> &updates, int* buffer) {

    for (auto &update:updates) {

        int global_offset = update[0] ;
        int new_val = update[1] ;

//        log_message ("updating height to : " + to_string (new_val) + " of : " + to_string (global_offset)) ;

        if (global_to_local.find (global_offset) != global_to_local.end ()) {
            
            buffer[global_to_local[global_offset]] = new_val ; 
        }
	free (update-1) ;
	update=NULL ;
    }

} 

void update_buffer_n (vector<int*> update_requests, vector<int> &buffer) {
    
    for (auto &update:update_requests) {
        int idx = update[0] ;
        int delta = update[1] ;
        buffer[idx] += delta ;
        free (update-1) ;
        update=NULL ;
    }
}

/*void Distributed_Network::update_buffer (std::vector<int*> &updates, int* buffer, unordered_map <int,int> &global_to_local, Distributed_Network* network) {

    for (auto &update:updates) {

        int global_offset = update[0] ;
        int new_val = update[1] ;

        log_message ("updating height to : " + to_string (new_val) + " of : " + to_string (global_offset)) ;

        if (global_to_local.find (global_offset) != global_to_local.end ()) {
            
            int local_offset = global_to_local[global_offset] ;
            int obliterate = buffer[local_offset] ;
            buffer[global_to_local[global_offset]] = new_val ; 
            network->update_gap (obliterate, new_val) ;
            
        }
	free (update-1) ;
	update=NULL ;
    }
} */



void update_residual (std::vector<int*> &updates, vvii &residual, int* excess, unordered_set<int> &active_vertices, const int &source, const int &sink, unordered_map <int,int> &global_to_local) {

    for (auto &update:updates) {
        
        int global_u = update[0] ;
        int global_v = update[1] ;
        int update_amount = update[2] ;

        log_message (" update residual " + to_string (global_u) + " : " + to_string (global_v) + " : " + to_string (update_amount)) ;

        if (global_to_local.find (global_u) != global_to_local.end () && global_to_local.find (global_v) != global_to_local.end ()) {
            
            log_message ("found one node") ;
            int u = global_to_local[global_u] ;
            int v = global_to_local[global_v] ;

            for (int v_idx = 0; v_idx < residual[u].size (); v_idx++) {
                if (residual[u][v_idx][0] == v) {
                    residual[u][v_idx][1] -= update_amount ;
                }
            } 

            for (int u_idx = 0; u_idx < residual[v].size (); u_idx++) {

                if (residual[v][u_idx][0] == u) {
                    residual[v][u_idx][1] += update_amount ;
                }
            }
            excess[v] += update_amount ;
            excess[u] -= update_amount ;

            if (excess[v] > 0 and global_v != source and global_v != sink) {
                active_vertices.insert (v) ;
            }
        }
    	free (update-1) ;
    	update=NULL ;
    }
}
#endif
