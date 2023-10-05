#include <fstream>
#include <sstream>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<string>
// #include "omp.h"

//using namespace std;
using namespace std;

class edge
{
  public:
  long long  source;
  long long  destination;
  long long  weight;
};

class graph
{
  private:
  long long  original_nodesTotal;
  long long  padded_nodesTotal;
  long long  edgesTotal;
  long long  degree_max;
  long long  np;

  char* filePath;
  std::map<long long ,std::vector<edge>> edges;

  public:
  long long * indexofNodes;
  long long * edgeList;
  long long * edgeLen;
  long long * rev_indexofNodes; /*stores destination corresponding to edgeNo.
                       required for iteration over out neighbours */
  long long * srcList;  /*stores source corresponding to edgeNo.
                       required for iteration over in neighbours */
  explicit graph(char* file)
  {
    filePath=file;
    original_nodesTotal=0;
    edgesTotal=0;
    degree_max=0;
    padded_nodesTotal=0;
    np = 1;
    //std::cerr << "World size: " <<np << std::endl;

  }


  std::map<long long ,std::vector<edge>> getEdges()
  {
      return edges;
  }
  
  long long * getEdgeLen()
  {
    return edgeLen;
  }
  long long * getEdgeList()
  {
    return edgeList;
  }
  long long * getSrcList()
  {
    return srcList;
  }
  long long * getIndexofNodes()
  {
    return indexofNodes;
  }
  long long * getRevIndexofNodes()
  {
    return rev_indexofNodes;
  }

  long long  num_nodes()
  {
      return padded_nodesTotal;
  }
  long long  ori_num_nodes()
  {
      return original_nodesTotal;
  }
  long long  num_edges()
  {
      return edgesTotal;
  }
  long long  max_degree()
  {
      return degree_max;
  }

#if 0
   bool check_if_nbr(long long  s, long long  d)
    {
      long long  startEdge=indexofNodes[s];
      long long  endEdge=indexofNodes[s+1]-1;


      if(edgeList[startEdge]==d)
          return true;
      if(edgeList[endEdge]==d)
         return true;   

       long long  mid = (startEdge+endEdge)/2;

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
     	std::cerr <<"Cannot open the file"<<std::endl;
     	exit(0);
     }
     std::string line;
     std::stringstream ss;
     std::cerr << "graph recognized \n";
     while (getline(infile, line)) 
     {
       
       if (line[0] < '0' || line[0] >'9') {
          long long  k=3;
          while (k--) {
            std::cerr << "ignoring " << line << std::endl;
            getline (infile, line);
            continue;
          }

	    	}
      
        ss.clear();
        ss<<line;
        edgesTotal++; 
        
        edge e;
        long long  source;
        long long  destination;
	      long long  weight;
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

      
    // edge list made. 
     original_nodesTotal+=1;
     if(original_nodesTotal % np == 0)
     {
        padded_nodesTotal = original_nodesTotal;
     }
     else
     {
        padded_nodesTotal = original_nodesTotal + np -(original_nodesTotal%np);
     }

      for(long long  i=0;i<padded_nodesTotal;i++)
      {
       std::vector<edge> edgeOfVertex=edges[i];

     	  if(edges[i].size() > degree_max)
     		  degree_max = edges[i].size();



        sort(edgeOfVertex.begin(),edgeOfVertex.end(),
                            [](const edge& e1,const edge& e2) {

                                if(e1.source!=e2.source)
                                  return e1.source<e2.source;

                                return e1.destination<e2.destination;  

                            });

      }                      

      std::cerr << "Edges : " << ' ' << edgesTotal << "\n Vertices : " << original_nodesTotal <<std::endl;
      indexofNodes=new long long [padded_nodesTotal+1];
      edgeList=new long long [edgesTotal];
      edgeLen=new long long [edgesTotal];
      rev_indexofNodes=new long long [padded_nodesTotal+1];
      srcList=new long long [edgesTotal];

    
    long long  edge_no=0;
    
    for(long long  i=0;i<padded_nodesTotal;i++)
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
    for(long long  i=0;i<padded_nodesTotal;i++)
    {
	 std::vector <std::pair<long long ,long long >> vect;
	 for (long long  j = indexofNodes[i]; j < indexofNodes[i+1]; j++)
	 	vect.push_back(std::make_pair(edgeList[j], edgeLen[j]));
	 std::sort(vect.begin(),vect.end());
	
	 for (long long  j = 0; j < vect.size(); j++)
	 {
		edgeList[j + indexofNodes[i]] = vect[j].first;
		edgeLen[j + indexofNodes[i]] = vect[j].second;
	 }

	vect.clear();
	std::vector <std::pair<long long ,long long >>().swap(vect);
	
          //std::vector<long long > vect;
          //vect.insert(vect.begin(),edgeList+indexofNodes[i],edgeList+indexofNodes[i+1]);
          //std::sort(vect.begin(),vect.end());
          //for(long long  j=0;j<vect.size();j++)
          //  edgeList[j+indexofNodes[i]]=vect[j];

            //vect.clear();
	//	std::vector<long long >().swap(vect);

    }

cerr<<"generating reverse list\n" ;
    for(long long  i=0;i<padded_nodesTotal+1;i++)
       rev_indexofNodes[i] = 0;
   

        
    /* Prefix Sum computation for in neighbours 
       Loads rev_indexofNodes and srcList.
    */

    /* count indegrees first */
    long long * edge_indexinrevCSR = new long long [edgesTotal];

    for(long long  i=0;i<padded_nodesTotal;i++)
      {

       for(long long  j=indexofNodes[i];j<indexofNodes[i+1];j++)
           {
             long long  dest = edgeList[j];
             long long  temp = __sync_fetch_and_add( &rev_indexofNodes[dest], 1 );
             edge_indexinrevCSR[j] = temp;
           }

      }   
      /* convert to revCSR */
      long long  prefix_sum = 0;
      for(long long  i=0;i<padded_nodesTotal;i++)
        {
          long long  temp = prefix_sum;
          prefix_sum = prefix_sum + rev_indexofNodes[i];
          rev_indexofNodes[i]=temp;
      
        }
        rev_indexofNodes[padded_nodesTotal] = prefix_sum;

    /* store the sources in srcList */
      for(long long  i=0;i<padded_nodesTotal;i++)
        {
          for(long long  j=indexofNodes[i];j<indexofNodes[i+1];j++)
            {
               long long  dest = edgeList[j];
               long long  index_in_srcList=rev_indexofNodes[dest]+edge_indexinrevCSR[j];
               srcList[index_in_srcList] = i;
            }
        }
      std::cerr << "done\n";
        for(long long  i=0;i<padded_nodesTotal;i++)
        {
          std::vector<long long > vect;
          vect.insert(vect.begin(),srcList+rev_indexofNodes[i],srcList+rev_indexofNodes[i+1]);
          std::sort(vect.begin(),vect.end());
          for(long long  j=0;j<vect.size();j++)
            srcList[j+rev_indexofNodes[i]]=vect[j];

          vect.clear();
		      std::vector<long long >().swap(vect);

        }
	delete [] edge_indexinrevCSR;



      cerr << edges.size () << " this is what I should get\n" ;
  std::cerr << "okay" <<std::endl;
 }


};
