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
#include "distributed_methods.hpp"
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
        queue<int> modified_heights_sync_list ;
        int active_nodes ;
    
    public:

        Distributed_Network (char* file_name, const int &source, const int &sink, const int &my_rank, const int &size)  {
            
            FILE* graph_file = fileIO (file_name) ; 
            input (graph_file) ; 
            this->p_no = my_rank ;

            MPI_Allreduce (&g_vx, &global_vertices, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD) ;
            global_vertices++ ;
            MPI_Allreduce (&e, &global_edges, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;

            max_distributed_flow_init (my_rank, source, sink, v_id, global_vertices, heights, excess, CAND, work_list, heights_buffer, excess_buffer, CAND_buffer, work_list_buffer) ;

            active_nodes = 1 ;
        }

        ~Distributed_Network () {
            MPI_Win_free (&heights) ;
            MPI_Win_free (&excess) ;
            MPI_Win_free (&work_list) ;
            MPI_Win_free (&CAND) ;
        }

        bool discharge_until_one_relabel (const int &curr_v) {

            
        }

        void sync_all_heights () {

            return ;
        }

        int discharge_active_vertices () {
            
            int did_work = 0 ;

            // start pulse.
            MPI_Barrier (MPI_COMM_WORLD) ;

            for (int v=0; v<v_id; v++) {
                int active_status ;
                MPI_Get (&active_status, 1, MPI_INT, p_no, v, 1, MPI_INT, work_list) ;
                if (active_status) {
                    if(discharge_until_one_relabel (v)) modified_heights_sync_list.push (v) ;
                    did_work = 1 ;
                }
            }

            // end pulse.
            MPI_Barrier (MPI_COMM_WORLD) ;

            // synchronize the heights.
            sync_all_heights () ;

            MPI_Allreduce (&did_work, &active_nodes, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD) ;

            MPI_Barrier (MPI_COMM_WORLD) ;

            return active_nodes ;
        }
        void sync_active_vertices () {

        }
} ;

#endif
