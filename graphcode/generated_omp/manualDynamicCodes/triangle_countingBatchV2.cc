#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include<cmath>
#include"../graph.hpp"



bool isModified(std::vector<update>& updates, int src, int dest)
{
  for(int i=0 ; i<updates.size() ; i++)
     {
       update u = updates[i] ;
       if(u.source == src && u.destination == dest)
          return true;
     }

     return false;
}

void serialUpdateTC(int numVertices, const std::vector<std::vector<int>> &adjList, 
                   const std::map<std::pair<int,int>, int> &edgeWeights)
  {
     long triangle_count = 0;

  for (int v = 0; v < numVertices; v++) 
  {
    for (int u : adjList[v]) 
    { 
      if (u < v )
      {
        for (int w : adjList[v] ) 
         { 
          if (w > v )
          {
            if (std::find(adjList[u].begin(), adjList[u].end(), w)!=adjList[u].end())
            {
             // printf("u %d v %d w %d \n",u, v, w);  
              triangle_count+=1;
            }
          }
        }
      }
    }
  }

 printf("Triangle Count %d\n",triangle_count);




   
    
  }  

void  Compute_TC1(graph& g)
 {
   long triangle_count = 0;

  #pragma omp parallel for num_threads(4) reduction(+ : triangle_count)
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    for (edge e:g.getNeighbors(v)) 
    { 
      int u = e.destination;
      if (u < v )
      {
        for (edge e1:g.getNeighbors(v)) 
         { 
          int w = e1.destination;
          if (w > v )
          {
            if (g.check_if_nbr(u, w) )
            {
             // printf("u %d v %d w %d \n",u, v, w);  
              triangle_count+=1;
            }
          }
        }
      }
    }


 }                 

   printf("True Triangle Count %d\n",triangle_count);
 }
int Compute_TC(graph& g)
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
             // printf("u %d v %d w %d \n",u, v, w);  
              triangle_count+=1;
            }
          }
        }
      }
    }
  }

 //printf("Triangle Count %d\n",triangle_count);

 return triangle_count;

}

int  Compute_dynamicTCAdd(graph& g, int triangleC, bool* modified, std::vector<update>& addBatch)
 {
   
  long triangle_count = triangleC;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;

    
  #pragma omp parallel for num_threads(4) reduction(+ : count1, count2, count3)
  for(int i = 0 ; i < addBatch.size() ; i++)
     {
      // printf("Inside this %d\n");
       update u = addBatch[i] ;
       int v1 = addBatch[i].source;
       int v2 = addBatch[i].destination;

       for(edge e1:g.getNeighbors(v1))
          {   
              int newEdge = 1;
              bool isTriangle = false;
              int v3 = e1.destination ;

              if(v3==v2)
                continue;


              if(g.check_if_nbr(v2,v3))
                 {
                     edge e2 = g.getEdge(v2, v3);
                     isTriangle = true;

                     if(modified[e1.id] || modified[e2.id])
                       {
                         newEdge = newEdge + modified[e1.id] + modified[e2.id];
                       }
                     
                 }   


          if(isTriangle)
           {
          if(newEdge == 1)
             { 
               count1 += 1;
             }
           else if(newEdge == 2)
               {
                 count2 += 1;   
               } 
           else
              {
                 count3 += 1;
              }   
            }     

         }
        
               

     }
//        printf("triangle_count1 %d\n",triangle_count);
        triangle_count = triangle_count + count1/2 + count2/4 + count3/6;
 //       printf("updated %d\n",count1/2+count2/4+count3/6);

        return triangle_count;
 }

 int Compute_dynamicTCDel(graph& g, int triangleC, bool* modified, std::vector<update>& deleteBatch)
   {
     long triangle_count = triangleC;
     int count1 = 0;
     int count2 = 0;
     int count3 = 0;

    
    #pragma omp parallel for num_threads(4)  reduction(+ : count1, count2, count3)
    for(int i = 0 ; i < deleteBatch.size() ; i++)
     {
       update u = deleteBatch[i] ;
       int v1 = deleteBatch[i].source;
       int v2 = deleteBatch[i].destination;
  
       
       for(edge e1:g.getNeighbors(v1))
          {   
              
              int oldEdge = 1;
              bool isTriangle = false;
              int v3 = e1.destination ;
              

               if(v3 == v2)
                  continue;

              if(g.check_if_nbr(v2,v3))
                 {
                     edge e2 = g.getEdge(v2, v3);
                     isTriangle = true;
                     
                     if(modified[e1.id] || modified[e2.id])
                       {
                         oldEdge = oldEdge + modified[e1.id] + modified[e2.id];
                       }

                        //printf("v1 %d v2 %d v3 %d oldedge %d i %d modified_2 %d modified_3 %d \n ",v1,v2,v3,oldEdge,i,modified[e1.id],modified[e2.id]);   
                               
                 } 


          if(isTriangle)
          {
          if(oldEdge == 1)
             { 
               count1 += 1;
             }
           else if(oldEdge == 2)
               {
                  count2 += 1;
               } 
           else
              {
                count3 += 1;
              }    
          }      

       }
          
    }
        
   //     printf("count1 %d\n",count1);
    //    printf("count2 %d\n",count2);
      //  printf("count3 %d\n",count3);
        triangle_count = triangle_count - (count1/2 + count2/4 + count3/6);

        return triangle_count; 
   
  }

int main(int argc, char** argv)
{

  graph G("../dataRecords/roadNetwork/GermanyRoadud.txt");

  G.parseGraph();

  //printf("node Number %d edge Number %d\n", G.num_nodes(), G.num_edges());
  bool* modified_add ;
  bool* modified_del;
   std::vector<std::vector<int>> adjList(G.num_nodes());
  std::map<std::pair<int,int>, int> edgeWeights;

  
    for(int i=0; i<G.num_nodes(); i++) {
       for(int j = G.indexofNodes[i] ; j<G.indexofNodes[i+1] ; j++)
       {
        int weight = 1;
        adjList[i].push_back(G.edgeList[j]);
        edgeWeights[{i,G.edgeList[j]}] = weight;
       }
    }

  int triangleC =  Compute_TC(G);
  printf("Initial Triangle %d\n",triangleC);
 // int batchSize = 1000;
  int elements = 0;

  std::vector<update> updateEdges = G.parseUpdates("../dataRecords/update_germanyRoadNet.txt");
 //G.parseUpdates("../dataRecords/sampleUpdates/update_usaSmall.txt");
  
  std::vector<update> deleteBatch;
  std::vector<update> addBatch;

  

  float percent = atof(argv[1]); 
  int updateSize = ceil(percent*G.num_edges());//updateEdges.size();
  printf("UPDATE SIZE %d\n",updateSize);
  int batchSize = 1000000;

  double startTime=omp_get_wtime();

   for(int k=0;k<updateSize/*updateEdges.size()*/;k+=batchSize)
    {
       
        // printf("k val %d\n",k);
        if((k+batchSize) > updateSize/*updateEdges.size()*/)
         {
          elements = /*updateEdges.size()*/ updateSize - k ;
         
         
        }
        else
          elements = batchSize;


    /* Batchwise deletion and decremental call */
    
      printf("elementsize %d\n",elements);
         
     modified_del = new bool[G.num_edges()];
     for(int i = 0; i < G.num_edges() ; i++)
       {
         modified_del[i] = false;
       }

      for(int i=k;i<(k+batchSize) && i<updateSize /*updateEdges.size()*/;i++)
       {
         if(updateEdges[i].type=='d')
           {
       
               update u = updateEdges[i];
               int src = u.source;
               int dest = u.destination;
               for (edge e: G.getNeighbors(src))
                   {
                      
                      if(e.destination == dest)
                        {
                         modified_del[e.id] = true;
                        }
                      
                   }

                   deleteBatch.push_back(u);
           }

      }  
    
    triangleC = Compute_dynamicTCDel(G, triangleC, modified_del, deleteBatch);
    G.updateCSR_Del(updateEdges, k, elements);

    /*int deleteSize = deleteBatch.size();
    updateSize = deleteSize;
    batchSize = updateSize;*/
     deleteBatch.clear();

    printf("triangle Count Del %d\n",triangleC);

   
  /* Batchwise addition and incremental call */
   

   G.updateCSR_Add(updateEdges, k, elements);

    modified_add = new bool[G.num_edges()];

    for(int i = 0; i < G.num_edges() ; i++)
       {
         modified_add[i] = false;
       }

      for(int i=k;i<(k+batchSize) && i<updateSize ;i++)
        {
         if(updateEdges[i].type=='a')
           {
              //printf("Inside this check %d\n");
               update u = updateEdges[i];
               int src = u.source;
               int dest = u.destination;
               for (edge e: G.getNeighbors(src))
                   {
                      if(e.destination == dest)
                        {
                         modified_add[e.id] = true;
                        }
                      
                   }             
              
              addBatch.push_back(u);
           }

      }
 
    triangleC = Compute_dynamicTCAdd(G, triangleC, modified_add, addBatch);
    addBatch.clear();
    free(modified_add);
    free(modified_del);
    printf("triangle Count Add %d\n",triangleC);


 }

  double endTime=omp_get_wtime();
  printf("RunTime : %f\n",endTime-startTime);

  printf("triangle Count %d\n",triangleC);

  Compute_TC1(G);
/*  updateSize = deleteBatch.size();
  batchSize = updateSize;*/

  for(int k=0;k<updateSize;k++)
    { 
         int u =updateEdges[k].source;
         int v = updateEdges[k].destination;
         int weight = updateEdges[k].weight;

         if(updateEdges[k].type=='d')
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
           else if(updateEdges[k].type=='a')
            {
               adjList[u].push_back(v);
               edgeWeights[{u,v}] = weight;
            }         
      

    }

    serialUpdateTC(G.num_nodes(), adjList, edgeWeights);







 
 

}
