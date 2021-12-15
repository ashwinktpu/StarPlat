#include"SSSP_V2.h"

__global__ void Compute_SSSP_kernel(graph& g,int* dist,int src)

{
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
    modified_nxt[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  while ( !finished[0] )
  {
    finished[0] = true;
    
    bool* tempModPtr = modified_nxt ;
    modified_nxt = modified_prev ;
    modified_prev = tempModPtr ;
    modified_nxt[v] = false ;

}