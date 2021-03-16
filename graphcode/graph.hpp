#include <fstream>
#include <sstream>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<string>
#include "omp.h"

//using namespace std;

class edge
{
  public:
  int source;
  int destination;
  int weight;

};

class graph
{
  private:
  int nodesTotal;
  int edgesTotal;
  int* edgeLen;
  char* filePath;
  std::map<int,std::vector<edge>> edges;

  public:
  int* indexofNodes;
  int* edgeList;
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
      return nodesTotal;
  }
  int num_edges()
  {
      return edgesTotal;
  }

  
   void parseGraph()
  { 
     std::ifstream infile;
     infile.open(filePath);
     std::string line;
     std::stringstream ss;
     while (getline(infile, line)) 
     {
       
       if (line[0] < '0' || line[0] >'9') {
          continue; 

	    	}
      
        ss.clear();
        ss<<line;
        edgesTotal++;
        
         
        
        edge e;
        int source;
        int destination;
        if(ss>>source&&ss>>destination)
        {  
           if(source>nodesTotal)
              nodesTotal=source;
            if(destination>nodesTotal)
               nodesTotal=destination;  
           e.source=source;
           e.destination=destination;
           e.weight=1;

           edges[source].push_back(e);
          
        }
       
      
      
        

     }
    #pragma omp parallel for 
     for(int i=1;i<=nodesTotal;i++)
     {
       std::vector<edge> edgeOfVertex=edges[i];
     
       sort(edgeOfVertex.begin(),edgeOfVertex.end(),
                            [](const edge& e1,const edge& e2) {
                               if(e1.source!=e2.source)
                                  return e1.source<e2.source;

                                return e1.destination<e2.destination;  

                            });

     }                      
                   
     indexofNodes=new int[nodesTotal+2];
     edgeList=new int[edgesTotal];
     edgeLen=new int[edgesTotal];
    
    int edge_no=0;
    
    for(int i=1;i<=nodesTotal;i++)
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
    indexofNodes[nodesTotal+1]=edge_no;


 }








};