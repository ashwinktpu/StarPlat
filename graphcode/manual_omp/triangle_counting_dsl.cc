#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void Compute_TC(graph& g)
{
  long triangle_count = 0;

  #pragma omp parallel for num_threads(4) reduction(+ : triangle_count)
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
    { 
      int u = g.edgeList[edge] ;
      if (u < v )
      {
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
         { 
          int w = g.edgeList[edge] ;
          if (w > v )
          {
            if (g.check_if_nbr(u, w) )
            {
              triangle_count+=1;
            }
          }
        }
      }
    }
  }

 printf("Triangle Count %d\n",triangle_count);

}

int main()
{

  graph G("../sample_graphs/test_graph.txt");
  G.parseGraph();

  float* node_pr = new float[G.num_nodes()];

 
  // double startTime = omp_get_wtime();
  Compute_TC(G);
  // double endTime = omp_get_wtime();
  // printf("EXECUTION TIME %f \n", endTime - startTime);

 char *outputfilename = "outputN.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");

  // First crossing vehicles list
  for (int i = 0; i <G.num_nodes(); i++)
  {
    fprintf(outputfilepointer, "%d  %0.9lf\n", i, node_pr[i]);
  }
  
}