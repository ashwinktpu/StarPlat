#include"bc_dsl.h"

void Compute_BC(graph g)
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
      #pragma omp parallel for
      for( int l = 0; l < prev_count ; l++)
      {
        int v = levelNodes[phase][l] ;
        for(int edge = g.indexofNodes[v] ; edge < g.indexofNodes[v+1] ; edge++) {
          int nbr = g.edgeList[edge] ;
          int dnbr ;
          dnbr = __sync_val_compare_and_swap(&bfsDist[nbr],-1,bfsDist[v]+1);
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
      for( int i = 0; i < levelCount[phase] ; i++)
      {
        int v = levelNodes[phase][i] ;
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
  }
}

int main()
{ 

 	struct timeval start, end;
   //graph G("soc-Slashdot0811.txt");
   graph G("sample_graph.txt");
   gettimeofday(&start, NULL);
   G.parseGraph();
   gettimeofday(&end, NULL);
   long seconds = (end.tv_sec - start.tv_sec);
   long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
   printf("The graph loading time = %ld micro secs.\n",micros);
  int* edgeLen=G.getEdgeLen();
  int* dist=new int[G.num_nodes()+1];
  int* parent=new int[G.num_nodes()+1];
  int src=0;
  //vector <int> seeds;
  //for (int i=0;i<10;i++)
	//seeds.push_back(i);
	  //seeds.push_back(rand()%G.num_nodes());
  //for (int i=0;i<5;i++)
  //	cout<<seeds[i]<<endl;
  /*
  printf("The number of nodes is %d\n",G.num_nodes());
  printf("The number of edges is %d\n",G.num_edges());
  printf("The maximum degree of the graph is %d\n",G.max_degree());
  for(int i=0;i<G.num_nodes()+1;i++)
  {
  	std::cout<<G.indexofNodes[i]<< " ";
  }
  printf("\nEdgedetails\n");
  for(int i=0;i<G.num_edges();i++)
  {
  	std::cout<<G.edgeList[i]<< " ";
  }
  printf("\nNeighbordetails\n");
  for(int i=0;i<G.num_nodes();i++)
  {
  printf("The neighbors of node %d is ",i);
  for (int j = G.indexofNodes[i]; j<G.indexofNodes[i+1]; j ++)
            {
                int nbr=G.edgeList[j];
                printf("%d : %d, ",nbr,G.edgeLen[j]);
              }
              printf("\n");
  }
   */           
  //for(int i=1;i<G.num_nodes()+1;i++)
  //{
  //	std::cout<<G.edge[i]<< " ";
  //}
   Compute_BC(G);

 // calculateShortestPath(indexOfNodes,edgeIndex,edgeLen,parent,dist,nodes,2);
  
  //for(int i=0;i<G.num_nodes();i++)
  //{
    //  std::cout<<dist[i]<<" ";
  //}
  
  return 0;
}
