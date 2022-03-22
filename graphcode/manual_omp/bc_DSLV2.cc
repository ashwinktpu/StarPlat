#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../randomGeneratorUtil.h"

void Compute_BC(graph& g, float *BC,std::set<int>& sourceSet)
{
   const int node_count=g.num_nodes();
  /*omp_lock_t lock[node_count+1];
  #pragma omp parallel for
  for(int v = 0; v<g.num_nodes(); v++)
  omp_init_lock(&lock[v]);*/
  /*float* BC=new float[g.num_nodes()];*/
  #pragma omp parallel for num_threads(4)
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    BC[t] = 0;
  }
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++)
  {
    int src = *itr;
    double* sigma=new double[g.num_nodes()];
    int* bfsDist=new int[g.num_nodes()];
    float* delta=new float[g.num_nodes()];
    #pragma omp parallel for num_threads(4)
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      delta[t] = 0;
      bfsDist[t] = -1;
    }
    #pragma omp parallel for num_threads(4)
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
      #pragma omp parallel for schedule(dynamic,256) num_threads(4)
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
      #pragma omp parallel for num_threads(4)
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int v = levelNodes[phase][l] ;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int w = g.edgeList[edge] ;
          if(bfsDist[w]==bfsDist[v]+1)
          {
           // delta[v] = delta[v] + sigma[v] / sigma[w] *( 1 + delta[w]);
             float contrib = ((float)1 + delta[w]) /
                              sigma[w];
              delta[v] = delta[v] + contrib;
          }
        }
        delta[v]*= sigma[v];
        BC[v] = BC[v] + delta[v];
      }

      phase=phase-1;
    }
  }

}

int main()
{

  graph G("../dataRecords/uniform-random1m.txt");
  G.parseGraph();
  randomGeneratorUtil rand;
  //randomGeneratorUtil rand(0,G.num_nodes());
  float *BC = new float[G.num_nodes()];
  std::set<int> seeds;

  //int seeds[5];={267649,518682,10978,22382,43958};
   //rand.feedRandomUniqueNum(seeds,20);
  //int nseeds[5];
    rand.feedFromFile(seeds,"../dataRecords/socJournalSources/sources-uniform/sources20.txt");
   /* std::set<int>::iterator itr;
   int count=0;
    char *outputfilename = "sources20.txt";
    FILE *outputfilepointer;
     outputfilepointer = fopen(outputfilename, "w");
   for(itr=seeds.begin();itr!=seeds.end();itr++)
   {
       fprintf(outputfilepointer,"%d\n",*itr);
        //nseeds[count++]=*itr;
   }*/
  double startTime = omp_get_wtime();
  Compute_BC(G, BC, seeds);
  double endTime = omp_get_wtime();
  printf("EXECUTION TIME %f \n", endTime - startTime);

  char *outputfilename = "outputI.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");

  // First crossing vehicles list
  for (int i = 0; i <= G.num_nodes(); i++)
  {
    fprintf(outputfilepointer, "%d  %f\n", i, BC[i]);
  }
}