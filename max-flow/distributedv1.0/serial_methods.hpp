
#ifndef _SERIAL_METHODS_ 

#define _SERIAL_METHODS_ 


#include <fstream>
#include <iostream>
#include <sstream>
#include <vector> 
#include <queue>
#include <chrono>
#include <climits>
#include <cassert>
#include <tuple>
#include <ctime> 
#include <iomanip>
#include <set>
#include <unistd.h>
#include <unordered_map>
#include <mpi.h>
#include <string.h>

using namespace std;

typedef long long ll;
typedef vector<ll> vi;
typedef vector<vi> vvi ;
typedef pair<ll,ll> ii ;
typedef vector<ii> vii ;
typedef vector<vvi> vvii ;
typedef queue<ll> q ;

#define FORWARD 0 
#define BACKWARD 1 


const int no_debug = 0 ;
const int log_emergency=1 ;
const int evaluate_mode = 0 ;

unordered_map <int,int> local_to_global ;
unordered_map <int,int> global_to_local ;
vi current_arc ;
vvi graph ;
vvii residual_graph ;
vvi edges ;
vi count ;
q active ;
ll g_vx, e, src, snk, v_id ;
ll counter_to_global_relabel=0 ;
ll pointer_1, pointer_2 ;



void log_graph (vvii &a) {
    if (evaluate_mode) return ;
    if (no_debug) return ;
    for (int i=0; i<a.size (); i++) {
        for (auto &it1:a[i]) {
            cerr <<i << " : " << it1[0] << " " << it1[1] << " " << it1[2] << " " << it1[3] << endl;
        }
    }
}

void log_arr (vi &a) {
    if (evaluate_mode) return ;
    if (no_debug) return ;
    for (auto &it:a) {
        cerr << it << " " ;
    }
    cerr << endl ;
}

void log_message (const string &msg) {
    if (evaluate_mode) return ;
     if (no_debug) return ;
    cerr << msg << endl ;
}

void log_message_push (const string &msg) {
    return ;
    //  if (no_debug) return ;
    if (evaluate_mode) return ;
    cerr << msg << endl ;
}

void log_sos (const string &msg) {
    if (evaluate_mode) return ;
	if (!log_emergency) return ;
	cout << msg << endl ;
}

void make_entry (const int &x) {

    if (global_to_local.find (x) == global_to_local.end ()) {
            global_to_local[x] = v_id ;
            v_id++ ;
        }
}

FILE* fileIO(char* file_path){

    char* global = "/home/cs22m028/Documents/CS22M028/StarPlat/max-flow/" ;
    int n = strlen (global) ;
    int m = strlen (file_path) ;

    char actual_path [n+m+1] ;
    strcpy (actual_path, global) ;
    strcat (actual_path, file_path) ;

    log_sos (actual_path) ;
    FILE* graph_file = fopen (actual_path, "r") ;
	
	if (graph_file == NULL) {
		log_sos ("FILE NOT FOUND") ;
		return NULL ;
	}

    return graph_file ;
}

void input (FILE* graph_file) {

    string s ;
    g_vx = 0, e = 0, v_id = 0 ;
    ll u, v, w, p ;
    while (fscanf (graph_file, "%lld %lld %lld %lld", &u, &v, &w, &p) == 4) {
        
        make_entry (u) ;
        make_entry (v) ;
        edges.push_back ({global_to_local[u],global_to_local[v],w,p}) ;
        g_vx = max (g_vx, v) ;
        g_vx = max (g_vx, u) ;
        e++ ;
    }

    v_id++ ; 

    log_message ("input successful") ;
	log_message (" number of vertices : " + to_string (v_id)) ;
    
	log_sos ("raw input OK") ;

    MPI_Barrier (MPI_COMM_WORLD) ;

    for (auto &edge:edges) {

        try {
            residual_graph[edge[0]].push_back ({edge[1], edge[2], (ll)residual_graph[edge[1]].size (),FORWARD, edge[3]}) ;
        }
        catch (...) {
            log_message ("failed at " + to_string (edge[0])) ;
        }

        try {
            residual_graph[edge[1]].push_back ({edge[0], 0, (ll)residual_graph[edge[0]].size ()-1ll, BACKWARD, edge[3]}) ;        
        }
        catch (...) {
            log_message ("failed at " + to_string (edge[1])) ;
        }
        
    }
    log_sos ("graph build successful") ;
}

// void fix_gaps () {

//     for (pointer_2 = pointer_1; pointer_2>=0 ; pointer_2--) if (!count[pointer_2]) break ;
//     cout << "gap = " << pointer_2 << endl ;
//     for (auto &it:heights) {
//         if (it > pointer_2) {
//             it = vx ;
//         }
//     }
// }


void update_gap (const ll &obliterate, const ll &current) {

    count[obliterate]-- ;
    count[current]++ ;

    pointer_1 = max (pointer_1, current) ;

    if (obliterate == pointer_1 && count[obliterate] == 0) {
        for (; pointer_1 >= 0; pointer_1--) {
            if (count[pointer_1]) break ;
        }
    }
}

// void bfs_label () {

//     for (auto &it:heights) {
//         it = vx ;
//     }
// 	log_message ("starting bfs : " + to_string (heights.size ())) ;
//     queue<pair<int,int> > q1 ;
//     q1.push ({0,snk}) ;
//     vector<int> visited (vx, 0) ;
//     int max_level = 0 ;
//     while (!q1.empty()) {
//         int level, u ;
//         tie (level, u) = q1.front () ;
//         max_level = max (max_level, level) ;
//         q1.pop () ;
// 		if (visited[u]) continue ;
// 		log_message ("heighst.size () = " + to_string (heights.size ())) ;
//         log_message ("searching through " + to_string (u)) ;
//         log_message ("assigning: " + to_string(u) + " " + to_string(heights[u])) ;
//         visited[u]=1 ;
//         heights[u]=level ;

//         for (auto &x:residual_graph[u]) {
//             int v = x[0] ;
//             int w = x[1] ;
//             int offset = x[2] ;
// 			int capacity = residual_graph[v][offset][1] ;

//             if (/*residual_graph[v][offset].back () == BACKWARD &&*/ !visited[v] && w < capacity) {
//                 q1.push ({level+1, v}) ;
//             }
//         }
//     }


//     heights[src] = max_level+1  ;
//     log_message ("bfs labeled heights : " ) ;
//     log_arr (heights) ;
    
// 	log_sos ("did bfs again") ;
// }

// void initialize_count_ds () {

//     for (auto &height:heights) {
//         count[height]++ ;
//         pointer_1 = max ((ll)pointer_1, height) ;
//     }
//     log_message ("count array : ") ;
//     log_arr (count) ;
//     log_message ("pointer_1 = " + to_string (pointer_1)) ;
//     pointer_2 = vx ;
//     for (; pointer_2 >= 0; pointer_2--) {
//         if (!count[pointer_2]) break ;
//     }
//     log_message ("pointer_2 = " + to_string (pointer_2)) ;
// }

// void max_flow_init ()  {

//     heights.assign (vx,vx) ;
//     excess.assign (vx, 0) ;
//     current_arc.assign (vx, 0) ;
//     count.assign (vx+5, 0) ;
// }


// bool relabel (int u) {

//     log_sos ("relabeling " + to_string (u) + " at height " + to_string (heights[u])) ;
//     log_message ("=======================================") ;
    

//     log_message ("=======================================") ;

//     assert (excess[u]>0 ) ;
//     ll minim = INT_MAX ;
//     for (int v=0; v<residual_graph[u].size (); v++) {
//         if (residual_graph[u][v][1]>0 ) {
//             assert (heights[u] <= heights[residual_graph[u][v][0]]) ;
//             minim = min (minim, heights[residual_graph[u][v][0]]) ;
//         }
//     }
//     int obliterate = heights[u] ;
//     heights[u] = min (minim + 1, (ll)vx) ;
//     // log_message ("new height : " + to_string (heights[u])) ;

//     // set<ll> mex (heights.begin (), heights.end ()) ;

//     update_gap (obliterate, heights[u]) ;
//     // ll gap = update_gap (mex) ;
//     // log_message ("gap = " + to_string (gap)) ;
//     // fix_gaps (gap) ;
//     log_message ("heights :") ;
//     log_arr (heights) ;
//     // sleep (30) ;

//     if (heights[u] == obliterate) return false ;

//     return true ;
// }


// bool push (const int &u, const int &v) {

//     assert (excess[u] > 0 && (heights[u] >= heights[v] + 1) ); 
//     ll delta = min (excess[u], residual_graph[u][current_arc[u]][1]) ;
//     // log_message_push ("pushing flow from " + to_string(u) + " to " + to_string (v) + " of val " + to_string (delta)) ;
//     // log_message_push ("heights : " + to_string (heights[u]) + " and " + to_string(heights[v])) ;
//     residual_graph[u][current_arc[u]][1]-=delta ;
//     log_graph (residual_graph) ;
//     residual_graph[v][residual_graph[u][current_arc[u]][2]][1]+=delta ;
//     excess[v]+=delta ;
//     excess[u]-=delta ;
//     if (excess[v]>0 && (v != src) && (v != snk) && excess[v] == delta) {
//         log_message(  "pushing " + to_string(v) + " into active "  );
//         active.push(v) ;
//     }
//     return true ;
// }

// void discharge (const int &u) {

//     while (excess[u]>0) {
        
//         // log_sos (to_string (active.size ()) + " " + to_string(active.front ()) + " " + to_string (excess[active.front ()])) ;
        
// 		if (counter_to_global_relabel==15) {
// 			counter_to_global_relabel=0 ;
//             // log_sos (to_string (active.size ()) + " " + to_string(active.front ()) + " " + to_string (excess[active.front ()])) ;
// 			bfs_label () ;
//             // fix_gaps () ;
// 		}

//         if (current_arc[u] < residual_graph[u].size ()) {
//             int v = residual_graph[u][current_arc[u]][0];
//             if (residual_graph[u][current_arc[u]][1] > 0 && heights[u] >= heights[v] + 1)
//                 push(u, v);
//             else 
//                 current_arc[u]++;
//         } else {
//             // relabel(u);

//             if (relabel(u) == false ) {
//                 return ;
//             }
//             current_arc[u] = 0;
//         }
// 		// counter_to_global_relabel++ ;
//     }
// }

// void max_flow () {
  
//     while (!active.empty () ) {

//         log_message ("queue element label : " + to_string (active.front ())) ;
//         int u = active.front () ;
//         active.pop () ;
//         log_message ("popped it out ") ;
//         if (u!=src && u!=snk) 
//         discharge (u) ;
//         log_arr (excess) ;
//         log_arr (heights) ;
//     }
// }

#endif