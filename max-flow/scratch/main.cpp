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


const int no_debug = 1 ;
const int log_emergency=1 ;
const int evaluate_mode = 0 ;
vi heights ;
vi excess ;
vi current_arc ;
vvi graph ;
vvii residual_graph ;
vvi edges ;
vi count ;
q active ;
ll vx, e, src, snk ;
ll counter_to_global_relabel=0 ;
ll pointer_1, pointer_2 ;

void fileIO(){
    freopen("input.txt", "r", stdin);
}

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

void input (FILE* graph_file) {

    string s ;
    vx = 0, e = 0 ;
    ll u, v, w ;
    while (fscanf (graph_file, "%lld %lld %lld", &u, &v, &w) == 3) {

        edges.push_back ({u,v,w}) ;
        vx = max (vx, v) ;
        vx = max (vx, u) ;
        e++ ;
    }
    vx++ ;

    log_message ("input successful") ;
	log_message (" number of vertices : " + to_string (vx)) ;
    
    residual_graph.resize (vx) ;
	log_sos ("raw input OK") ;

    for (auto &edge:edges) {
        residual_graph[edge[0]].push_back ({edge[1], edge[2], (ll)residual_graph[edge[1]].size (),FORWARD}) ;
        residual_graph[edge[1]].push_back ({edge[0], 0, (ll)residual_graph[edge[0]].size ()-1ll, BACKWARD}) ;
    }
    log_sos ("graph build successful") ;
}

void fix_gaps (const ll &gap) {

    for (auto &it:heights) {
        if (it > gap) {
            it = vx ;
        }
    }
}


ll update_gap (const ll &obliterate, const ll &current) {

    count[obliterate]-- ;
    count[current]++ ;

    pointer_1 = max (pointer_1, current) ;

    if (obliterate == pointer_1 && count[obliterate] == 0) {
        for (; pointer_1 >= 0; pointer_1--) {
            if (count[pointer_1]) break ;
        }
    }

    for (; pointer_2 >= 0; pointer_2--) {
        if (!count[pointer_2]) break ;
    }

    return pointer_2 ; // pointer_2 is the gap.
}

void bfs_label () {

    for (auto &it:heights) {
        it = vx ;
    }
	log_message ("starting bfs : " + to_string (heights.size ())) ;
    queue<pair<int,int> > q1 ;
    q1.push ({0,snk}) ;
    vector<int> visited (vx, 0) ;
    int max_level = 0 ;
    while (!q1.empty()) {
        int level, u ;
        tie (level, u) = q1.front () ;
        max_level = max (max_level, level) ;
        q1.pop () ;
		if (visited[u]) continue ;
		log_message ("heighst.size () = " + to_string (heights.size ())) ;
        log_message ("searching through " + to_string (u)) ;
        log_message ("assigning: " + to_string(u) + " " + to_string(heights[u])) ;
        visited[u]=1 ;
        heights[u]=level ;

        for (auto &x:residual_graph[u]) {
            int v = x[0] ;
            int w = x[1] ;
            int offset = x[2] ;
			int capacity = residual_graph[v][offset][1] ;

            if (/*residual_graph[v][offset].back () == BACKWARD &&*/ !visited[v] && w < capacity) {
                q1.push ({level+1, v}) ;
            }
        }
    }


    heights[src] = max_level+1  ;
    log_message ("bfs labeled heights : " ) ;
    log_arr (heights) ;
    
	log_sos ("did bfs again") ;
}

void initialize_count_ds () {

    for (auto &height:heights) {
        count[height]++ ;
        pointer_1 = max ((ll)pointer_1, height) ;
    }
    log_message ("count array : ") ;
    log_arr (count) ;
    log_message ("pointer_1 = " + to_string (pointer_1)) ;
    pointer_2 = vx ;
    for (; pointer_2 >= 0; pointer_2--) {
        if (!count[pointer_2]) break ;
    }
    log_message ("pointer_2 = " + to_string (pointer_2)) ;
}

void max_flow_init ()  {

    heights.assign (vx,vx) ;
    excess.assign (vx, 0) ;
    current_arc.assign (vx, 0) ;
    count.assign (vx+5, 0) ;
}

void set_source_sink (const int &source, const int &sink) {

    src = source ;
    snk = sink ;

    log_graph (residual_graph) ;

    heights[src] = vx ;
    for (int v_idx = 0 ; v_idx < residual_graph[src].size (); v_idx++ ){
        int v = residual_graph[src][v_idx][0] ;
        residual_graph[v][residual_graph[src][v_idx][2]][1] += residual_graph[src][v_idx][1] ;
        excess[v] += residual_graph[src][v_idx][1] ;
        residual_graph[src][v_idx][1] = 0 ;
        if (excess[v] > 0 && v!= src && v!=snk) {
            active.push (v) ;
        }
    }
    excess[src] = 0 ;
}

bool relabel (int u) {

    // log_message_push ("relabeling " + to_string (u) + " at height " + to_string (heights[u])) ;
    log_message ("=======================================") ;
    log_arr (heights) ;
    log_arr (excess) ;
    log_graph (residual_graph) ;
    log_message ("=======================================") ;

    assert (excess[u]>0 ) ;
    ll minim = INT_MAX ;
    for (int v=0; v<residual_graph[u].size (); v++) {
        if (residual_graph[u][v][1]>0 ) {
            assert (heights[u] <= heights[residual_graph[u][v][0]]) ;
            minim = min (minim, heights[residual_graph[u][v][0]]) ;
        }
    }
    int obliterate = heights[u] ;
    heights[u] = min (minim + 1, (ll)vx) ;
    // log_message ("new height : " + to_string (heights[u])) ;

    // set<ll> mex (heights.begin (), heights.end ()) ;

    ll gap = update_gap (obliterate, heights[u]) ;
    // ll gap = update_gap (mex) ;
    log_message ("gap = " + to_string (gap)) ;
    fix_gaps (gap) ;
    log_message ("heights :") ;
    log_arr (heights) ;
    // sleep (30) ;

    if (heights[u] == obliterate) return false ;

    return true ;
}


bool push (const int &u, const int &v) {

    assert (excess[u] > 0 && (heights[u] >= heights[v] + 1) ); 
    ll delta = min (excess[u], residual_graph[u][current_arc[u]][1]) ;
    // log_message_push ("pushing flow from " + to_string(u) + " to " + to_string (v) + " of val " + to_string (delta)) ;
    // log_message_push ("heights : " + to_string (heights[u]) + " and " + to_string(heights[v])) ;
    residual_graph[u][current_arc[u]][1]-=delta ;
    log_graph (residual_graph) ;
    residual_graph[v][residual_graph[u][current_arc[u]][2]][1]+=delta ;
    excess[v]+=delta ;
    excess[u]-=delta ;
    if (excess[v]>0 && (v != src) && (v != snk) && excess[v] == delta) {
        log_message(  "pushing " + to_string(v) + " into active "  );
        active.push(v) ;
    }
    return true ;
}

void discharge (const int &u) {

    while (excess[u]>0) {
        
        // log_sos (to_string (active.size ()) + " " + to_string(active.front ()) + " " + to_string (excess[active.front ()])) ;
        
		if (counter_to_global_relabel==15) {
			counter_to_global_relabel=0 ;
            // log_sos (to_string (active.size ()) + " " + to_string(active.front ()) + " " + to_string (excess[active.front ()])) ;
			bfs_label () ;
		}

        if (current_arc[u] < residual_graph[u].size ()) {
            int v = residual_graph[u][current_arc[u]][0];
            if (residual_graph[u][current_arc[u]][1] > 0 && heights[u] >= heights[v] + 1)
                push(u, v);
            else 
                current_arc[u]++;
        } else {
            // relabel(u);

            if (relabel(u) == false ) {
                return ;
            }
            current_arc[u] = 0;
        }
		// counter_to_global_relabel++ ;
    }
}

void max_flow () {

    while (!active.empty () ) {

        log_message ("number of items in queue : " + to_string (active.size ())) ;
        log_message ("queue element label : " + to_string (active.front ())) ;
        int u = active.front () ;
        active.pop () ;
        log_message ("popped it out ") ;
        if (u!=src && u!=snk) 
        discharge (u) ;
        log_arr (excess) ;
        log_arr (heights) ;
    }
}

int main (int argc, char** argv) {
    //fileIO();

	char* file_path = argv[1] ;
	FILE* graph_file = fopen (file_path, "r") ;
	log_sos (file_path) ;
	if (graph_file == NULL) {
		log_sos ("FILE NOT FOUND") ;
		return 1 ;
	}

    auto start_tick = std::chrono::steady_clock::now () ;
	log_sos ("starting the code") ;
    input (graph_file) ;
    auto input_done_tick = std::chrono::steady_clock::now () ;
    auto duration = std::chrono::duration_cast <std::chrono::minutes> (input_done_tick-start_tick) ;
    log_sos ("Input time : " +  to_string(duration.count ()) + " us") ;
	log_sos ("input OK") ;

    max_flow_init () ;
	log_sos ("init OK") ;

    int source = 0 ;
    int sink = 1 ;
    set_source_sink (source, sink) ;
	log_sos ("source and sink OK ") ;

    bfs_label () ;
    initialize_count_ds () ;
    log_sos ("counts made OK") ;
    auto bfs_tick = std::chrono::steady_clock::now () ;
    duration = std::chrono::duration_cast <std::chrono::minutes> (bfs_tick-start_tick) ;
    log_sos ("bfs time : " +  to_string(duration.count ()) + " us") ;
	log_sos ("bfs labelling OK " ) ; 

    max_flow () ;
	log_sos ("max flow successful ") ; 
    auto end_tick = std::chrono::steady_clock::now () ;
    duration = std::chrono::duration_cast <std::chrono::minutes> (end_tick-input_done_tick) ;
    log_sos ("total time : " + to_string (duration.count()) + " s") ;

    log_arr (excess) ;
    cout << excess[sink] << endl ;
  return 0;
}
