#include "update.hpp"
#include "omp.h"


 std::vector<std::pair<int,int>> perNodeUpdateInfo;
 std::vector<std::pair<int,int>> perNodeUpdateRevInfo;


void updateCSRDel_omp(int numnodes, int numEdges, int32_t* indexofNodes, int32_t* edgeList, int32_t* rev_indexofNodes, int32_t* srcList, int32_t* edgeLen, int32_t* diff_edgeLen,
                  int32_t* diff_indexofNodes, int32_t* diff_edgeList, int32_t* diff_rev_indexofNodes,int32_t* diff_rev_edgeList, int32_t* rev_edgeLen, int32_t* diff_rev_edgeLen,
                  std::vector<std::pair<int,int>>& perNodeUpdateInfo_sent, std::vector<std::pair<int,int>>& perNodeUpdateRevInfo_sent, int32_t* perNodeCSRSpace, int32_t* perNodeRevCSRSpace, std::vector<update>& updates)
{
  
  perNodeUpdateInfo = perNodeUpdateInfo_sent;
  perNodeUpdateRevInfo = perNodeUpdateRevInfo_sent;


   #pragma omp parallel for
   for(int i=0; i<updates.size();i++)
     {
       update u = updates[i];
       if(u.type == 'd')
         {
            int32_t source = u.source;
            int32_t destination = u.destination;
            bool csrfound = false;
            bool revcsrfound = false;
            for(int j=indexofNodes[source] ; j<indexofNodes[source+1]; j++)
                {
                     int nbr = edgeList[j];
                     if(nbr==destination)
                       {
                        edgeList[j] = INT_MAX/2; 
                        #pragma omp atomic
                        perNodeCSRSpace[source] =  perNodeCSRSpace[source] + 1;
                        csrfound = true;
                       }
                        
                }

              for(int j=rev_indexofNodes[destination] ; j<rev_indexofNodes[destination+1]; j++)
                {
                     int nbr = srcList[j];
                     if(nbr==source)
                       {
                        srcList[j] = INT_MAX/2;
                        #pragma omp atomic
                        perNodeRevCSRSpace[destination] = perNodeRevCSRSpace[destination] + 1;
                        revcsrfound = true;
                       }
                        
                }  
             if(!csrfound) //search in diff-CSR
              {  
                 // printf("source %d destination %d\n",source,destination);
                  for(int j=diff_indexofNodes[source] ; j<diff_indexofNodes[source+1]; j++)
                    {
                      int nbr = diff_edgeList[j];
                      if(nbr==destination)
                       {
                        diff_edgeList[j] = INT_MAX/2;
                         
                         
                          csrfound = true;
                       }
                    }
              } 

              if(!revcsrfound)
                 {
                    for(int j=diff_rev_indexofNodes[destination] ; j<diff_rev_indexofNodes[destination+1]; j++)
                    {
                      int nbr = diff_rev_edgeList[j];
                      if(nbr==source)
                       {
                        diff_rev_edgeList[j] = INT_MAX/2;
                          
                          
                          revcsrfound = true;
                       }
                    }

                 }
         

     }
     } 


}
 void updateCSRAdd_omp(int numnodes, int numEdges, int32_t* indexofNodes, int32_t* edgeList, int32_t* rev_indexofNodes, int32_t* srcList, int32_t* edgeLen, int32_t** diff_edgeLen,
                  int32_t** diff_indexofNodes, int32_t** diff_edgeList, int32_t** diff_rev_indexofNodes,int32_t** diff_rev_edgeList, int32_t* rev_edgeLen, int32_t** diff_rev_edgeLen,
                  std::vector<std::pair<int,int>>& perNodeUpdateInfo_sent, std::vector<std::pair<int,int>>& perNodeUpdateRevInfo_sent,
                  int32_t* perNodeCSRSpace, int32_t* perNodeRevCSRSpace, std::vector<update>& updates)
     {

        int* counterLoc = new int[numnodes];
        int* counterRevLoc = new int[numnodes];
        int*  diff_CSRPerNodeDel = new int[numnodes];
        int*  diff_revCSRPerNodeDel = new int[numnodes];

        perNodeUpdateInfo = perNodeUpdateInfo_sent;
        perNodeUpdateRevInfo = perNodeUpdateRevInfo_sent;

        std::fill(diff_CSRPerNodeDel, diff_CSRPerNodeDel+numnodes, 0);
        std::fill(diff_revCSRPerNodeDel, diff_revCSRPerNodeDel+numnodes, 0);

        std::vector<int> diffPerNodeSpace(numnodes);
        std::vector<int> diff_revPerNodeSpace(numnodes);
        int diff_edgeListSize=0;
        int diff_rev_edgeListSize=0;




     if(*diff_edgeList==NULL)
      { 
        /* if diff-CSR is not created and this is the first instance of diff-CSR  creation.*/ 

        #pragma omp parallel for 
        for(int i=0;i<numnodes;i++)
          {
              if(perNodeUpdateInfo[i].second>perNodeUpdateInfo[i].first)
                {
                 diffPerNodeSpace[i] = perNodeUpdateInfo[i].second - perNodeUpdateInfo[i].first;
                 #pragma omp atomic
                 diff_edgeListSize = diff_edgeListSize + diffPerNodeSpace[i];
                }
              else
                 diffPerNodeSpace[i] = 0;

          }
         

 
        if(diff_edgeListSize>0)
        {
         // printf("DIFF_EDGELISTSIZE = %d\n",diff_edgeListSize);
        *diff_edgeList = new int[diff_edgeListSize];
        *diff_indexofNodes = new int[numnodes+1];
        *diff_edgeLen = new int[diff_edgeListSize];
        (*diff_indexofNodes)[0] = 0;
        counterLoc[0] = 0;
        for(int i=1;i<=numnodes;i++)
         {
           (*diff_indexofNodes)[i]=(*diff_indexofNodes)[i-1] + diffPerNodeSpace[i-1];
           if(i!=numnodes)
             counterLoc[i] = 0;
         }  
        }
      }
      else
      {     
            /* If there exists a diff-CSR */

            //store the deleted space size for each node in CSR.
            std::vector<int> csrPerNodeSpace(numnodes,0);
            
            //store the extra space required for 
              std::vector<int> diff_CSRPerNodeAdd(numnodes,0);

               #pragma omp parallel for
               for(int source = 0; source<numnodes; source++)
               {
               for(int j=(*diff_indexofNodes)[source] ; j<(*diff_indexofNodes)[source+1]; j++)
                    {
                      if((*diff_edgeList)[j] == INT_MAX/2)
                          {
                            #pragma omp atomic
                            diff_CSRPerNodeDel[source] = diff_CSRPerNodeDel[source] + 1;
                          }

                          
                    }  
               }   

               
           
           // int diff_edgeListSize=0;

            #pragma omp parallel for
            for(int i=0 ; i<numnodes ; i++)
              {

                csrPerNodeSpace[i] = perNodeUpdateInfo[i].first - diff_CSRPerNodeDel[i];
                if(perNodeUpdateInfo[i].second>(csrPerNodeSpace[i]+perNodeCSRSpace[i]))
                   diff_CSRPerNodeAdd[i]= perNodeUpdateInfo[i].second - csrPerNodeSpace[i] - perNodeCSRSpace[i];
                else
                   diff_CSRPerNodeAdd[i] = 0;  

                  diffPerNodeSpace[i] = (*diff_indexofNodes)[i+1] - (*diff_indexofNodes)[i] - diff_CSRPerNodeDel[i] +
                                           diff_CSRPerNodeAdd[i];  
                  #pragma omp atomic                          
                  diff_edgeListSize = diff_edgeListSize + diffPerNodeSpace[i]; 

                  counterLoc[i]  = (*diff_indexofNodes)[i+1] - (*diff_indexofNodes)[i] - diff_CSRPerNodeDel[i] ;                        
              }

  
        if(diff_edgeListSize>0)
            {
            int* diff_edgeListNew = new int[diff_edgeListSize];
            int* diff_indexofNodesNew = new int[numnodes+1];
            int* diff_edgeLenNew = new int[diff_edgeListSize];
              
            diff_indexofNodesNew[0] = 0;
            for(int i=1;i<=numnodes;i++)
             {
                diff_indexofNodesNew[i]=diff_indexofNodesNew[i-1] + diffPerNodeSpace[i-1];
             } 

              #pragma omp parallel for
              for(int i=0;i<numnodes;i++)
              {
                 int pos = 0;  

                 
                for(int j=(*diff_indexofNodes)[i];j<(*diff_indexofNodes)[i+1];j++)
                  {
                      int nbr = (*diff_edgeList)[j];
                      int weight = (*diff_edgeLen)[j];
                     
                      if(nbr!=INT_MAX/2)
                        {
                            diff_edgeListNew[diff_indexofNodesNew[i]+pos] = nbr;
                            diff_edgeLenNew[diff_indexofNodesNew[i]+pos] = weight;
                           // printf("pos %d nbr %d",diff_indexofNodesNew[i]+pos,nbr);
                            pos++;
                        }
                  }
              }
             
              free(*diff_indexofNodes);
              free(*diff_edgeList);
              free(*diff_edgeLen);

             *diff_edgeList = diff_edgeListNew;
             *diff_indexofNodes = diff_indexofNodesNew;
             *diff_edgeLen = diff_edgeLenNew;
            }

     }

     if(*diff_rev_edgeList==NULL)
         {
         #pragma omp parallel for 
         for(int i=0;i<numnodes;i++)
          {
              if(perNodeUpdateRevInfo[i].second>perNodeUpdateRevInfo[i].first)
                {
                  diff_revPerNodeSpace[i] = perNodeUpdateRevInfo[i].second - perNodeUpdateRevInfo[i].first;
                 #pragma omp atomic
                 diff_rev_edgeListSize = diff_rev_edgeListSize + diff_revPerNodeSpace[i];

                }  
                else
                  diff_revPerNodeSpace[i]=0;

          }

        if(diff_rev_edgeListSize>0)
         {
           
           *diff_rev_edgeList = new int[diff_rev_edgeListSize];
           *diff_rev_indexofNodes = new int[numnodes+1];
           *diff_rev_edgeLen = new int[diff_rev_edgeListSize];
           (*diff_rev_indexofNodes)[0] = 0;
            counterRevLoc[0] = 0;

          for(int i=1;i<=numnodes;i++)
           {
           (*diff_rev_indexofNodes)[i]=(*diff_rev_indexofNodes)[i-1] + diff_revPerNodeSpace[i-1];
             if(i!=numnodes)
               counterRevLoc[i] = 0;
            
            }  
        }

     
      }
      else
      {
         
          //store the deleted space size for each node in revCSR.
             std::vector<int> revcsrPerNodeSpace(numnodes,0);

          //store the extra space required for 
              std::vector<int> diff_revCSRPerNodeAdd(numnodes,0);  


               #pragma omp parallel for
               for(int destination = 0; destination<numnodes; destination++)
               {
               for(int j=(*diff_rev_indexofNodes)[destination] ; j<(*diff_rev_indexofNodes)[destination+1]; j++)
                    {
                      if((*diff_rev_edgeList)[j]== INT_MAX/2)
                          {
                          #pragma omp atomic
                          diff_revCSRPerNodeDel[destination] = diff_revCSRPerNodeDel[destination] + 1;
                          }

                          
                    }  
               }   

            #pragma omp parallel for
            for(int i=0 ; i<numnodes ; i++)
              {
                revcsrPerNodeSpace[i] = perNodeUpdateRevInfo[i].first - diff_revCSRPerNodeDel[i];
                if(perNodeUpdateRevInfo[i].second> (revcsrPerNodeSpace[i]+perNodeRevCSRSpace[i]))
                   diff_revCSRPerNodeAdd[i]= perNodeUpdateRevInfo[i].second - revcsrPerNodeSpace[i] - perNodeRevCSRSpace[i];
                else
                   diff_revCSRPerNodeAdd[i] = 0;  

                  diff_revPerNodeSpace[i] = (*diff_rev_indexofNodes)[i+1] - (*diff_rev_indexofNodes)[i] - diff_revCSRPerNodeDel[i] +
                                           diff_revCSRPerNodeAdd[i];   
                  #pragma omp atomic                         
                  diff_rev_edgeListSize = diff_rev_edgeListSize + diff_revPerNodeSpace[i]; 
                  counterRevLoc[i]  = (*diff_rev_indexofNodes)[i+1] - (*diff_rev_indexofNodes)[i] - diff_revCSRPerNodeDel[i] ;                        
              }

           if(diff_rev_edgeListSize>0)
            {
            int* diff_revedgeListNew = new int[diff_rev_edgeListSize];
            int* diff_revindexofNodesNew = new int[numnodes+1];
            int* diff_rev_edgeLenNew = new int[diff_rev_edgeListSize];
              
            diff_revindexofNodesNew[0] = 0;
            for(int i=1;i<=numnodes;i++)
             {
                diff_revindexofNodesNew[i]=diff_revindexofNodesNew[i-1] + diff_revPerNodeSpace[i-1];
             } 

              #pragma omp parallel for
              for(int i=0;i<numnodes;i++)
              {
                 int pos = 0;  
                for(int j=(*diff_rev_indexofNodes)[i];j<(*diff_rev_indexofNodes)[i+1];j++)
                  {
                      int nbr = (*diff_rev_edgeList)[j];
                      int weight = (*diff_rev_edgeLen)[j];
                      if(nbr!=INT_MAX/2)
                        {
                            diff_revedgeListNew[diff_revindexofNodesNew[i]+pos] = nbr;
                            diff_rev_edgeLenNew[diff_revindexofNodesNew[i]+pos] = weight;
                            pos++;
                        }
                  }
              }
             
               free(*diff_rev_indexofNodes);
               free(*diff_rev_edgeList);
               free(*diff_rev_edgeLen);

             *diff_rev_edgeList = diff_revedgeListNew;
             *diff_rev_indexofNodes = diff_revindexofNodesNew;
             *diff_rev_edgeLen = diff_rev_edgeLenNew;
       }   
    }
    


   /* parallel updation to CSR/diff-CSR*/
   
   #pragma omp parallel for
   for(int i=0; i<updates.size();i++)
     {
       update u = updates[i];
       int32_t source = u.source;
       int32_t destination = u.destination;
       int32_t weight = u.weight;
       bool updated = false;
       bool rev_updated = false;
       if(u.type == 'a')
         { 
           //first check for free space in source's dest array in CSR.
           for(int i=indexofNodes[source];i<indexofNodes[source+1];i++)
              {
                  int prev_Val ;
                 if(edgeList[i]==INT_MAX/2)
                  {  
                     prev_Val = __sync_val_compare_and_swap(&edgeList[i],INT_MAX/2,destination);
                     if(prev_Val==INT_MAX/2)
                       {
                         updated = true;
                         edgeLen[i] = weight;
                         #pragma omp atomic
                         perNodeCSRSpace[source] = perNodeCSRSpace[source] - 1;
                         break;
                       }
                  }

                   

                }

              if(!updated)
                { 
                  // printf("2- source %d destination %d\n",source,destination);
                   int startIndex = (*diff_indexofNodes)[source];
                   int displacement = __sync_fetch_and_add(&counterLoc[source],1);
                   (*diff_edgeList)[startIndex+displacement] = destination;
                   (*diff_edgeLen)[startIndex+displacement] = weight;
                  // printf(" src %d destination %d weight %d \n",source,destination,weight);

               }

              for(int i=rev_indexofNodes[destination];i<rev_indexofNodes[destination+1];i++)
              {
                 int prev_Val ;
                 if(srcList[i]==INT_MAX/2)
                  {  
                     prev_Val = __sync_val_compare_and_swap(&srcList[i],INT_MAX/2,source);
                     if(prev_Val==INT_MAX/2)
                       {
                         rev_updated = true;
                         rev_edgeLen[i] = weight;
                         #pragma omp atomic 
                         perNodeRevCSRSpace[destination] =  perNodeRevCSRSpace[destination] - 1;
                         break;
                       }
                  }

                }

                if(!rev_updated)
                {
                   int startIndex = (*diff_rev_indexofNodes)[destination];
                   int displacement = __sync_fetch_and_add(&counterRevLoc[destination],1);
                //   printf("index gave segfault %d destination %d\n",startIndex+displacement,destination);
                   (*diff_rev_edgeList)[startIndex+displacement] = source;
                   (*diff_rev_edgeLen)[startIndex+displacement] = weight;
                }
         }
     }

    /*free the meta data fields.*/
    
     diffPerNodeSpace.clear();
     diff_revPerNodeSpace.clear();
     free(diff_CSRPerNodeDel);
     free(diff_revCSRPerNodeDel); 
     free(counterLoc);
     free(counterRevLoc);


}



