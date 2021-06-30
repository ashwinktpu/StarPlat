#include"sssp.h"
#include <atomic>
#include <sys/time.h>

void Compute_SSSP(graph& g,int* weight,int* dist,int src)
{ 

    
  const int node_count=g.num_nodes();
//  printf("nodeCount %d\n",node_count);
  omp_lock_t* lock=(omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

  #pragma omp parallel for num_threads(4)
  for(int v = 0; v<g.num_nodes(); v++)
  omp_init_lock(&lock[v]);
  //int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];

  #pragma omp parallel for num_threads(4)
  for (int t = 0; t<g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
 
  }
  modified[src] = true;
  dist[src] = 0;

  std::atomic_bool finished = {false};
  int round=0;
   
  while ( !finished )
  { 
    finished=true;
    #pragma omp parallel for num_threads(4) schedule(dynamic,128)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true ){
          modified[v] = false;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int nbr = g.edgeList[edge] ;
          int e = edge;
           int dist_new = dist[v] + weight[e];
           
          if (dist[nbr] > dist_new)
          {
            bool modified_new = true;
            finished.store(false,std::memory_order_relaxed);
            gm_spinlock_acquire_for_node(nbr);
            if (dist[nbr] > dist_new)
            {
              
              dist[nbr] = dist_new;
            
              modified[nbr] = modified_new;
            }
           gm_spinlock_release_for_node(nbr);
          }
        }
      }
    }
   
  
   
  }

    printf("Rounds : %d\n",round);
  
    delete[] weight;
    delete[] modified;
    delete[] lock;

   
 

}
int main()
{ 

  
   graph G("dataRecords/cleanuwUSARoad.txt");
   G.parseGraph();
   gm_spinlock_table_init();
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=0;
 
    struct timeval T1, T2;
    gettimeofday(&T1, NULL);
   Compute_SSSP(G,edgeLen,dist,src);
   gettimeofday(&T2, NULL);
    
   printf("running time=%lf\n", (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
   char *outputfilename = "outputSG.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");

  // First crossing vehicles list
  for (int i = 0; i <= G.num_nodes(); i++)
  {
    fprintf(outputfilepointer, "%d  %d\n", i, dist[i]);
  }
  
  
  
  




  return 0;
}