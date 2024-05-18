#include"scc_V3.h"

void scc(graph& g)
{
  bool* vis=new bool[g.num_nodes()];
  int* tim=new int[g.num_nodes()];
  int* low=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    vis[t] = false;
    tim[t] = -1;
    low[t] = -1;
  }
  int cur_t = 0;
  #pragma omp parallel for
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    if (tim[u] == -1 )
      {
      dfs(u,vis,tim,low,g,cur_t);

    }
  }

}
auto dfs(int u,bool* vis,int* tim,int* low,
  graph& g,int cur_t)
{
  vis[u] = true;
  tim[u] = cur_t;
  low[u] = cur_t;
  cur_t = cur_t + 1;
  #pragma omp parallel for
  for (int edge = g.indexofNodes[u]; edge < g.indexofNodes[u+1]; edge ++) 
  {int v = g.edgeList[edge] ;
    if (tim[v] == -1 )
      {
      cur_t = dfs(v,vis,tim,low,g,cur_t);
      if (low[u] > low[v] )
        {
        low[u] = low[v];
      }
    }
    else
    if (vis[v] == true )
      {
      if (tim[v] < low[u] )
        {
        low[u] = tim[v];
      }
    }
  }
  if (low[u] == tim[u] )
    {
    dfs1(u,g,vis);

  }
  return cur_t;

}
void dfs1(int u,graph& g,bool* vis)
{
  vis[u] = false;
  #pragma omp parallel for
  for (int edge = g.indexofNodes[u]; edge < g.indexofNodes[u+1]; edge ++) 
  {int v = g.edgeList[edge] ;
    if (vis[v] == true )
      {
      dfs1(v,g,vis);

    }
  }

}
