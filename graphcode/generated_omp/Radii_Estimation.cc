#include"Radii_Estimation.h"

void rad(graph& g , int k)
{
  int* visited=new int[g.num_nodes()];
  int* n_visited=new int[g.num_nodes()];
  bool* old_front=new bool[g.num_nodes()];
  bool* old_front_nxt=new bool[g.num_nodes()];
  bool* new_front=new bool[g.num_nodes()];
  int* Radii=new int[g.num_nodes()];
  int round = 0;
  int j = 1;
  int t = 0;
  int co = 0;
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    visited[t] = 0;
    n_visited[t] = 0;
    Radii[t] = -1;
    old_front[t] = false;
    old_front_nxt[t] = false;
    new_front[t] = false;
  }
  while (co < k ){
    old_front[co] = true;
    Radii[co] = 0;
    visited[co] = j;
    j = j * 2;
    co++;
  }
  bool finished = false;
  while ( !finished )
  {
    finished = true;
    round = round + 1;
    #pragma omp parallel
    {
      #pragma omp for
      for (int u = 0; u < g.num_nodes(); u ++) 
      {
        if (old_front[u] == true )
          {
          for (int edge = g.indexofNodes[u]; edge < g.indexofNodes[u+1]; edge ++) 
          {int v = g.edgeList[edge] ;
            if (visited[u] != visited[v] )
              {
              n_visited[v] = visited[u] || visited[v];
               int Radii_new = round;
              bool old_front_new = true;
              if(Radii[v] > Radii_new)
              {
                int oldValue = Radii[v];
                atomicMin(&Radii[v],Radii_new);
                if( oldValue > Radii[v])
                {old_front_nxt[v] = old_front_new;
                  finished = false ;
                }
              }
            }
          }
        }
      }
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      { old_front[v] =  old_front_nxt[v] ;
        old_front_nxt[v] = true ;
      }
    }
    #pragma omp parallel
    {
      #pragma omp for
      for (int u = 0; u < g.num_nodes(); u ++) 
      {
        if (old_front[u] == true )
          {
          n_visited[u] = n_visited[u] || visited[u];
        }
      }
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      { old_front[v] =  old_front_nxt[v] ;
        old_front_nxt[v] = true ;
      }
    }
    #pragma omp parallel for
    for (int u = 0; u < g.num_nodes(); u ++) 
    {
      int t = visited[u];
      visited[u] = n_visited[u];
      n_visited[u] = t;
    }
  }

}
