#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<cmath>
#include<algorithm>
#include<omp.h>
#include"graph.hpp"

void Compute_PR(graph& g,float beta, float delta, int maxIter, float* node_pr)
 {
  float num_nodes=(float)g.num_nodes();


  #pragma omp parallel for num_threads(4)
  for(int i=0;i<g.num_nodes();i++)
     {
         node_pr[i]=1/num_nodes;
     }

  int iterCount=0;
  float diff;



  do
  {
    diff=0.0;
   
    #pragma omp parallel for num_threads(4) reduction(+:diff) 
    for(int v=0;v<g.num_nodes();v++)
    { 
      float sum=0.0;
      
      for(int edge=g.rev_indexofNodes[v];edge<g.rev_indexofNodes[v+1];edge++)
         {
           int nbr=g.srcList[edge];
           sum=sum+node_pr[nbr]/(g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
          
         }

         float val=(1-delta)/num_nodes+delta*sum;
         float temp = std::fabs(val-node_pr[v]);
         diff=diff+temp;
       /*   if(diff<temp)
           diff=temp;*/
         node_pr[v]=val;

    }
     

    
   
 // printf("iterCount %d diff %f\n",iterCount,diff);
   
  iterCount=iterCount+1;

  


  } while ((diff>beta)&&(iterCount<maxIter));
     
 //printf("iteration took %d\n",iterCount);



 }

 int main()
{

  graph G("dataRecords/uniform-random1m.txt");
  G.parseGraph();
  float* node_pr = new float[G.num_nodes()];

 
  double startTime = omp_get_wtime();
  Compute_PR(G,0.001,0.85,100,node_pr);
  double endTime = omp_get_wtime();
  printf("EXECUTION TIME %f \n", endTime - startTime);

  char *outputfilename = "outputN.txt";
  FILE *outputfilepointer;
  outputfilepointer = fopen(outputfilename, "w");

  // First crossing vehicles list
  for (int i = 0; i <G.num_nodes(); i++)
  {
    fprintf(outputfilepointer, "%d  %0.9lf\n", i, node_pr[i]);
  }

  
}
