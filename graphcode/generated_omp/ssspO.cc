#include"sssp.h"
#include <atomic>

void Compute_SSSP(graph& g,int* weight,int* dist,int src)
{
  const int node_count=g.num_nodes();
  printf("nodeCount %d\n",node_count);
  omp_lock_t* lock=(omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

  #pragma omp parallel for num_threads(4)
  for(int v = 0; v<g.num_nodes(); v++)
  omp_init_lock(&lock[v]);
  //int* dist=new int[g.num_nodes()];
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  #pragma omp parallel for num_threads(4)
  for (int t = 0; t<g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
    modified_nxt[t]=false;
  }
  modified[src] = true;
  dist[src] = 0;
  //std::atomic_bool finished = {false};
  int rounds=0;
   bool finished = false;
  double regionTime=omp_get_wtime();
  while ( !finished )
  {
    /*finished=true;
    rounds++;*/
    #pragma omp parallel for num_threads(4) schedule(dynamic,128)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true ){
          modified[v] = false;
         // int v_dist=dist[v];
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int nbr = g.edgeList[edge] ;
          int e = edge;
           int dist_new = dist[v] + weight[e];
          if (dist[nbr] > dist_new)
          {
            bool modified_new = true;
            //finished.store(false,std::memory_order_relaxed);
            omp_set_lock(&lock[nbr]);
            if (dist[nbr] > dist_new)
            {
              
              dist[nbr] = dist_new;
              modified[nbr] = modified_new;
            }
            omp_unset_lock(&lock[nbr]);
          }
        }
      }
    }

   

    bool modified_fp = false ;
    #pragma omp parallel for num_threads(4) reduction(||:modified_fp)
    for (int v = 0; v <g.num_nodes(); v ++) 
    modified_fp = modified_fp || modified[v] ;
    finished=!modified_fp;

     /*#pragma omp parallel for num_threads(4)
     for (int v = 0; v <g.num_nodes(); v ++) 
           {
            modified[v] = modified_nxt[v];
            modified_nxt[v]=false;
           }*/

  }
   double regionTimeE=omp_get_wtime();
  printf("rounds %d TimeRegion %f\n",rounds, regionTimeE-regionTime);
  delete[] modified;
  delete[] weight;
  

}

int main()
{ 

 
   graph G("dataRecords/cleanuwUSARoad.txt");
   G.parseGraph();
   //gm_spinlock_table_init();
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* dist_next=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=0;
   double startTime=omp_get_wtime();
   Compute_SSSP(G,edgeLen,dist,src);
   double endTime=omp_get_wtime();
   printf("RunTime : %f\n",endTime-startTime);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
 /* for(int i=16700;i<=16752;i++)
  {
      std::cout<<dist[i]<<" ";
  }*/

  char *outputfilename = "outputSG.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");

  // First crossing vehicles list
  for (int i = 0; i <G.num_nodes(); i++)
  {
    fprintf(outputfilepointer, "%d  %d\n", i, dist[i]);
  }
  
  
  




  return 0;
}