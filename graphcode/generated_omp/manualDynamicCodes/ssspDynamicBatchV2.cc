#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"


long long dijkstra(int numVertices, const std::vector<std::vector<int>> &adjList, 
                   const std::map<std::pair<int,int>, int> &edgeWeights, int *distances, int* parent, int source=0) {
    for(int i=0; i<numVertices; i++) distances[i] = INT_MAX/2;
    distances[source] = 0;
    for(int i=0;i<numVertices;i++)
        parent[i] = -1;

    std::set<std::pair<int,int>> active_vertices;
    active_vertices.insert({0, source});

    printf("INSIDE DIJKSTRA FOR CORRECTNESS CHECK!\n");

    while(!active_vertices.empty()) {
        int u = active_vertices.begin()->second;
        active_vertices.erase(active_vertices.begin());
        for(int v : adjList[u]) {
            if(edgeWeights.at({u,v}) == INT_MAX) continue;
            int newDist = distances[u] + edgeWeights.at({u,v});
            if(newDist < distances[v]) {
                active_vertices.erase({distances[v], v});
                distances[v] = newDist;
                parent[v] = u;
                active_vertices.insert({newDist, v});
            }
        }
    }

    long long sum = 0;
    for(int i=0; i<numVertices; i++) sum += distances[i];
    return sum;
}

bool checkDistances(int *distances1, int *distances2, int numVertices) {
    for(int i=0; i<numVertices; i++) {
        if(distances1[i] != distances2[i]) return false;
    }
    return true;
}

void Compute_SSSP(graph& g,int* dist,int* parent,int* weight, int src)
{

  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  omp_lock_t* lock=(omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));


  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    dist[t] = INT_MAX/2;
    parent[t] = -1;
    modified[t] = false;
    modified_nxt[t] = false;
    omp_init_lock(&lock[t]);
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
        if (modified[v] == true ){
          for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
          { 

            int nbr = g.edgeList[edge] ;
            int e = edge;
            int dist_new = dist[v] + weight[e];
            bool modified_new = true;
            
            if(dist[nbr] > dist_new)
            {

            int oldValue = dist[nbr];

            omp_set_lock(&lock[nbr]);
            if(dist[nbr] > dist_new)
            {
              
              dist[nbr] = dist_new;
              parent[nbr] = v;
            }
            omp_unset_lock(&lock[nbr]);


              if(oldValue > dist[nbr])
              {
                modified_nxt[nbr] = modified_new;
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

void Compute_dynamicSSSPAdd(graph& g, int* dist, int* parent, bool* modified)
{
    bool* modified_nxt=new bool[g.num_nodes()];
    omp_lock_t* lock=(omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

    for(int i=0; i< g.num_nodes() ; i++)
    {
        modified_nxt[i] = false;
        omp_init_lock(&lock[i]);
    }

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
          for (edge e: g.getNeighbors(v)) 
          {  
            int nbr = e.destination;
            int weight = e.weight;
            int dist_new = dist[v] + weight;
            bool modified_new = true;

            if(dist[nbr] > dist_new)
            {
              int oldValue = dist[nbr];
             // atomicMin(&dist[nbr],dist_new);
               omp_set_lock(&lock[nbr]);
               if (dist[nbr] > dist_new)
                {
              
                  dist[nbr] = dist_new;
                  parent[nbr] = v;
                 }
                omp_unset_lock(&lock[nbr]);
              if(oldValue > dist[nbr])
              {
                modified_nxt[nbr] = modified_new;
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

void Compute_dynamicSSSP(graph& g, int* dist,int* parent, bool* modified)
 {
   
  // bool* modified=new bool[g.num_nodes()];
   bool* modified_nxt=new bool[g.num_nodes()];
   omp_lock_t* lock=(omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

     #pragma omp parallel for
     for (int t = 0; t < g.num_nodes(); t ++) 
      { 
         omp_init_lock(&lock[t]);

      }
  
     #pragma omp parallel for
     for (int t = 0; t < g.num_nodes(); t ++) 
      { 
         modified_nxt[t] = false;
      }

    
    bool finished = false;

    while ( !finished )
    {
    finished = true;
    
      #pragma omp parallel for
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        if (!modified[v])
         {
          
             int parent_v = parent[v];
             if(parent_v>-1 && modified[parent_v])
               {  
                 /* include parent = -1 */
                   dist[v] = INT_MAX/2;
                   modified[v] = true;
                   finished = false;
                   
               }

         }
      }  

  } 




   finished = false;

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
              
          for (edge e: g.getInNeighbors(v)) 
          {
             int nbr = e.destination;
            // printf("v %d nbr %d\n",v,nbr);
             int weight = e.weight;
             int dist_new = dist[nbr] + weight;
              bool modified_new = true;
              if(dist[v] > dist_new)
               { 
                 int oldValue = dist[v];
                 dist[v] = dist_new;
                 parent[v] = nbr;
                 finished = false;
              }
          }

        }
     }
      
        
    }
  }     

 }


int main()
{

  graph G("../dataRecords/socialNetwork/soc-LiveJournal1.txt");
  G.parseGraph();
  int* dist = new int[G.num_nodes()];
  int* parent = new int[G.num_nodes()];
  bool* modified = new bool[G.num_nodes()];
  bool* modified_add = new bool[G.num_nodes()];
  int* edgeLen = G.getEdgeLen();
   std::vector<std::vector<int>> adjList(G.num_nodes());
    std::map<std::pair<int,int>, int> edgeWeights;
    int* distance = new int[G.num_nodes()];
    int* parent_temp = new int[G.num_nodes()];

     for(int i=0; i<G.num_nodes(); i++) {
       for(int j = G.indexofNodes[i] ; j<G.indexofNodes[i+1] ; j++)
       {
        int weight = 1;
        adjList[i].push_back(G.edgeList[j]);
        edgeWeights[{i,G.edgeList[j]}] = weight;
       }
    }

  Compute_SSSP(G , dist , parent, edgeLen, 0);
 
 
  std::vector<update> updateEdges = G.parseUpdates("../dataRecords/update_ljsmall.txt");
  int batchSize = 1000;
 
 
  int elements = 0;
  for(int k=0;k<updateEdges.size();k+=batchSize)
    { 
       
       printf("Batch No : %d\n",k);
      if((k+batchSize) > updateEdges.size())
        {
          elements = updateEdges.size() - k ;

         
         
        }
        else
          elements = batchSize;


     #pragma omp parallel for
     for (int t = 0; t < G.num_nodes(); t ++) 
      { 
         modified[t] = false;
         modified_add[t] = false;
  
      }



    for(int i=k;i<(k+batchSize) && i<updateEdges.size();i++)
      {
         if(updateEdges[i].type=='d')
           {
               update u = updateEdges[i];
               int src = u.source;
               int dest = u.destination;

         if(parent[dest]==src)
            {
             dist[dest] = INT_MAX/2;
             modified[dest] = true;  
             parent[dest] = -1;
           }

           } 

          
      } 

      

   for(int i=k;i<(k+batchSize) && i<updateEdges.size();i++)
      {
         if(updateEdges[i].type=='a')
           {
       
               update u = updateEdges[i];
               int src = u.source;
               int dest = u.destination;
               if(dist[dest]>dist[src] + 1)
                {
                    modified_add[dest] = true;
                    modified_add[src] = true;
                  
                }
             }

      }

   G.updateCSR_Del(updateEdges,k,elements);    
   
   Compute_dynamicSSSP(G,dist,parent, modified);

   G.updateCSR_Add(updateEdges,k,elements);
 
   Compute_dynamicSSSPAdd(G,dist, parent, modified_add);

  }
  
    for(int i=0;i<G.num_nodes();i++)
    {
        printf("i %d dist %d \n",i,dist[i]);
    }
/*
 
  for(int k=0;k<updateEdges.size();k+=batchSize)
    { 

     for(int i=k;i<(k+batchSize) && i<updateEdges.size();i++)
      {
         int u = updateEdges[i].source;
         int v = updateEdges[i].destination;
         int weight = updateEdges[i].weight;

         if(updateEdges[i].type=='d')
           {
              
             std::vector<int> temp_vec;
             for(int l=0;l<adjList[u].size();l++)
                 {
                   if(adjList[u][l]!=v)
                      temp_vec.push_back(adjList[u][l]);
                 }
             adjList[u].clear();
             
             for(int l=0; l<temp_vec.size(); l++)
                adjList[u].push_back(temp_vec[l]);

             temp_vec.clear();  

             edgeWeights.erase({u,v});

           } 
           else if(updateEdges[i].type=='a')
            {
               adjList[u].push_back(v);
               edgeWeights[{u,v}] = weight;
            }         
      } 

    }


   dijkstra(G.num_nodes(), adjList, edgeWeights, distance, parent_temp, 0);

    for(int i=0;i<G.num_nodes();i++)
    {
        printf("i %d dist %d \n",i,distance[i]);
    }
   /* if(checkDistances(dist, distance, G.num_nodes()) == false) {
        printf("\nFinal: Check failed..!!!\n");
        return 0;
    }*/
       

 
 
  

}
