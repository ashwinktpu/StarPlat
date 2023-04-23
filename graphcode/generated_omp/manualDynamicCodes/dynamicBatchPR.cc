#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void staticPR(graph& g,float beta,float delta,int maxIter,
  float* pageRank)
{
  float num_nodes = (float)g.num_nodes();
  float* pageRank_nxt=new float[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    pageRank[t] = 1 / num_nodes;
  }
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    #pragma omp parallel for reduction(+ : diff)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      float sum = 0.000000;
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
      {int nbr = g.srcList[edge] ;
        sum = sum + pageRank[nbr] / g.getOutDegree(nbr);
      }
      float val = (1 - delta) / num_nodes + delta * sum;
      diff = diff + val - pageRank[v];
      pageRank_nxt[v] = val;
    }
    #pragma omp parallel for
    for (int node = 0; node < g.num_nodes(); node ++) 
    {
      pageRank [node] = pageRank_nxt [node] ;
    }
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
  free(pageRank_nxt) ;

}
void dynamicBatchPR_add(graph& g,float beta,float delta,int maxIter,
  float* pageRank,bool* modified)
{
  float* pageRank_nxt=new float[g.num_nodes()];
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    #pragma omp parallel for reduction(+ : diff)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true )
        {
        float sum = 0.000000;
        for (edge v_inedge : g.getInNeighbors(v)) 
        {int nbr = v_inedge.destination ;
          sum = sum + pageRank[nbr] / g.getOutDegree(nbr);
        }
        float val = (1 - delta) / g.num_nodes() + delta * sum;
        diff = diff + val - pageRank[v];
        pageRank_nxt[v] = val;
      }
    }
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true )
        {
        pageRank[v] = pageRank_nxt[v];
      }
    }
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
  free(pageRank_nxt) ;

}
void dynamicBatchPR_del(graph& g,float beta,float delta,int maxIter,
  float* pageRank,bool* modified)
{
  float* pageRank_nxt=new float[g.num_nodes()];
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    #pragma omp parallel for reduction(+ : diff)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true )
        {
        float sum = 0.000000;
        for (edge v_inedge : g.getInNeighbors(v)) 
        {int nbr = v_inedge.destination ;
          sum = sum + pageRank[nbr] / g.getOutDegree(nbr);
        }
        float val = (1 - delta) / g.num_nodes() + delta * sum;
        diff = diff + val - pageRank[v];
        pageRank_nxt[v] = val;
      }
    }
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true )
        {
        pageRank[v] = pageRank_nxt[v];
      }
    }
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
  free(pageRank_nxt) ;

}
void DynPR(graph& g,float beta,float delta,int maxIter,
  float* pageRank,std::vector<update>&  updateBatch,int batchSize)
{
  staticPR(g,beta,delta,maxIter,pageRank);

  int batchSize = batchSize;
  int batchElements = 0;
  for( int updateIndex = 0 ; updateIndex < updateBatch.size() ; updateIndex += batchSize){
    if((updateIndex + batchSize) > updateBatch.size())
    {
      batchElements = updateBatch.size() - updateIndex ;
    }
    else
    batchElements = batchSize ;
    bool* modified=new bool[g.num_nodes()];
    bool* modified_add=new bool[g.num_nodes()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      modified[t] = false;
      modified_add[t] = false;
    }
    std::vector<update> deleteBatch = g.getDeletesFromBatch(updateIndex, batchSize, updateBatch);
    std::vector<update> addBatch = g.getAddsFromBatch(updateIndex, batchSize, updateBatch);
    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'd')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        modified[dest] = true;

      }
    }
    g.propagateNodeFlags(modified);

    g.updateCSRDel(updateBatch, updateIndex, batchElements);

    dynamicBatchPR_del(g,beta,delta,maxIter,pageRank,modified);

    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'a')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        modified_add[dest] = true;

      }
    }
    g.propagateNodeFlags(modified_add);

    g.updateCSRAdd(updateBatch, updateIndex, batchElements);

    dynamicBatchPR_add(g,beta,delta,maxIter,pageRank,modified_add);

    free(modified) ;
    free(modified_add) ;
    deleteBatch.clear();
    addBatch.clear();

  }

}
