#include"edmonds-karp.h"

void ek(graph& g , int s , int d , int* weight
)
{
  int flow = 0;
  int cur;
  int prev;
  int new_flow = -1;
  int cnew_flow = 0;
  int e_cap = 0;
  bool terminate = false;
  int* cap=new int[g.num_edges()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_edges(); t ++) 
  {
    cap[t] = weight[t];
  }
  while (new_flow != 0 ){
    new_flow = 0;
    int* n_flow=new int[g.num_nodes()];
    int* par=new int[g.num_nodes()];
    int* bfsDist=new int[g.num_nodes()];
    bool* in_bfs=new bool[g.num_nodes()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      n_flow[t] = INT_MAX;
      par[t] = -1;
      in_bfs[t] = false;
      bfsDist[t] = -1;
    }
    in_bfs[s] = true;
    par[s] = -2;
    bfsDist[s] = 0;
    terminate = false;
    std::vector<std::vector<int>> levelNodes(g.num_nodes()) ;
    std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
    std::vector<int>  levelCount(g.num_nodes()) ;
    int phase = 0 ;
    levelNodes[phase].push_back(s) ;
    int bfsCount = 1 ;
    levelCount[phase] = bfsCount;
    while ( bfsCount > 0 )
    {
       int prev_count = bfsCount ;
      bfsCount = 0 ;
      #pragma omp parallel for
      for( int l = 0; l < prev_count ; l++)
      {
        int cur = levelNodes[phase][l] ;
        for(int edge = g.indexofNodes[cur] ; edge < g.indexofNodes[cur+1] ; edge++) {
          int nbr = g.edgeList[edge] ;
          int dnbr ;
          if(bfsDist[nbr]<0)
          {
            dnbr = __sync_val_compare_and_swap(&bfsDist[nbr],-1,bfsDist[cur]+1);
            if (dnbr < 0)
            {
              int num_thread = omp_get_thread_num();
               levelNodes_later[num_thread].push_back(nbr) ;
            }
          }
        }
        if (in_bfs[cur] == true && terminate == false )
          {
          for (int edge = g.indexofNodes[cur]; edge < g.indexofNodes[cur+1]; edge ++) 
          {int nbr = g.edgeList[edge] ;
            int e = edge;
            if (par[nbr] == -1 && cap[e] > 0 )
              {
              par[nbr] = cur;
              cnew_flow = n_flow[cur];
              if (cnew_flow > cap[e] )
                {
                cnew_flow = cap[e];
              }
              if (nbr == d )
                {
                new_flow = cnew_flow;
                terminate = true;
              }
              in_bfs[nbr] = true;
              n_flow[nbr] = cnew_flow;
            }
          }
        }
      }
      phase = phase + 1 ;
      for(int i = 0;i < omp_get_max_threads();i++)
      {
         levelNodes[phase].insert(levelNodes[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
         bfsCount = bfsCount+levelNodes_later[i].size();
         levelNodes_later[i].clear();
      }
       levelCount[phase] = bfsCount ;
    }
    phase = phase - 1 ;
    while (phase > 0)
    {
      #pragma omp parallel for
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int cur = levelNodes[phase][l] ;
      }
      phase = phase - 1 ;
    }
    flow = flow + new_flow;
    cur = d;
    while (cur != s ){
      prev = par[cur];
      int back = g.getEdge(prev,cur).id;
      cap[back] = cap[back] - new_flow;
      int forw = g.getEdge(cur,prev).id;
      cap[forw] = cap[forw] + new_flow;
      cur = prev;
    }
  }
  printf("MaxFlow:%d\n",flow);

}

int main(int argc, char** argv)
{
  graph G(argv[1]);
  G.parseGraph();
  int* edgeLen=G.getEdgeLen();
  double st = omp_get_wtime();
  ek(G,atoi(argv[2]),atoi(argv[3]),edgeLen);
  double en = omp_get_wtime();
  printf("Execution Time %f\n",en - st);

}