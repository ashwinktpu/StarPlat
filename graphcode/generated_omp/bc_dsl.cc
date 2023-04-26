#include"bc_dsl.h"

Compute_BC(graph g)
{
  const int node_count=g.num_nodes();
  omp_lock_t lock[node_count+1];
  #pragma omp parallel for
  for(int v = 0; v<g.num_nodes(); v++)
  omp_init_lock(&lock[v]);
  float* BC=new float[g.num_nodes()];

  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    BC[t] = 0;
  }
  
  #pragma omp parallel for
  for (int src = 0; src < g.num_nodes(); src ++) 
  {
    int* sigma=new int[g.num_nodes()];
    int* bfsDist=new int[g.num_nodes()];
    float* delta=new float[g.num_nodes()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      delta[t] = 0;
      bfsDist[t] = -1;
    }
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      sigma[t] = 0;
    }
    bfsDist[src] = 0;
    sigma[src] = 1;
    std::vector<std::vector<int>> levelNodes(g.num_nodes()) ;
    std::vector<int>  levelNodes_later(g.num_nodes()) ;
    std::vector<int>  levelCount(g.num_nodes()) ;
    int phase = 0 ;
    levelNodes[phase].push_back(src) ;
    std::atomic_int bfsCount = {1} ;
    levelCount[phase] = bfsCount;
    while ( bfsCount > 0 )
    {
       int prev_count = bfsCount ;
      bfsCount = 0 ;
      #pragma omp for all
      for( int l = 0; l = prev_count ; l++)
      {
        int v = levelNodes[phase][l] ;
        for(int edge = g.indexofNodes[v] ; edge < g.indexofNodes[v+1] ; edge++) {
          int nbr = g.edgeList[edge] ;
          int dnbr ;
          dnbr = __sync_val_compare_and_swap(&dist[nbr],-1,dist[v]+1);
          if (dnbr < 0)
          {
            int loc = bfsCount.fetch_add(1,std::memory_order_relaxed) ;
             levelNodes_later[loc]=nbr ;
          }
        }
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int w = g.edgeList[edge] ;
          if(bfsDist[w]==bfsDist[v]+1)
          {
            #pragma omp atomic
            sigma[w] = sigma[w] + sigma[v];
          }
        }
      }
      phase = phase + 1 ;
      levelCount[phase] = bfsCount ;
       levelNodes[phase].assign(levelNodes_later.begin(),levelNodes_later.begin()+bfsCount);
    }
    phase = phase -1 ;
    while (phase > 0)
    {
      #pragma omp parallel for
      for( int i = 0; l = levelCount[phase] ; l++)
      {
        int v = levelCount[phase][i] ;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int w = g.edgeList[edge] ;
          if(bfsDist[w]==bfsDist[v]+1)
          {
            delta[v] = delta[v] + sigma[v] / sigma[w] * 1 + delta[w];
          }
        }
        BC[v] = BC[v] + delta[v];
      }

}