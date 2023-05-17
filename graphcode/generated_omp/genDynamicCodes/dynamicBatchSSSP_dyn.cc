#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

void staticSSSP(graph& g,int* dist,int* parent,int* weight,
  int src)
{
  omp_lock_t* lock = (omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

  for(int v = 0; v<g.num_nodes(); v++)
    omp_init_lock(&lock[v]);

  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX;
    modified[t] = false;
    modified_nxt[t] = false;
    parent[t] = -1;
  }
  modified[src] = true;
  dist[src] = 0;
  bool finished = false;
  while ( !finished )
  {
    finished = true;
    #pragma omp parallel
    {
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        if (modified[v] == true )
          {
          for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
          {int nbr = g.edgeList[edge] ;
            int e = edge;
             int dist_new = dist[v] + weight[e];
            bool modified_new = true;
            int parent_new = v;
            if(dist[nbr] > dist_new)
            {
              int oldValue = dist[nbr];
              omp_set_lock(&lock[nbr]);
              if(dist[nbr] > dist_new)
              {
                dist[nbr] = dist_new;
                parent[nbr] = parent_new;
              }
              omp_unset_lock(&lock[nbr]);
              if( oldValue > dist[nbr])
              {modified_nxt[nbr] = modified_new;
                finished = false ;
              }
            }
          }
        }
      }
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      { modified[v] =  modified_nxt[v] ;
        modified_nxt[v] = false ;
      }
    }
  }
  free(modified) ;

}
void dynamicBatchSSSP_add(graph& g,int* dist,int* parent,bool* modified
)
{
  omp_lock_t* lock = (omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

  for(int v = 0; v<g.num_nodes(); v++)
    omp_init_lock(&lock[v]);

  bool finished = false;
  while ( !finished )
  {
    finished = true;
    #pragma omp parallel
    {
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        if (modified[v] == true )
          {
          for (edge v_edge : g.getNeighbors(v)) 
          {int nbr = v_edge.destination ;
            edge e = v_edge;
             int dist_new = dist[v] + e.weight;
            bool modified_new = true;
            int parent_new = v;
            if(dist[nbr] > dist_new)
            {
              int oldValue = dist[nbr];
              omp_set_lock(&lock[nbr]);
              if(dist[nbr] > dist_new)
              {
                dist[nbr] = dist_new;
                parent[nbr] = parent_new;
              }
              omp_unset_lock(&lock[nbr]);
              if( oldValue > dist[nbr])
              {modified_nxt[nbr] = modified_new;
                finished = false ;
              }
            }
          }
        }
      }
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      { modified[v] =  modified_nxt[v] ;
        modified_nxt[v] = false ;
      }
    }
  }

}
void dynamicBatchSSSP_del(graph& g,int* dist,int* parent,bool* modified
)
{
  bool finished = false;
  while (!finished ){
    finished = true;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == false )
        {
        int parent_v = parent[v];
        if (parent_v > -1 && modified[parent_v] )
          {
          dist[v] = INT_MAX / 2;
          modified[v] = true;
          parent[v] = -1;
          finished = false;
        }
      }
    }
  }
  finished = false;
  while (!finished ){
    finished = true;
    #pragma omp parallel for
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      if (modified[v] == true )
        {
        for (edge v_inedge : g.getInNeighbors(v)) 
        {int nbr = v_inedge.destination ;
          edge e = v_inedge;
          if (dist[v] > dist[nbr] + e.weight )
            {
            dist[v] = dist[nbr] + e.weight;
            parent[v] = nbr;
            finished = false;
          }
        }
      }
    }
  }

}
void DynSSSP(graph& g,int* dist,int* parent,int* weight,
  std::vector<update>&  updateBatch,int batchSize,int src)
{
  staticSSSP(g,dist,parent,weight,src);

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
        if (parent[dest] == src )
          {
          dist[dest] = INT_MAX / 2;
          modified[dest] = true;
          parent[dest] = -1;
        }

      }
    }
    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'a')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        if (dist[dest] > dist[src] + 1 )
          {
          modified_add[dest] = true;
          modified_add[src] = true;
        }

      }
    }
    g.updateCSRDel(updateBatch, updateIndex, batchElements);

    dynamicBatchSSSP_del(g,dist,parent,modified,deleteBatch);

    g.updateCSRAdd(updateBatch, updateIndex, batchElements);

    dynamicBatchSSSP_add(g,dist,parent,modified_add,addBatch);

    free(modified) ;
    free(modified_add) ;
    deleteBatch.clear();
    addBatch.clear();

  }

}