#include"sssp.h"

void Compute_SSSP(graph g,int* weight,int* dist,int src)
{
  const int node_count=g.num_nodes();
  omp_lock_t lock[node_count+1];
  #pragma omp parallel for
  for(int v = 1; v<=g.num_nodes(); v++)
  omp_init_lock(&lock[v]);
  //int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 1; t<=g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  while ( !finished )
  {
    #pragma omp parallel for
    for (int v = 1; v <= g.num_nodes(); v ++) 
    {
      if (modified[v] == true ){
        modified[v] = false;
        #pragma omp parallel for
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int nbr = g.edgeList[edge] ;
          int e = edge;
           int dist_new = dist[v] + weight[e];
          if (dist[nbr] > dist_new)
          {
            bool modified_new = true;
            omp_set_lock(&(lock[nbr])) ;
            if (dist[nbr] > dist_new)
            {
              dist[nbr] = dist_new;
              modified[nbr] = modified_new;
            }
            omp_unset_lock(&(lock[nbr]));
          }
        }
      }
    }
    bool modified_fp = false ;
    #pragma omp parallel for reduction(||:modified_fp)
    for (int v = 1; v <= g.num_nodes(); v ++) 
    modified_fp = modified_fp || modified[v] ;
    finished = !modified_fp ;
  }

}
int main()
{ 

 
   graph G("oregon1_010526.txt");
   G.parseGraph();
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=1;

   Compute_SSSP(G,edgeLen,dist,src);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  for(int i=16700;i<=16752;i++)
  {
      std::cout<<dist[i]<<" ";
  }
  
  
  




  return 0;
}