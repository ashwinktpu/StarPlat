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
  int original_nodesTotal;
  int padded_nodesTotal;
  int edgesTotal;
  int degree_max;
  int np;

  char* filePath;
  std::map<int,std::vector<edge>> edges;

  public:
  int* indexofNodes;
  int* edgeList;
  int* edgeLen;
  graph(char* file,int n)
  {
    filePath=file;
    original_nodesTotal=0;
    edgesTotal=0;
    degree_max=0;
    padded_nodesTotal=0;
    np = n;

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
      return padded_nodesTotal;
  }
  int ori_num_nodes()
  {
      return original_nodesTotal;
  }
  int num_edges()
  {
      return edgesTotal;
  }
  int max_degree()
  {
      return degree_max;
  }

  
   void parseGraph()
  { 
     std::ifstream infile;
     infile.open(filePath);
     if(!infile)
     {
     	std::cout <<"Cannot open the file"<<std::endl;
     	exit(0);
     }
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
        if(ss>>source && ss>>destination)
        {  
           if(source >  original_nodesTotal)
              original_nodesTotal = source;
            if(destination > original_nodesTotal)
               original_nodesTotal = destination;  
           e.source = source;
           e.destination = destination;
           e.weight = 1;

           edges[source].push_back(e);
          
        }
             

     }
     original_nodesTotal+=1;
     if(original_nodesTotal % np == 0)
     {
        padded_nodesTotal = original_nodesTotal;
     }
     else
     {
        padded_nodesTotal = original_nodesTotal + np -(original_nodesTotal%np);
     }
   // #pragma omp parallel for 
     for(int i=0;i<padded_nodesTotal;i++)
     {
       std::vector<edge> edgeOfVertex=edges[i];
     	//std::cout<<i << " "<<edges[i].size()<<std::endl;
     	if(edges[i].size() > degree_max)
     		degree_max = edges[i].size();
       sort(edgeOfVertex.begin(),edgeOfVertex.end(),
                            [](const edge& e1,const edge& e2) {
                               if(e1.source!=e2.source)
                                  return e1.source<e2.source;

                                return e1.destination<e2.destination;  

                            });

     }                      
                   
     indexofNodes=new int[padded_nodesTotal+1];
     edgeList=new int[edgesTotal];
     edgeLen=new int[edgesTotal];
    
    int edge_no=0;
    
    for(int i=0;i<padded_nodesTotal;i++)
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
    indexofNodes[padded_nodesTotal]=edge_no;


 }


};
