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
using namespace std;
namespace mpi = boost::mpi;

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
  int* rev_indexofNodes; /*stores destination corresponding to edgeNo.
                       required for iteration over out neighbours */
  int* srcList;  /*stores source corresponding to edgeNo.
                       required for iteration over in neighbours */
  graph(char* file)
  {
    filePath=file;
    original_nodesTotal=0;
    edgesTotal=0;
    degree_max=0;
    padded_nodesTotal=0;
    //np = n;
    mpi::communicator world;
    int my_rank = world.rank();
    np = world.size();
    //std::cout << "World size: " <<np << std::endl;

  }

  std::map<int,std::vector<edge>> getEdges()
  {
      return edges;
  }
  
  int* getEdgeLen()
  {
    return edgeLen;
  }
  int* getEdgeList()
  {
    return edgeList;
  }
  int* getSrcList()
  {
    return srcList;
  }
  int* getIndexofNodes()
  {
    return indexofNodes;
  }
  int* getRevIndexofNodes()
  {
    return rev_indexofNodes;
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
#if 0
   bool check_if_nbr(int s, int d)
    {
      int startEdge=indexofNodes[s];
      int endEdge=indexofNodes[s+1]-1;


      if(edgeList[startEdge]==d)
          return true;
      if(edgeList[endEdge]==d)
         return true;   

       int mid = (startEdge+endEdge)/2;

      while(startEdge<=endEdge)
        {
       
          if(edgeList[mid]==d)
             return true;

          if(d<edgeList[mid])
             endEdge=mid-1;
          else
            startEdge=mid+1;   
          
          mid = (startEdge+endEdge)/2;

        }
      
      return false;

    }
#endif
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
	int weight;
        if(ss >> source && ss >> destination && ss >> weight)
        {  
           if(source >  original_nodesTotal)
              original_nodesTotal = source;
            if(destination > original_nodesTotal)
               original_nodesTotal = destination;  
           e.source = source;
           e.destination = destination;
           e.weight = weight;

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
     rev_indexofNodes=new int[padded_nodesTotal+1];
     srcList=new int32_t[edgesTotal];

    
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
    for(int i=0;i<padded_nodesTotal;i++)
    {
	 std::vector <std::pair<int,int>> vect;
	 for (int j = indexofNodes[i]; j < indexofNodes[i+1]; j++)
	 	vect.push_back(std::make_pair(edgeList[j], edgeLen[j]));
	 std::sort(vect.begin(),vect.end());
	
	 for (int j = 0; j < vect.size(); j++)
	 {
		edgeList[j + indexofNodes[i]] = vect[j].first;
		edgeLen[j + indexofNodes[i]] = vect[j].second;
	 }

	vect.clear();
	std::vector <std::pair<int,int>>().swap(vect);
	
          //std::vector<int> vect;
          //vect.insert(vect.begin(),edgeList+indexofNodes[i],edgeList+indexofNodes[i+1]);
          //std::sort(vect.begin(),vect.end());
          //for(int j=0;j<vect.size();j++)
          //  edgeList[j+indexofNodes[i]]=vect[j];

            //vect.clear();
	//	std::vector<int>().swap(vect);

    }

         #pragma omp parallel for
    for(int i=0;i<padded_nodesTotal+1;i++)
       rev_indexofNodes[i] = 0;
   

        
    /* Prefix Sum computation for in neighbours 
       Loads rev_indexofNodes and srcList.
    */

    /* count indegrees first */
    int32_t* edge_indexinrevCSR = new int32_t[edgesTotal];

    #pragma omp parallel for
    for(int i=0;i<padded_nodesTotal;i++)
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
      for(int i=0;i<padded_nodesTotal;i++)
        {
          int temp = prefix_sum;
          prefix_sum = prefix_sum + rev_indexofNodes[i];
          rev_indexofNodes[i]=temp;
      
        }
        rev_indexofNodes[padded_nodesTotal] = prefix_sum;

    /* store the sources in srcList */
      #pragma omp parallel for 
      for(int i=0;i<padded_nodesTotal;i++)
        {
          for(int j=indexofNodes[i];j<indexofNodes[i+1];j++)
            {
               int dest = edgeList[j];
               int index_in_srcList=rev_indexofNodes[dest]+edge_indexinrevCSR[j];
               srcList[index_in_srcList] = i;
            }
        }
        
       #pragma omp parallel for num_threads(4)
        for(int i=0;i<padded_nodesTotal;i++)
        {
          std::vector<int> vect;
          vect.insert(vect.begin(),srcList+rev_indexofNodes[i],srcList+rev_indexofNodes[i+1]);
          std::sort(vect.begin(),vect.end());
          for(int j=0;j<vect.size();j++)
            srcList[j+rev_indexofNodes[i]]=vect[j];

            vect.clear();
		std::vector<int>().swap(vect);

        }
	delete [] edge_indexinrevCSR;
	edges.clear();
	std::map<int,std::vector<edge>>().swap(edges);
 }


};
