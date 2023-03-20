#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include<queue>
#include<stack>
#include"../graph.hpp"
#include "../randomGeneratorUtil.h"


void staticBC(graph& g, float *BC,std::set<int>& sourceSet, std::map<int,std::vector<int>>& bfsDistMem, std::map<int,std::vector<float>>& deltaMem, std::map<int,std::vector<double>>& sigmaMem, std::map<int,int*>& parentMem)
 {

  const int node_count=g.num_nodes();

  #pragma omp parallel for
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
    int* parent = new int[g.num_nodes()];

   

    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      delta[t] = 0;
      bfsDist[t] = INT_MAX;
      parent[t] = -1;
    }

    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      sigma[t] = 0;
    }

    bfsDist[src] = 0;
    sigma[src] = 1;
    std::vector<std::vector<int>> levelNodes(g.num_nodes()) ;
    std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
    std::vector<int>  levelCount(g.num_nodes()) ;

    int phase = 0 ;
    levelNodes[phase].push_back(src) ;
    int  bfsCount = 1 ;
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
            if(bfsDist[nbr] == INT_MAX)
            {
          dnbr = __sync_val_compare_and_swap(&bfsDist[nbr],INT_MAX,phase+1);
          if (dnbr == INT_MAX)
          {
               int num_thread=omp_get_thread_num();
               levelNodes_later[num_thread].push_back(nbr);
               parent[nbr] = v;

          }
         }
        }
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {
          int w = g.edgeList[edge] ;
          if(bfsDist[w]==bfsDist[v]+1)
          {
            #pragma omp atomic
            sigma[w] = sigma[w] + sigma[v];

           // printf("w %d v %d sigma_w %f\n",w,v,sigma[w]);
          }
        }
      }
         phase = phase + 1 ;
       for(int i=0;i<omp_get_max_threads();i++)
            {
           levelNodes[phase].insert(levelNodes[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
           bfsCount=bfsCount+levelNodes_later[i].size();
            levelNodes_later[i].clear();
           }
        levelCount[phase] = bfsCount ;

    }
    phase = phase - 1 ;
    while (phase >= 0)
    {
      #pragma omp parallel for
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int v = levelNodes[phase][l] ;
    //    float val=0.0;
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {
          int w = g.edgeList[edge] ;
           
          if(bfsDist[w]==bfsDist[v]+1)
          {
             
             delta[v] = delta[v] + (sigma[v] / sigma[w]) *( 1 + delta[w]);
             /*printf("v %d w %d delta_v %f src %d\n",v,w,delta[v],src);
             printf("sigma %f of v %d sigma %f of w %d delta %f of w %d \n",sigma[v],v,sigma[w],w,delta[w],w);*/
          }
        }
        
        if(v != src)
           BC[v] = BC[v] + delta[v];
        
      }

      phase=phase-1;
    }

    for(int i = 0 ; i<g.num_nodes() ; i++)
       {
           bfsDistMem[src].push_back(bfsDist[i]);
           sigmaMem[src].push_back(sigma[i]);
           deltaMem[src].push_back(delta[i]);
           
       }

       parentMem[src] = parent;
  }

}

void oneLevelDelProcessing(graph& g, float* BC, std::vector<std::vector<int>>& levelQueues,std::map<int,std::vector<int>>& bfsDistMem,
                         std::map<int,std::vector<double>>& sigmaMem, std::map<int,std::vector<float>>& deltaMem, int* bfsDist_new, double* sigma_new, float* delta_new,
                         int src,  int* direction, std::vector<vector<bool>>& modified, std::vector<int>& delEdgeIds, std::map<int,int*>& parentMem, bool* isChanged)
{
    

   /* 1st Step: calculate the correct shortest distance values */
    
   bool finished = false;
    omp_lock_t* lock = (omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

    #pragma omp parallel for
     for (int t = 0; t < g.num_nodes(); t ++) 
      {
            omp_init_lock(&lock[t]);
      }

   while ( !finished )
    {
    finished = true;
    #pragma omp parallel
    {
      #pragma omp for
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        if (isChanged[v] == true )
         {       
          for (edge e: g.getInNeighbors(v)) 
          {
            if(modified[src][e.id])
           {   
           
             int nbr = e.destination;
             int weight = e.weight;
            if(bfsDist_new[nbr] != INT_MAX/2)
             {
             int dist_new = bfsDist_new[nbr] + weight;
          //    printf("entered for node %d nbr %d old_dist %d dist_new %d\n",v,e.destination,bfsDist_new[v],dist_new);
              bool modified_new = true;
              if(bfsDist_new[v] > dist_new)
               { 
                 int oldValue = bfsDist_new[v];
                 bfsDist_new[v] = dist_new;
            //     printf("node %d bfsdist_new %d\n",v,bfsDist_new[v]);
                 parentMem[src][v] = nbr;
                 finished = false;
              }
           }
           }
          }

        }
      }
    }
  }

   std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
   std::vector<int>  levelCount(g.num_nodes(),0) ;
   int phase = 0;
   for(int t = 0 ; t<g.num_nodes() ; t++)
     {
         if(isChanged[t])
           {
               
               if(bfsDist_new[t] != INT_MAX/2)
                {
                
                  levelQueues[bfsDist_new[t]].push_back(t);
                  levelCount[bfsDist_new[t]] += 1;
                  direction[t] = 1;
                }
            else
             {             
                sigma_new[t] = 0;

                for(edge e: g.getInNeighbors(t))
                {
                 if(modified[src][e.id] || std::find(delEdgeIds.begin(),delEdgeIds.end(),e.id)!=delEdgeIds.end())
                {
               if(bfsDistMem[src][e.destination] + 1 == bfsDistMem[src][t])
                 {
                   if(bfsDist_new[e.destination] != INT_MAX/2 && direction[e.destination] == 0)
                    { 
                   levelQueues[bfsDist_new[e.destination]].push_back(e.destination);
                   levelCount[bfsDist_new[e.destination]] += 1;
                   sigma_new[e.destination] = 0;
                   direction[e.destination] = 2;
                    }
                 }
                }
              }
          }
       }
     }



     while(phase < g.num_nodes())
       {
          if(!levelQueues[phase].empty())
            {

            for(int i = 0 ; i<levelCount[phase] ; i++)
            {       
             int v = levelQueues[phase][i];
             direction[v] = 1;
             sigma_new[v] = 0;     
             

            for(edge e:g.getInOutNbrs(v))
             {                
                int w = e.destination ;                
                if(modified[src][e.id])
                  {
                  if(((bfsDist_new[w] == bfsDist_new[v] + 1) || (bfsDistMem[src][w] == bfsDistMem[src][v] + 1)) && direction[w] == 0 && e.dir == 1)
                   {  
                
                    if(direction[w] == 0 )
                    {
                      direction[w] = 1;
                      levelQueues[bfsDist_new[w]].push_back(w);
                      levelCount[bfsDist_new[w]]+=1;
                      sigma_new[w] = 0; 
                    } 

                   }       
                   if((bfsDist_new[w] == bfsDist_new[v]) && (bfsDistMem[src][w] != bfsDistMem[src][v]))
                        {
                          if(direction[w] == 0 )
                             {
                               direction[w] = 1;
                               levelQueues[bfsDist_new[w]].push_back(w);
                               levelCount[bfsDist_new[w]]+=1; 
                               sigma_new[w] = 0;                        
                             }                      
                        }
                 }
             }
          }
        }  
        
        phase = phase + 1; 
 }   

     phase = 0;

    /* 2nd step: sigma recalculation */ 
    
      while(phase < g.num_nodes())
        {
          if(!levelQueues[phase].empty())
            {
             
             for(int i = 0 ; i<levelCount[phase] ; i++)
             { 
              int v = levelQueues[phase][i];
            //  printf("phase %d v %d\n",phase,v);
              direction[v] = 1;  
               if(v == src)
                 sigma_new[v] = 1;            
              for(edge e:g.getInNeighbors(v))
                {
                  int w = e.destination ;
                  if(modified[src][e.id])
                  {                     
                  // printf("w has val %d for v %d e.id %d e.dir %d bfsdist_w %d bfsdist_v %d\n",w,v,e.id,e.dir,bfsDist_new[w],bfsDist_new[v]);  
                     if((bfsDist_new[w] + 1 == bfsDist_new[v]))
                        {
                    //      printf("old sigma %f of v %d\n",sigma_new[v],v);  
                          sigma_new[v]+=sigma_new[w] ;
                      //   printf("sigma_new %f of v %d\n",sigma_new[v],v);
                        }      
                  }
                }

           }
        }
           phase = phase + 1;
     }

   phase = g.num_nodes() - 1;

   std::vector<std::vector<std::vector<int>>>  levelNodes_back(omp_get_max_threads(),std::vector<std::vector<int>>(g.num_nodes())) ;

   /* 3rd step: delta values accumulation */    

   while (phase >= 0)
    {
      #pragma omp parallel for
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int w = levelQueues[phase][l] ;
        delta_new[w] = deltaMem[src][w];  
      for(edge e: g.getNeighbors(w)) 
       {      
           int v = e.destination ;
          //  printf("w %d v %d \n", w, v);

           if(modified[src][e.id] || std::find(delEdgeIds.begin(),delEdgeIds.end(),e.id)!=delEdgeIds.end())
             {
           if((bfsDist_new[v] > bfsDist_new[w]) ||  (bfsDistMem[src][v] == bfsDistMem[src][w] + 1) /*&& e.dir==0*/)
                 {
                        
                   if(modified[src][e.id]) 
                    {
                      if(sigma_new[v] > 0 && bfsDist_new[v] == bfsDist_new[w] + 1)
                         {                 
                         if(direction[v] == 0)
                            { 
                            delta_new[v] = deltaMem[src][v];   
                            }
                         delta_new[w] += sigma_new[w]*(1 + delta_new[v])/sigma_new[v];
                        // printf("delta_new %f of w %d increase val %f\n",delta_new[w],w, sigma_new[w]*(1 + delta_new[v])/sigma_new[v]);
                        // printf("sigma_new %f of w %d sigma_new %f of v %d delta_new %f of v %d \n",sigma_new[w],w,sigma_new[v],v,delta_new[v],v);
                         }
                    }

                        float alpha = 0;                     
                       if(sigmaMem[src][v] > 0 && (bfsDistMem[src][v] == bfsDistMem[src][w] + 1))
                         {
                          alpha = sigmaMem[src][w]*(1 + deltaMem[src][v])/sigmaMem[src][v];
                        // printf("alpha*** %f of w %d v %d\n",alpha,w,v);
                         }
                                        
                         // printf("delta_new+++ before %f of w %d\n",delta_new[w],w);
                          delta_new[w] = delta_new[w] - alpha;   
                          //printf("delta_new+++ %f of w %d\n",delta_new[w],w);
                 }      
         }
   }

     /* 4th step: in-neighbors accumulation in appriopriate levels */

     for(edge e: g.getInNeighbors(w))
       {
           if(modified[src][e.id] || std::find(delEdgeIds.begin(),delEdgeIds.end(),e.id)!=delEdgeIds.end())
            {
           int v = e.destination;
           if(bfsDist_new[v] < bfsDist_new[w] )
                 {
                    omp_set_lock(&lock[v]);
                    if(direction[v] == 0)
                       {
                         direction[v] = 2; 
                         delta_new[v] = deltaMem[src][v];
                         int num_thread = omp_get_thread_num();
                         levelNodes_back[num_thread][bfsDist_new[v]].push_back(v);
                       }
                      omp_unset_lock(&lock[v]);  
                }

            }

      }     
    //  printf("delta_new  over %f of w %d\n",delta_new[w],w);  
    /*  if(w != src)
          {      
           #pragma omp atomic   
           BC[w] += delta_new[w] - deltaMem[src][w] ;
          }*/

         // printf("this is for node %d BC %f\n",w,BC[w]); 
        
    }
    
    phase = phase-1;

     for(int i=0;i<omp_get_max_threads();i++)
         {  
           for(int j = 0 ; j<=phase ; j++)
            {      
               if(!levelNodes_back[i][j].empty())
                 {
                levelQueues[j].insert(levelQueues[j].end(),levelNodes_back[i][j].begin(), levelNodes_back[i][j].end());
                levelCount[j] = levelCount[j] + levelNodes_back[i][j].size();
                levelNodes_back[i][j].clear();
                }
            }
           
        }

    }

    

}


/* Testing passed */
void oneLevelProcessing(graph& g, float* BC, std::vector<std::vector<int>>& levelQueues, std::queue<int> startQueue, std::map<int,std::vector<int>>& bfsDistMem,
                         std::map<int,std::vector<double>>& sigmaMem, std::map<int,std::vector<float>>& deltaMem, int* bfsDist_new, double* sigma_new, float* delta_new,
                         int src, int v1, int v2, int* direction, std::vector<vector<bool>>& modified)
{

    levelQueues[bfsDistMem[src][v1]].push_back(v1);
    direction[v1] = 1;
    sigma_new[v1] += sigma_new[v2];
    omp_lock_t* lock = (omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

    #pragma omp parallel for
     for (int t = 0; t < g.num_nodes(); t ++) 
      {
            omp_init_lock(&lock[t]);
      }
    //std::vector<std::vector<int>> levelNodes(g.num_nodes()) ;
    std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
    std::vector<int>  levelCount(g.num_nodes()) ;

    int phase = bfsDistMem[src][v1] ;
   // levelNodes[phase].push_back(src) ;
    int  bfsCount = 1 ;
    levelCount[phase] = bfsCount;

    while ( bfsCount > 0 )
    {
      int prev_count = bfsCount ;
      bfsCount = 0 ;
      #pragma omp parallel for 
      for( int l = 0; l < prev_count ; l++)
      {
        int v = levelQueues[phase][l] ;
        for(edge e:g.getNeighbors(v)) 
        {
        if(modified[src][e.id])
          {
          int nbr = e.destination ;
          //int dnbr ;
          if(bfsDist_new[nbr] == bfsDist_new[v] + 1)
            {  
                omp_set_lock(&lock[nbr]);
                if(direction[nbr] == 0)
                   {
                     direction[nbr] = 1;
                     int num_thread = omp_get_thread_num();
                     levelNodes_later[num_thread].push_back(nbr);
                   }
                omp_unset_lock(&lock[nbr]);  
                #pragma omp atomic
                sigma_new[nbr] += sigma_new[v] - sigmaMem[src][v] ;

            }
         }
       }
    }
       
       phase = phase + 1 ;

       for(int i=0;i<omp_get_max_threads();i++)
         {  
           levelQueues[phase].insert(levelQueues[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
           bfsCount=bfsCount+levelNodes_later[i].size();
           levelNodes_later[i].clear();
        }

        levelCount[phase] = bfsCount ;

    }
  
    phase = phase - 1;
     
   while (phase > 0)
    {
      #pragma omp parallel for
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int w = levelQueues[phase][l] ;
    //    float val=0.0;
      for(edge e: g.getInNeighbors(w)) 
       {
          if(modified[src][e.id]) 
          { 
          int v = e.destination ;
           
           if(bfsDist_new[v] < bfsDist_new[w] /*&& e.dir==0*/)
                 {
                    omp_set_lock(&lock[v]); 
                    if(direction[v] == 0)
                       {
                         direction[v] = 2;
                         delta_new[v] = deltaMem[src][v];
                        // levelQueues[level-1].push(v);
                         int num_thread = omp_get_thread_num();
                         levelNodes_later[num_thread].push_back(v);
                        }

                      if(sigma_new[w] > 0)
                         delta_new[v] += sigma_new[v]*(1 + delta_new[w])/sigma_new[w];

                        float alpha = 0; 
                       if(sigmaMem[src][w] > 0)
                         alpha = sigmaMem[src][v]*(1 + deltaMem[src][w])/sigmaMem[src][w];

                        if(direction[v] == 2 && (v!= v2 || w != v1))
                         delta_new[v] -= alpha;    
                    omp_unset_lock(&lock[v]);     

                 }
         }

     }
      
      if(w != src)
          {
           #pragma omp atomic   
           BC[w] += delta_new[w] - deltaMem[src][w] ;
          }
        
    }
    
    phase = phase-1;

     for(int i=0;i<omp_get_max_threads();i++)
         {  
           levelQueues[phase].insert(levelQueues[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
           levelCount[phase] = levelCount[phase] + levelNodes_later[i].size();
           levelNodes_later[i].clear();
           
        }

    }
        

}

/* Testing passed */
void nonAdjacentLevelProcessing(graph& g, float* BC, std::vector<std::vector<int>>& levelQueues, std::queue<int> startQueue, std::map<int,std::vector<int>>& bfsDistMem,
                         std::map<int,std::vector<double>>& sigmaMem, std::map<int,std::vector<float>>& deltaMem, int* bfsDist_new, double* sigma_new, float* delta_new,
                         int src, int v1, int v2, int* direction, std::vector<vector<bool>>& modified, std::map<int,int*>& parentMem)
 {

   bfsDist_new[v1] = bfsDistMem[src][v2] + 1; 
   levelQueues[bfsDist_new[v1]].push_back(v1) ;
   parentMem[src][v1] = v2;
   omp_lock_t* lock = (omp_lock_t*)malloc(g.num_nodes()*sizeof(omp_lock_t));

    #pragma omp parallel for
     for (int t = 0; t < g.num_nodes(); t ++) 
      {
            omp_init_lock(&lock[t]);
      }
  
    std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
    std::vector<std::vector<int>>  levelNodes_prev(omp_get_max_threads()) ;
    std::vector<int>  levelCount(g.num_nodes()) ;

    int phase = bfsDist_new[v1] ;
    int  bfsCount = 1 ;
    levelCount[phase] = bfsCount;
     
      while(!startQueue.empty())
        {
          
             int v = startQueue.front();
             startQueue.pop();
             direction[v] = 1;
             sigma_new[v] = 0;     /*the problem lies here */
             

             for(edge e:g.getInOutNbrs(v/*,modified[src]*/))
                {
                  int w = e.destination ;
                  // printf("w has val %d for v %d e.id %d \n",w,v,e.id);
                
                  if(modified[src][e.id])
                  {
                      
                  //printf("w has val %d for v %d e.id %d e.dir %d bfsdist_w %d bfsdist_v %d\n",w,v,e.id,e.dir,bfsDist_new[w],bfsDist_new[v]);  

                     if((bfsDist_new[w] + 1 == bfsDist_new[v]) && e.dir == 0)
                        {
                          sigma_new[v]+=sigma_new[w] ;
                        }
                      
                      /* this line has problem*/
                  if((bfsDist_new[w] > bfsDist_new[v]) && direction[w] == 0 && e.dir == 1 )
                     {
                       direction[w] = 1;
                       bfsDist_new[w] = bfsDist_new[v] + 1;
                       parentMem[src][w] = v;
                       levelQueues[bfsDist_new[w]].push_back(w);
                       levelCount[bfsDist_new[w]]+=1;
                       startQueue.push(w);
                       
                   
                     }  

                     if((bfsDist_new[w] == bfsDist_new[v]) && (bfsDistMem[src][w] != bfsDistMem[src][v]))
                        {

                           if(direction[w] == 0)
                             {
                               
                                direction[w] = 1;
                                levelQueues[bfsDist_new[w]].push_back(w);
                                levelCount[bfsDist_new[w]]+=1;
                               startQueue.push(w);
                               
                             }                      

                        }
                  }
                }
        }





      phase = g.num_nodes() - 1;

          
   while (phase >= 0)
    {
      #pragma omp parallel for
      for( int l = 0; l < levelCount[phase] ; l++)
      {
        int w = levelQueues[phase][l] ;
      for(edge e: g.getInNeighbors(w)) 
       {
          if(modified[src][e.id]) 
          { 
          int v = e.destination ;
           
           if(bfsDist_new[v] < bfsDist_new[w] /*&& e.dir==0*/)
                 {
                    omp_set_lock(&lock[v]); 
                    if(direction[v] == 0)
                       {
                         direction[v] = 2;
                         delta_new[v] = deltaMem[src][v];
                         int num_thread = omp_get_thread_num();
                         levelNodes_later[num_thread].push_back(v);
                        }

                      if(sigma_new[w] > 0)
                         delta_new[v] += sigma_new[v]*(1 + delta_new[w])/sigma_new[w];

                        float alpha = 0; 
                       if(sigmaMem[src][w] > 0)
                         alpha = sigmaMem[src][v]*(1 + deltaMem[src][w])/sigmaMem[src][w];

                        if(direction[v] == 2 && (v!= v2 || w != v1))
                          delta_new[v] = delta_new[v] - alpha;   
                    omp_unset_lock(&lock[v]);     

                 }
         }

     }
      
      /*if(w != src)
          {
           #pragma omp atomic   
           BC[w] += delta_new[w] - deltaMem[src][w] ;
          }*/
        
    }
    
    phase = phase-1;

     for(int i=0;i<omp_get_max_threads();i++)
         {  
           levelQueues[phase].insert(levelQueues[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
           levelCount[phase] = levelCount[phase] + levelNodes_later[i].size();
           levelNodes_later[i].clear();
           
        }

    }

 }                        


void computeBCAdd(graph& g, float *BC, std::set<int>& sourceSet, std::map<int,std::vector<int>>& bfsDistMem, std::map<int,std::vector<float>>& deltaMem, std::map<int, std::vector<double>>& sigmaMem, std::vector<update>& addBatch, vector<vector<bool>>& modified, int* edgeMapping, std::map<int,int*>& parentMem)
{
 
const int node_count=g.num_nodes();       
std::set<int>::iterator itr;
std::vector<int> srcVec(sourceSet.size());
srcVec.assign(sourceSet.begin(), sourceSet.end());

//#pragma omp parallel for
for(int k = 0 ; k<srcVec.size() ; k++/*itr = sourceSet.begin(); itr!=sourceSet.end() ; itr++*/)
    {
       
    int src = srcVec[k];

    for(int i = 0 ; i < addBatch.size() ; i++)
      {
  
       update u = addBatch[i] ;
       int v1 = addBatch[i].source;
       int v2 = addBatch[i].destination;          
       modified[src][edgeMapping[i]] = true;
       //printf("edgeMapping value %d\n",edgeMapping[i]);
       
       if(bfsDistMem[src][v1] > bfsDistMem[src][v2])
        continue;
        

         
       int levelDiff = bfsDistMem[src][v2] - bfsDistMem[src][v1];
       double* sigma_new = new double[g.num_nodes()];
       int* bfsDist_new = new int[g.num_nodes()];
       float* delta_new = new float[g.num_nodes()];
       int* direction = new int[g.num_nodes()];
     

     //  printf("levelDiff value %d v1 %d v2 %d\n",levelDiff,v1,v2);

       if(levelDiff == 0)
          continue;
       else
         {
       
           std::queue<int> startQueue;
           std::vector<std::queue<int>> levelQueues(g.num_nodes());
            std::vector<std::vector<int>> levelQueues1(g.num_nodes());
            startQueue.push(v2);

           for(int i = 0 ; i < g.num_nodes() ; i++)
              {
                  bfsDist_new[i] = bfsDistMem[src][i];
                  sigma_new[i] = sigmaMem[src][i];
                  delta_new[i] = 0;
                  direction[i] = 0;
                  
              }

          if(levelDiff == 1)
             oneLevelProcessing(g, BC, levelQueues1, startQueue, bfsDistMem, sigmaMem, deltaMem,
                                bfsDist_new, sigma_new, delta_new, src, v2, v1 , direction, modified) ;
          else
             nonAdjacentLevelProcessing(g, BC, levelQueues1, startQueue, bfsDistMem, sigmaMem, deltaMem,
                                bfsDist_new, sigma_new, delta_new, src, v2, v1 , direction, modified, parentMem);                          


         }


         for(int i = 0 ; i<g.num_nodes() ; i++)
             {

                 bfsDistMem[src][i] = bfsDist_new[i];
                 sigmaMem[src][i] = sigma_new[i];

                 if(direction[i]!=0)
                    deltaMem[src][i] = delta_new[i];

               

             }
        
      }
            
 }


}

void computeBCDel(graph& g, float *BC, std::set<int>& sourceSet, std::map<int,std::vector<int>>& bfsDistMem, std::map<int,std::vector<float>>& deltaMem, std::map<int, std::vector<double>>& sigmaMem, std::vector<update>& deleteBatch, vector<vector<bool>>& modified, vector<int>& delEdgeIds, std::map<int,int*>& parentMem,  std::map<int,bool*>& isChanged,std::map<int,bool*>& bfsModMem)
 {
  const int node_count=g.num_nodes();       
  std::set<int>::iterator itr;
  std::vector<int> srcVec(sourceSet.size());
  srcVec.assign(sourceSet.begin(), sourceSet.end());
  
  for(int k = 0 ; k<srcVec.size() ; k++)
    {
       
    int src = srcVec[k];

    double* sigma_new = new double[g.num_nodes()];
    int* bfsDist_new = new int[g.num_nodes()];
    float* delta_new = new float[g.num_nodes()];
    int* direction = new int[g.num_nodes()];
   // bool* modifiedNodes = new bool[g.num_nodes()];
    std::vector<std::vector<int>> levelQueues(g.num_nodes());


     for(int i = 0 ; i < g.num_nodes() ; i++)
         {
            bfsDist_new[i] = bfsDistMem[src][i];
            sigma_new[i] = sigmaMem[src][i];
            delta_new[i] = 0;
            direction[i] = 0;
                  
        }


   bool finished = false;
   bool* modifiedNodes = isChanged[src];
   bool* modMem = bfsModMem[src];


    #pragma omp parallel for
     for (int t = 0; t < g.num_nodes(); t ++) 
      { 
         if(parentMem[src][t]==-1 && t!=src)
           {   
            bfsDist_new[t] = INT_MAX/2;
           }

      }

     


  /* iteratively detect all the modified nodes */
   int* parent = parentMem[src];  
   while (!finished )
    {
      finished = true;   
      #pragma omp parallel for
      for (int v = 0; v < g.num_nodes(); v ++) 
      {
        if (!modMem[v])
         {        
             int parent_v = parent[v];
             if(parent_v>-1 && modMem[parent_v])
               {  
                   bfsDist_new[v] = INT_MAX/2;
                   sigma_new[v] = 0;
                   modMem[v] = true;
                   finished = false; 
               }
         }
      }  

  } 

  for(int i = 0 ; i<g.num_nodes() ; i++)
     {
        if(modMem[i])
          {
            if(!modifiedNodes[i])
               modifiedNodes[i] = true; 
            
          }
     }

 
  oneLevelDelProcessing(g, BC, levelQueues, bfsDistMem, sigmaMem, deltaMem,
                                bfsDist_new, sigma_new, delta_new, src,  direction, modified, delEdgeIds, parentMem, modifiedNodes/*, edgeMapping[i]*/) ;

    for(int i = 0 ; i<g.num_nodes() ; i++)
     {
        bfsDistMem[src][i] = bfsDist_new[i];
        sigmaMem[src][i] = sigma_new[i];
       // printf("delta val of i %d %f\n",i,delta_new[i]);
        if(direction[i]!=0)
           deltaMem[src][i] = delta_new[i];
        if(bfsDist_new[i] == INT_MAX/2)
           { 
            // BC[i] -= deltaMem[src][i];
             deltaMem[src][i] = 0;  
           } 
          /*if(i!=src)
             BC[i] += deltaMem[src][i]; */ 
       

       //  printf("BC val of node %d  %0.9f for src %d deltaMem[src][i] %f\n", i, BC[i],src, deltaMem[src][i]);
    }          
 }

}

int main()
{

  graph G("../dataRecords/socialNetwork/comparisionData1.txt");
  G.parseGraph();
  randomGeneratorUtil rand;
  //randomGeneratorUtil rand(0,G.num_nodes());
  float *BC = new float[G.num_nodes()];
  std::set<int> seeds;
  std::map<int, std::vector<int>> bfsDistMem;
  std::map<int, std::vector<float>> deltaMem; 
  std::map<int, std::vector<double>> sigmaMem; 
  std::map<int,int*> parentMem;
  std::vector<update> addBatch;
  std::vector<update> deleteBatch;
  std::map<int,bool*> modifiedMem;
  std::map<int,bool*> bfsModMem;
 
 rand.feedFromFile(seeds,"../dataRecords/sampleSources.txt");

  printf("seeds size %d\n",seeds.size());
   
  staticBC(G, BC, seeds, bfsDistMem, deltaMem, sigmaMem, parentMem);

 /* for (int i = 0; i <= G.num_nodes(); i++)
  {
    printf("bfsDist mem i %d val %d \n ",i,bfsDistMem[8][i]);
    printf("sigmamem i %d val %f \n ",sigmaMem[8][i]);
   printf("deltamem i %d val %f \n",deltaMem[8][i]);
   printf("BC i %d val %f\n ",i,BC[i]);
   BC[i] = 0;
  }*/

  printf("************************************\n");
 
  std::vector<update> updateEdges = G.parseUpdates("../dataRecords/random-rmat/update_uniformRandom.txt");
  int batchSize = 0;// 15 ;//updateEdges.size();
  int updateSize = batchSize;
 std::vector<vector<bool>> modified(G.num_nodes());
 std::vector<int> delEdgeIds;
 
 
  int elements = 0;
  std::set<int>::iterator itr;
  double startTime = omp_get_wtime();




  for(int k=0;k</*updateEdges.size()*/updateSize;k+=batchSize)
    { 
       
       printf("Batch No : %d\n",k);
      if((k+batchSize) > /*updateEdges.size()*/updateSize)
        {
          elements = /*updateEdges.size()*/updateSize - k ;

         
         
        }
        else
          elements = batchSize;


      for(int i=k;i<(k+batchSize) && i<updateSize/*updateEdges.size()*/;i++)
         {
         if(updateEdges[i].type=='d')
          {

            deleteBatch.push_back(updateEdges[i]);
          }
       }          

 int* edgeMapping = new int[deleteBatch.size()];

 for(itr = seeds.begin() ; itr!=seeds.end() ; itr++)
  {
    std::vector<bool> srcEdgeFilter(G.num_edges());
    bool* nodeFlag = new bool[G.num_nodes()];
    bool* dupNodeFlag = new bool[G.num_nodes()];
   for(int i = 0 ; i<G.num_edges() ; i++)
     {
       srcEdgeFilter[i] = true;
     }

    for(int i = 0 ;i< G.num_nodes() ; i++)
       {
           nodeFlag[i] = false;
           dupNodeFlag[i] = false;
       } 
     modifiedMem[*itr] = nodeFlag;  
     bfsModMem[*itr] = dupNodeFlag;
     modified[*itr] = srcEdgeFilter;
     srcEdgeFilter.clear();
  }   




  for(int i = 0 ; i<deleteBatch.size() ; i++)
      {
         int src = deleteBatch[i].source;
         int dest = deleteBatch[i].destination;
         for(itr = seeds.begin() ; itr!=seeds.end() ; itr++)
          {
           if(bfsDistMem[*itr][dest] == bfsDistMem[*itr][src] + 1)
            {
             modifiedMem[*itr][dest] = true; 
             if(parentMem[*itr][dest] == src) 
               {
                 // bfsDistMem[*itr][dest] = INT_MAX/2;
                 // printf("node %d \n",dest);
                  bfsModMem[*itr][dest] = true;
                  parentMem[*itr][dest] = -1;
               }
           }
          }
      


         for(edge e:G.getNeighbors(src))
             {
                 int nbr = e.destination;
                 if(nbr == dest)
                   {
                    edgeMapping[i] = e.id;
                    for(itr = seeds.begin() ; itr!=seeds.end() ; itr++)
                        modified[*itr][e.id] = false;  
                    delEdgeIds.push_back(e.id);    
                   }
             }

      } 

     /* for(int i = 0 ; i<G.num_nodes() ; i++)
         {
           printf("node %d modmem %d\n",i,bfsModMem[8][i]);
         }*/
   
  // printf("for src--------- 

  // computeBCDel(G, BC, seeds, bfsDistMem, deltaMem, sigmaMem, deleteBatch,modified,delEdgeIds,parentMem,modifiedMem,bfsModMem);
  // G.updateCSRDel(updateEdges,k,elements);
   

  

   for(int i=k;i<(k+batchSize) && i</*updateEdges.size()*/updateSize;i++)
      {
         if(updateEdges[i].type=='a')
          {

            addBatch.push_back(updateEdges[i]);
          }
   }


  /*int v1 = addBatch[0].source;
  int v2 = addBatch[0].destination;*/   
 


  G.updateCSRAdd(updateEdges,k,elements);

 
  edgeMapping = new int[addBatch.size()];

 for(itr = seeds.begin() ; itr!=seeds.end() ; itr++)
  {
    std::vector<bool> srcEdgeFilter(G.num_edges());
   for(int i = 0 ; i<G.num_edges() ; i++)
     {
       srcEdgeFilter[i] = true;
     }
     modified[*itr] = srcEdgeFilter;
     srcEdgeFilter.clear();
  }   

   for(int i = 0 ; i<addBatch.size() ; i++)
      {
         int src = addBatch[i].source;
         int dest = addBatch[i].destination;
         for(edge e:G.getNeighbors(src))
             {
                 int nbr = e.destination;
                 if(nbr == dest)
                   {
                    edgeMapping[i] = e.id;
                    for(itr = seeds.begin() ; itr!=seeds.end() ; itr++)
                        modified[*itr][e.id] = false;  
                   }
             }

      }   
 
  computeBCAdd(G, BC, seeds, bfsDistMem, deltaMem, sigmaMem, addBatch,modified,edgeMapping, parentMem);

  }

  double endTime=omp_get_wtime();
  printf("RunTime : %f\n",endTime-startTime);

 //std::set<int>::iterator itr;  
for(itr = seeds.begin() ; itr!=seeds.end() ; itr++)
  {
    //  printf("New Seed.............%d\n",*itr);
        for(int i = 0 ; i<G.num_nodes() ; i++)
             {
              /* printf("bfsDist mem i %d val %d\n ",i,bfsDistMem[*itr][i]);
                printf("sigmamem i %d val %f \n ",sigmaMem[*itr][i]);
                printf("deltamem i %d val %0.9f  \n",deltaMem[*itr][i]);*/
                if(i!=*itr)
                  {
                  BC[i] += deltaMem[*itr][i];
                 // printf("i %d BC %0.9f \n ",i,BC[i]);
                  }
                   
             }

  }         
  
    
  for (int i = 0; i <= G.num_nodes(); i++)
  {
    printf("%d  %0.9f\n", i, BC[i]);
  }



}