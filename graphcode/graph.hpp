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
  int32_t* indexofNodes;
  int32_t* edgeList;
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

  
   void parseGraph()
  {  printf("OH HELLOHIHod \n");
     std::ifstream infile;
     infile.open(filePath);
     std::string line;
     ////int maxSize=2048;
    // char line[maxSize];
    // std::stringstream ss;
     while (std::getline(infile,line))
     {
      
      // std::stringstream(line);
      
       if (line.length()==0||line[0] < '0' || line[0] >'9') {
          continue; 

	    	}
        
        std::stringstream ss(line);
        
        edgesTotal++;
        
        

        
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
          
           ss>>weightVal; //for edgelists having weight too.
       
      
      
        

     }
    // printf("HELLO AFTER THIS %d \n",nodesTotal);
    #pragma omp parallel for num_threads(4)
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
     edgeList=new int32_t[edgesTotal];
     edgeLen=new int32_t[edgesTotal];
    
    int edge_no=0;
    
    //#pragma omp parallel for num_threads(4)
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
    printf("hello after this %d %d\n",nodesTotal,edgesTotal);
    
 }








};