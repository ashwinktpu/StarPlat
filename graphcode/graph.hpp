#include <fstream>
#include <sstream>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<string.h>
#include "omp.h"

//using namespace std;

class edge
{
  public:
  int32_t source;
  int32_t destination;
  int32_t weight;

};

class graph
{
  private:
  int32_t nodesTotal;
  int32_t edgesTotal;
  int32_t* edgeLen;
  char* filePath;
  std::map<int32_t,std::vector<edge>> edges;

  public:
  int32_t* indexofNodes; /* stores prefix sum for outneighbours of a node*/
  int32_t* rev_indexofNodes; /* stores prefix sum for inneighbours of a node*/
  int32_t* edgeList; /*stores destination corresponding to edgeNo.
                       required for iteration over out neighbours */
  int32_t* srcList;  /*stores source corresponding to edgeNo.
                       required for iteration over in neighbours */
  graph(char* file)
  {
    filePath=file;
    nodesTotal=0;
    edgesTotal=0;

  }

  std::map<int,std::vector<edge>> getEdges()
  {
      return edges;
  }
  
   int* getEdgeLen()
  {
    return edgeLen;
  }

    int num_nodes()
  {
      return nodesTotal+1; //change it to nodesToTal
  }
   int num_edges()
  {
      return edgesTotal;
  }

  bool check_if_nbr(int s, int d)
    {
      int startEdge=indexofNodes[s];
      int endEdge=indexofNodes[s+1]-1;

      if(edgeList[startEdge]==d)
          return true;
      if(edgeList[endEdge]==d)
         return true;   

      int mid;

      while(startEdge<=endEdge)
        {
         mid = (startEdge+endEdge)/2;

          if(edgeList[mid]==d)
             return true;
     
          if(d<edgeList[mid])
             endEdge=mid-1;   
          else
            startEdge=mid+1;   
          
        
        }
      
      return false;



    }


    //GPU specific check for neighbours for TC algorithm
    __device__ bool findNeighborSorted (int s, int d, int * gpu_OA, int *gpu_edgeList)   //we can move this to graph.hpp file
    {
      int startEdge=gpu_OA[s];
      int endEdge=gpu_OA[s+1]-1;


      if(gpu_edgeList[startEdge]==d)
          return true;
      if(gpu_edgeList[endEdge]==d)
         return true;   

       int mid = (startEdge+endEdge)/2;

      while(startEdge<=endEdge)
        {
       
          if(gpu_edgeList[mid]==d)
             return true;

          if(d<gpu_edgeList[mid])
             endEdge=mid-1;
          else
            startEdge=mid+1;   
          
          mid = (startEdge+endEdge)/2;

        }
      
      return false;

    }

  
   void parseGraph()
  {  printf("OH HELLOHIHod \n");
     std::ifstream infile;
     infile.open(filePath);
     std::string line;
    
   


    
     while (std::getline(infile,line))
     {
      
      // std::stringstream(line);
      
       if (line.length()==0||line[0] < '0' || line[0] >'9') {
          continue; 

	    	}
        
        std::stringstream ss(line);
        
        edgesTotal++;
         
        //edgesTotal++; //TO BE REMOVED 

        
        edge e;
        
    
        int32_t source;
        int32_t destination;
        int32_t weightVal;
           ss>>source; 
          // printf("SOURCE %lu ",source);
           if(source>nodesTotal)
              nodesTotal=source;
            ss>>destination;  
           // printf("DESTINATION %lu \n",destination);
            if(destination>nodesTotal)
               nodesTotal=destination;  
           e.source=source;
           e.destination=destination;
           e.weight=1;

           edges[source].push_back(e);

           /*edge e1; //TO BE REMOVED 
           e1.source=destination; //TO BE REMOVED 
           e1.destination=source; //TO BE REMOVED 
           edges[destination].push_back(e1); //TO BE REMOVED */
          
          
           ss>>weightVal; //for edgelists having weight too.
          

     }
    
     
   //  printf("Here half\n");
    // printf("HELLO AFTER THIS %d \n",nodesTotal);
    #pragma omp parallel for 
     for(int i=0;i<=nodesTotal;i++)//change to 1-nodesTotal.
     {
       std::vector<edge>& edgeOfVertex=edges[i];
     
       sort(edgeOfVertex.begin(),edgeOfVertex.end(),
                            [](const edge& e1,const edge& e2) {
                               if(e1.source!=e2.source)
                                  return e1.source<e2.source;

                                return e1.destination<e2.destination;  

                            });

     }                      
                   
     indexofNodes=new int32_t[nodesTotal+2];
     rev_indexofNodes=new int32_t[nodesTotal+2];
     edgeList=new int32_t[edgesTotal];
     srcList=new int32_t[edgesTotal];
     edgeLen=new int32_t[edgesTotal];
    
    int edge_no=0;
    

    /* Prefix Sum computation for out neighbours 
       Loads indexofNodes and edgeList.
    */
    for(int i=0;i<=nodesTotal;i++) //change to 1-nodesTotal.
    {
      std::vector<edge> edgeofVertex=edges[i];
      
      indexofNodes[i]=edge_no;
     
      std::vector<edge>::iterator itr;
    
      for(itr=edgeofVertex.begin();itr!=edgeofVertex.end();itr++)
      { 
       
       
        edgeList[edge_no]=(*itr).destination;
        
        edgeLen[edge_no]=(*itr).weight;
        edge_no++;
      }
      
    }
    indexofNodes[nodesTotal+1]=edge_no;//change to nodesTotal+1.

   
     #pragma omp parallel for num_threads(4)
     for(int i=0;i<nodesTotal+1;i++)
       rev_indexofNodes[i] = 0;
   

        
    /* Prefix Sum computation for in neighbours 
       Loads rev_indexofNodes and srcList.
    */

    /* count indegrees first */
    int32_t* edge_indexinrevCSR = new int32_t[edgesTotal];

    #pragma omp parallel for num_threads(4)
    for(int i=0;i<=nodesTotal;i++)
      {

       for(int j=indexofNodes[i];j<indexofNodes[i+1];j++)
           {
             int dest = edgeList[j];
             int temp = __sync_fetch_and_add( &rev_indexofNodes[dest], 1 );
             edge_indexinrevCSR[j] = temp;
           }

      }   
    
      /* convert to revCSR */
     int prefix_sum = 0;
      for(int i=0;i<=nodesTotal;i++)
        {
          int temp = prefix_sum;
          prefix_sum = prefix_sum + rev_indexofNodes[i];
          rev_indexofNodes[i]=temp;
      
        }
        rev_indexofNodes[nodesTotal+1] = prefix_sum;

    /* store the sources in srcList */
      #pragma omp parallel for num_threads(4)
      for(int i=0;i<=nodesTotal;i++)
        {
          for(int j=indexofNodes[i];j<indexofNodes[i+1];j++)
            {
               int dest = edgeList[j];
               int index_in_srcList=rev_indexofNodes[dest]+edge_indexinrevCSR[j];
               srcList[index_in_srcList] = i;
            }
        }
        
      #pragma omp parallel for num_threads(4)
        for(int i=0;i<=nodesTotal;i++)
        {
          std::vector<int> vect;
          vect.insert(vect.begin(),srcList+rev_indexofNodes[i],srcList+rev_indexofNodes[i+1]);
          std::sort(vect.begin(),vect.end());
          for(int j=0;j<vect.size();j++)
            srcList[j+rev_indexofNodes[i]]=vect[j];

            vect.clear();

        }
     
    //change to nodesTotal+1.
   // printf("hello after this %d %d\n",nodesTotal,edgesTotal);
    
 }








};