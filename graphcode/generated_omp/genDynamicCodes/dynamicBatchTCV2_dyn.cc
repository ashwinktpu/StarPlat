#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

auto staticTC(graph& g)
{
  long triangle_count = 0;
  #pragma omp parallel for reduction(+ : triangle_count)
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
    {int u = g.edgeList[edge] ;
      if (u < v )
        {
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int w = g.edgeList[edge] ;
          if (w > v )
            {
            if (g.check_if_nbr(u, w) )
              {
              triangle_count = triangle_count + 1;
            }
          }
        }
      }
    }
  }
  return triangle_count;

}
auto dynamicBatchTCV2_add(graph& g,int triangle_countSent,bool* modified,std::vector<update>&  addBatch
)
{
  long triangle_count = triangle_countSent;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;
  #pragma omp parallel for reduction(+ : count1,count2,count3)
  for(int i = 0 ; i < addBatch.size() ; i++)
  {
    update update = addBatch[i];
    int v1 = update.source;
    int v2 = update.destination;
    for (edge v1_edge : g.getNeighbors(v1)) 
    {int v3 = v1_edge.destination ;
      if (v3 != v2 && v3 != v1 && v1 != v2 )
        {
        edge e1 = v1_edge;
        int newEdge = 1;
        bool isTriangle = false;
        if (modified[e1.id] )
          newEdge = newEdge + 1;
        if (g.check_if_nbr(v2, v3) )
          {
          edge e2 = g.getEdge(v2,v3);
          isTriangle = true;
          if (modified[e2.id] )
            newEdge = newEdge + 1;
        }
        if (isTriangle )
          {
          if (newEdge == 1 )
            {
            count1 = count1 + 1;
          }
          else
          if (newEdge == 2 )
            {
            count2 = count2 + 1;
          }
          else
          {
            count3 = count3 + 1;
          }
        }
      }
    }
  }
  triangle_count = triangle_count + (count1 / 2 + count2 / 4 + count3 / 6);
  return triangle_count;

}
auto dynamicBatchTCV2_del(graph& g,int triangle_countSent,bool* modified,std::vector<update>&  deleteBatch
)
{
  long triangle_count = triangle_countSent;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;
  #pragma omp parallel for reduction(+ : count1,count2,count3)
  for(int i = 0 ; i < deleteBatch.size() ; i++)
  {
    update update = deleteBatch[i];
    int v1 = update.source;
    int v2 = update.destination;
    for (edge v1_edge : g.getNeighbors(v1)) 
    {int v3 = v1_edge.destination ;
      if (v3 != v2 && v3 != v1 && v1 != v2 )
        {
        edge e1 = v1_edge;
        int newEdge = 1;
        bool isTriangle = false;
        if (modified[e1.id] )
          newEdge = newEdge + 1;
        if (g.check_if_nbr(v2, v3) )
          {
          edge e2 = g.getEdge(v2,v3);
          isTriangle = true;
          if (modified[e2.id] )
            newEdge = newEdge + 1;
        }
        if (isTriangle )
          {
          if (newEdge == 1 )
            {
            count1 = count1 + 1;
          }
          else
          if (newEdge == 2 )
            {
            count2 = count2 + 1;
          }
          else
          {
            count3 = count3 + 1;
          }
        }
      }
    }
  }
  triangle_count = triangle_count - (count1 / 2 + count2 / 4 + count3 / 6);
  return triangle_count;

}
void DynTC(graph& g,std::vector<update>&  updateBatch,int batchSize)
{
  int triangleCount = staticTC(g);
  int batchSize = batchSize;
  int batchElements = 0;
  for( int updateIndex = 0 ; updateIndex < updateBatch.size() ; updateIndex += batchSize){
    if((updateIndex + batchSize) > updateBatch.size())
    {
      batchElements = updateBatch.size() - updateIndex ;
    }
    else
    batchElements = batchSize ;
    bool* modified_del=new bool[g.num_edges()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_edges(); t ++) 
    {
      modified_del[t] = false;
    }
    std::vector<update> deleteBatch = g.getDeletesFromBatch(updateIndex, batchSize, updateBatch);
    std::vector<update> addBatch = g.getAddsFromBatch(updateIndex, batchSize, updateBatch);
    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'd')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        for (edge src_edge : g.getNeighbors(src)) 
        {int nbr = src_edge.destination ;
          edge e = src_edge;
          if (nbr == dest )
            modified_del[e.id] = true;
        }

      }
    }
    triangleCount = dynamicBatchTCV2_del(g,triangleCount,modified_del,deleteBatch);
    g.updateCSRDel(updateBatch, updateIndex, batchElements);

    g.updateCSRAdd(updateBatch, updateIndex, batchElements);

    bool* modified_add=new bool[g.num_edges()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_edges(); t ++) 
    {
      modified_add[t] = false;
    }
    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'a')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        for (edge src_edge : g.getNeighbors(src)) 
        {int nbr = src_edge.destination ;
          edge e = src_edge;
          if (nbr == dest )
            modified_add[e.id] = true;
        }

      }
    }
    triangleCount = dynamicBatchTCV2_add(g,triangleCount,modified_add,addBatch);
    free(modified_del) ;
    deleteBatch.clear();
    addBatch.clear();
    free(modified_add) ;

  }

}