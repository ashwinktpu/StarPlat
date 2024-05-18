#include <fstream>
#include <sstream>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<string.h>
#include<climits>
#include "graph_ompv2.hpp"

//using namespace std;

class edge
{
  public:
  int32_t source;
  int32_t destination;
  int32_t weight;
  int32_t id; /* -unique Id for each edge.
                 -useful in adding properties to edges. */
  int dir;               
};



//bool counter=true;
class graph
{
  private:
  int32_t nodesTotal;
  int32_t edgesTotal;
  int32_t* edgeLen;
  int32_t* diff_edgeLen;
  int32_t* rev_edgeLen;
  int32_t* diff_rev_edgeLen;
  char* filePath;
  std::map<int32_t,std::vector<edge>> edges;
 

  public:
  int32_t* indexofNodes; /* stores prefix sum for outneighbours of a node*/
  int32_t* rev_indexofNodes; /* stores prefix sum for inneighbours of a node*/
  int32_t* edgeList; /*stores destination corresponding to edgeNo.
                       required for iteration over out neighbours */
  int32_t* srcList;  /*stores source corresponding to edgeNo.
                       required for iteration over in neighbours */
   std::vector<edge> graph_edge; 
   int32_t* diff_indexofNodes;
   int32_t* diff_edgeList; 
   int32_t* diff_rev_indexofNodes;
   int32_t* diff_rev_edgeList;
   int32_t* perNodeCSRSpace;
   int32_t* perNodeRevCSRSpace;
   int32_t* edgeMap;  
   std::map<int, int> outDeg;
   std::map<int, int> inDeg;

  graph(char* file)
  {
    filePath=file;
    nodesTotal=0;
    edgesTotal=0;
    diff_edgeList = NULL;
    diff_indexofNodes = NULL;
    diff_rev_indexofNodes = NULL;
    diff_rev_edgeList = NULL;
    rev_edgeLen = NULL;

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


  //function to get total number of edges
   int num_edges()
  {   
      if(diff_edgeList !=NULL)
        {
          return (edgesTotal + diff_indexofNodes[nodesTotal+1]);
        }
        else 
           return edgesTotal;
  }


  std::vector<edge> getInOutNbrs(int v){

  std::vector<edge> resVec;

  std::vector<edge> inEdges = getInNeighbors(v);
  resVec.insert(resVec.end(), inEdges.begin(), inEdges.end());
  std::vector<edge> Edges = getNeighbors(v);
  resVec.insert(resVec.end(), Edges.begin(), Edges.end());
  
  return resVec;

  }

  edge getEdge(int s, int d) {

    int startEdge=indexofNodes[s];
    int endEdge=indexofNodes[s+1]-1;
    edge foundEdge;

    for(edge e: getNeighbors(s)) {

      int nbr = e.destination;
      if(nbr == d) {
        return e;
      }
    }

    return foundEdge; // TODO: Maybe return a default value?
  }


  // library function to check candidate vertex is in the path from root to dest in SPT.
  bool inRouteFromSource(int candidate, int dest, int* parent)
  {

    while(parent[dest]!=-1)
      {
        if(parent[dest]==candidate)
          return true;

        dest = parent[dest];
      }

      return false;

  }
  


  bool check_if_nbr(int s, int d)
    {
      /*int startEdge=indexofNodes[s];
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
          
        
        }*/
      
      /* int start = 0;
        int end = edges[s].size()-1;
        int mid;

         while(start<end)
        {
         mid = (start+end)/2;

          if(edges[s][mid].destination==d)
             return true;

          if(d<edges[s][mid].destination)
             end=mid-1;
          else
            start=mid+1;


        }*/



        for ( edge e: getNeighbors(s))
           {
             int nbr = e.destination;
             if(nbr == d)
               return true;
           }

      return false;



    }

    int common_nbrscount(int node1, int node2)
    {
       int count=0;
       int a=indexofNodes[node1+1];
       int b=indexofNodes[node2+1];
       int i=indexofNodes[node1];
       int j=indexofNodes[node2];
       
        
      while(i<a&&j<b)
       {  
         int n = edgeList[i];
         int m = edgeList[j];
       
          if(n==m)
           {
             i++;
             j++;
             count++;
           }
           else if(n<m)
                 i++;
           else
               j++;      

       }


       return count;



    }


    int getOutDegree(int v){

     return outDeg[v];

    }

    int getInDegree(int v){
  
     return inDeg[v];

    }

    void addEdge(int src, int dest,int aks)
     {
        int startIndex = indexofNodes[src];
        int endIndex = indexofNodes[src+1];
        int nbrsCount = endIndex - startIndex ;
        int insertAt = 0;

        if(edgeList[startIndex]>=dest||nbrsCount==0)
           insertAt = startIndex;
        else if(edgeList[endIndex-1]<=dest)
             insertAt = endIndex;    
        else
         {
         
        for(int i=startIndex;i<endIndex-1;i++) // find the correct index to insert.
         {
           if(edgeList[i]<=dest && edgeList[i+1]>=dest)
              {
                insertAt = i+1;
                break;
              }
         }
         }
        
       edgeList = (int32_t*)realloc(edgeList,sizeof(int32_t)*(edgesTotal+1));
       edgeLen =  (int32_t*)realloc(edgeLen,sizeof(int32_t)*(edgesTotal+1));
        
       for(int i=edgesTotal-1;i>=insertAt;i--) //shift the elements
         { 
           edgeList[i+1] = edgeList[i];
           edgeLen[i+1] = edgeLen[i] ;
           //edgeMap[i+1] = edgeMap[i];
         }  
          
         edgeList[insertAt] = dest; 
         edgeLen[insertAt] = aks; // to be changed. the weight should be from paramters.
         
         //update the CSR offset array.
         #pragma omp parallel for
         for(int i=src+1;i<=nodesTotal+1;i++)
          {
            indexofNodes[i]+=1;
          }
          
          edge newEdge;
          newEdge.source = src;
          newEdge.destination = dest;
          newEdge.weight=aks;
          edges[src].push_back(newEdge);
          edgesTotal++;
        
     }

   void delEdge(int src, int dest)
     {
         int startEdge=indexofNodes[src];
         int endEdge=indexofNodes[src+1]-1;
         int mid ;
        
       while(startEdge<=endEdge)
        {
         mid = (startEdge+endEdge)/2;

          if(edgeList[mid]==dest)
             break;
     
          if(dest<edgeList[mid])
             endEdge=mid-1;   
          else
            startEdge=mid+1;   
          
        
        }
      
       /* int startEdge=rev_indexofNodes[dest];
         int endEdge=rev_indexofNodes[dest+1]-1;
         int mid ;

       while(startEdge<=endEdge)
        {
         mid = (startEdge+endEdge)/2;

          if(srcList[mid]==src)
             break;
     
          if(src<srcList[mid])
             endEdge=mid-1;   
          else
            startEdge=mid+1;   
          
        
        }
       */
        
        edgeLen[mid]=INT_MAX/2;

        printf("src %d dest %d mid %d\n",src, dest, mid);

     }


   std::vector<update> parseUpdates(char* updateFile)
    {

        std::vector<update> update_vec = parseUpdateFile(updateFile);  
        return update_vec;
       
    }

  std::vector<update> getDeletesFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec)
  {
    std::vector<update> deleteVec = getDeletions(updateIndex, batchSize, updateVec);
    return deleteVec;
  }  
  
  std::vector<update> getAddsFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec)
  {
    std::vector<update> addVec = getAdditions(updateIndex, batchSize, updateVec);
    return addVec;
    
  }

  void propagateNodeFlags(bool* modified){
  
    bool finished = false;
    while (!finished)
    {
      finished = true;
      
     for(int v = 0 ; v <= nodesTotal ; v++)
      for (edge e : getNeighbors(v))
      {
        if (!modified[e.destination])
        {
          modified[e.destination] = true;
          finished = false;
        }
      }
    }

  }

  

    void parseEdges() {
    //printf("OH HELLOHIHod \n");
    std::ifstream infile;
    infile.open(filePath);
    std::string line;
    std::map<std::pair<int, int>, int> mpp;
    while (std::getline(infile, line)) {
        if (line.length() == 0 || line[0] < '0' || line[0] > '9') {
            continue;
        }

        std::stringstream ss(line);
        edge e;
        int32_t source;
        int32_t destination;
        int32_t weightVal;

        ss >> source;
        if (source > nodesTotal)
            nodesTotal = source;

        ss >> destination;
        if (destination > nodesTotal)
            nodesTotal = destination;

        ss >> weightVal;

        std::pair<int, int> p;
        p.first = source;
        p.second = destination;
        mpp[p] += weightVal;
    }

    for (auto it = mpp.begin(); it != mpp.end(); ++it) {
        std::pair<int, int> key = it->first;
        int value = it->second;
        edge e;
        e.source = key.first;
        e.destination = key.second;
        e.weight = value;

        edgesTotal++;
        edges[e.source].push_back(e);
        graph_edge.push_back(e);
    }

    infile.close();
}

    void parseEdgesResidual() {
    //printf("OH HELLOHIHod \n");
    std::ifstream infile;
    infile.open(filePath);
    std::string line;
    std::map<std::pair<int, int>, int> mpp;
    while (std::getline(infile, line)) {
        if (line.length() == 0 || line[0] < '0' || line[0] > '9') {
            continue;
        }

        std::stringstream ss(line);
        edge e;
        int32_t source;
        int32_t destination;
        int32_t weightVal;

        ss >> source;
        if (source > nodesTotal)
            nodesTotal = source;

        ss >> destination;
        if (destination > nodesTotal)
            nodesTotal = destination;

        ss >> weightVal;

        std::pair<int, int> p;
        p.first = source;
        p.second = destination;
        mpp[p] += weightVal;
        p.first = destination;
        p.second = source;
        mpp[p] += 0;
    }

    for (auto it = mpp.begin(); it != mpp.end(); ++it) {
        std::pair<int, int> key = it->first;
        int value = it->second;
        edge e;
        e.source = key.first;
        e.destination = key.second;
        e.weight = value;

        edgesTotal++;
        edges[e.source].push_back(e);
        graph_edge.push_back(e);
    }

    infile.close();
}

    void parseGraphResidual()
   {  
     
     
     parseEdgesResidual();
   
     printf("Here half\n");
     //printf("HELLO AFTER THIS %d \n",nodesTotal);
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
     edgeList=new int32_t[edgesTotal]; //new int32_t[edgesTotal] ;
     srcList=new int32_t[edgesTotal];
     edgeLen=new int32_t[edgesTotal]; //new int32_t[edgesTotal] ;
     edgeMap =  new int32_t[edgesTotal]; 
     perNodeCSRSpace = new int32_t[nodesTotal+1];
     perNodeRevCSRSpace = new int32_t[nodesTotal+1];
     int* edgeMapInter = new int32_t[edgesTotal]; 
     int* vertexInter = new int32_t[edgesTotal];         
     
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

      perNodeCSRSpace[i] = 0;
      perNodeRevCSRSpace[i] = 0;
      
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
               edgeMapInter[index_in_srcList] = j ;// RevCSR to CSR edge mapping.
               vertexInter[index_in_srcList] = srcList[index_in_srcList]; /*store the original content of srcList
                                                                           before sorting srcList.
                                                                           */                                                          
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
          int srcListIndex;  

          for(int j=0;j<vect.size();j++)
          {
            srcListIndex = j+rev_indexofNodes[i]; 
            for(int k=0;k<vect.size();k++)
              {
                if(vertexInter[k+rev_indexofNodes[i]]==srcList[srcListIndex])
                  {
                    edgeMap[srcListIndex] = edgeMapInter[k+rev_indexofNodes[i]];
                    break;
                  }
              }
          }
            vect.clear();

        }
     
      
      for(int i = 0 ; i <= nodesTotal ; i++){
         
         inDeg[i] = rev_indexofNodes[i+1] - rev_indexofNodes[i];
         outDeg[i] = indexofNodes[i+1] - indexofNodes[i];
     }
      free(vertexInter);
      free(edgeMapInter);
    //change to nodesTotal+1.
    // printf("hello after this %d %d\n",nodesTotal,edgesTotal);
    
 }
   
   
   void parseGraph()
   {  
     
     
     parseEdges();
   
     printf("Here half\n");
     //printf("HELLO AFTER THIS %d \n",nodesTotal);
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
     edgeList=new int32_t[edgesTotal]; //new int32_t[edgesTotal] ;
     srcList=new int32_t[edgesTotal];
     edgeLen=new int32_t[edgesTotal]; //new int32_t[edgesTotal] ;
     edgeMap =  new int32_t[edgesTotal]; 
     perNodeCSRSpace = new int32_t[nodesTotal+1];
     perNodeRevCSRSpace = new int32_t[nodesTotal+1];
     int* edgeMapInter = new int32_t[edgesTotal]; 
     int* vertexInter = new int32_t[edgesTotal];         
     
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

      perNodeCSRSpace[i] = 0;
      perNodeRevCSRSpace[i] = 0;
      
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
               edgeMapInter[index_in_srcList] = j ;// RevCSR to CSR edge mapping.
               vertexInter[index_in_srcList] = srcList[index_in_srcList]; /*store the original content of srcList
                                                                           before sorting srcList.
                                                                           */                                                          
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
          int srcListIndex;  

          for(int j=0;j<vect.size();j++)
          {
            srcListIndex = j+rev_indexofNodes[i]; 
            for(int k=0;k<vect.size();k++)
              {
                if(vertexInter[k+rev_indexofNodes[i]]==srcList[srcListIndex])
                  {
                    edgeMap[srcListIndex] = edgeMapInter[k+rev_indexofNodes[i]];
                    break;
                  }
              }
          }
            vect.clear();

        }
     
      
      for(int i = 0 ; i <= nodesTotal ; i++){
         
         inDeg[i] = rev_indexofNodes[i+1] - rev_indexofNodes[i];
         outDeg[i] = indexofNodes[i+1] - indexofNodes[i];
     }
      free(vertexInter);
      free(edgeMapInter);
    //change to nodesTotal+1.
    // printf("hello after this %d %d\n",nodesTotal,edgesTotal);
    
 }



 /******************************|| Dynamic Graph Libraries ||********************************/


 void updateCSRDel(std::vector<update>& batchUpdate, int k, int size)
  {
    int num_nodes = nodesTotal + 1;
    std::vector<std::pair<int,int>> perNodeUpdateInfo;
    std::vector<std::pair<int,int>> perNodeUpdateRevInfo;
    std::vector<update> slicedUpdates;
    if(rev_edgeLen==NULL)
       rev_edgeLen = new int[edgesTotal];
    for(int i=0;i<num_nodes;i++)
     {
       perNodeUpdateInfo.push_back({0,0});
       perNodeUpdateRevInfo.push_back({0,0});
     }

   // printf("size %d \n", size);
    /* perNode bookkeeping of updates and deletions */

    for(int i=0;i<size;i++)
     { 
       int pos = k+i;
       update u = batchUpdate[pos];
       int source = u.source;
       int destination = u.destination;
       char type = u.type;
       if(type == 'a')
         {
         perNodeUpdateInfo[source].second++;
         perNodeUpdateRevInfo[destination].second++;
         }
       else
         {
         perNodeUpdateInfo[source].first++; 
         perNodeUpdateRevInfo[destination].first++;
         }
           
         slicedUpdates.push_back(u);

    }
   

   /* edge weights book-keeping for reverse CSR */

    #pragma omp parallel for
    for(int i=0;i<edgesTotal;i++)
       {
        /* int e = edgeMap[i];
         int weight = edgeLen[e];*/
         rev_edgeLen[i] = 1 ;//weight;

       }

    updateCSRDel_omp(num_nodes, edgesTotal, indexofNodes, edgeList, rev_indexofNodes,  srcList, edgeLen, diff_edgeLen,
                  diff_indexofNodes, diff_edgeList, diff_rev_indexofNodes, diff_rev_edgeList, rev_edgeLen,
                  diff_rev_edgeLen,perNodeUpdateInfo, perNodeUpdateRevInfo, perNodeCSRSpace, perNodeRevCSRSpace,slicedUpdates) ;

    perNodeUpdateInfo.clear();
    perNodeUpdateRevInfo.clear();

    for(int i = 0 ; i <= nodesTotal ; i++){
         
         inDeg[i] = getInNeighbors(i).size();
         outDeg[i] = getNeighbors(i).size();
     }

  }


 void updateCSRAdd(std::vector<update>& batchUpdate, int k, int size)
  {
    int num_nodes = nodesTotal + 1;

    std::vector<std::pair<int,int>> perNodeUpdateInfo;
    std::vector<std::pair<int,int>> perNodeUpdateRevInfo;
    std::vector<update> slicedUpdates;
    
    for(int i=0;i<num_nodes;i++)
     {
       perNodeUpdateInfo.push_back({0,0});
       perNodeUpdateRevInfo.push_back({0,0});
     }

      for(int i=0;i<size;i++)
     { 
       int pos = k+i;
       update u = batchUpdate[pos];
       int source = u.source;
       int destination = u.destination;
       char type = u.type;
       if(type == 'a')
         {
         perNodeUpdateInfo[source].second++;
         perNodeUpdateRevInfo[destination].second++;
         }
       else
         {
         perNodeUpdateInfo[source].first++; 
         perNodeUpdateRevInfo[destination].first++;
         }
           
         slicedUpdates.push_back(u);

    }

    
  
    updateCSRAdd_omp(num_nodes, edgesTotal, indexofNodes, edgeList, rev_indexofNodes,  srcList, edgeLen, &diff_edgeLen,
                  &diff_indexofNodes, &diff_edgeList, &diff_rev_indexofNodes, &diff_rev_edgeList, rev_edgeLen,
                  &diff_rev_edgeLen,perNodeUpdateInfo, perNodeUpdateRevInfo,perNodeCSRSpace, perNodeRevCSRSpace,slicedUpdates) ;

    perNodeUpdateInfo.clear();
    perNodeUpdateRevInfo.clear();

     for(int i = 0 ; i <= nodesTotal ; i++){
         
         inDeg[i] = getInNeighbors(i).size();
         outDeg[i] = getNeighbors(i).size();
     }

  }




std::vector<edge> getNeighbors( int node)
 {
   
   std::vector<edge> out_edges;

   for(int i=indexofNodes[node]; i<indexofNodes[node+1] ; i++)
     {
       int nbr = edgeList[i] ;
       if(nbr!=INT_MAX/2)
         {
           edge e ;
           e.source = node;
           e.destination = nbr;
           e.weight = edgeLen[i];
           e.id = i;
           e.dir = 1;
         //  printf(" weight %d\n", e.weight);
           out_edges.push_back(e);
         }
     }  
     
     if(diff_edgeList!=NULL)
       {
     for(int j=diff_indexofNodes[node]; j<diff_indexofNodes[node+1] ; j++)
       {    
         int nbr = diff_edgeList[j];
          if(nbr!=INT_MAX/2)
          {
            edge e;
            e.source = node;
            e.destination = nbr;
            e.weight = diff_edgeLen[j];
            e.id = edgesTotal + j ;
            e.dir = 1;
           // printf(" weight %d\n", e.weight);
            out_edges.push_back(e);
          }

       }
   }  
    

     return out_edges;
   
  }
 

 std::vector<edge> getInNeighbors( int node)
 {
   
   std::vector<edge> in_edges;

   for(int i=rev_indexofNodes[node]; i<rev_indexofNodes[node+1] ; i++)
     {
       int nbr = srcList[i] ;
       if(nbr!=INT_MAX/2)
         {
           edge e ;
           e.source = node;
           e.destination = nbr;
           e.weight = rev_edgeLen[i];
           in_edges.push_back(e);
           e.dir = 0;
         }
     }  
     
     if(diff_rev_edgeList!=NULL)
      {
     for(int j=diff_rev_indexofNodes[node]; j<diff_rev_indexofNodes[node+1] ; j++)
       {
         int nbr = diff_rev_edgeList[j];
          if(nbr!=INT_MAX/2)
          {
            edge e;
            e.source = node;
            e.destination = nbr;
            e.weight = diff_rev_edgeLen[j];
            in_edges.push_back(e);
            e.dir = 0;
          }

       }

      }
    
     return in_edges;
   
  }

};
