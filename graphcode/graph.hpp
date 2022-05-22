#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string.h>
#include "graph_ompv2.hpp"
#include "graph_cuda.cuh"

class edge
{
public:
  int32_t source;
  int32_t destination;
  int32_t weight;
  int32_t id; // unique ID for each edge. useful in adding properties to edges
};

class graph
{
private:
  int32_t nodesTotal;                         //// total number of nodes present in the graph
  int32_t edgesTotal;                         //// total number of edges present in CSR/CSC
  int32_t *edgeLen;                           //// for storing edge weights (CSR)
  int32_t *diff_edgeLen;                      //// for storing edge weights (diffCSR)
  int32_t *rev_edgeLen;                       //// for storing edge weights (CSC)
  int32_t *diff_rev_edgeLen;                  //// for storing edge weights (diffCSC)
  char *filePath;                             //// path to the input file
  std::string backend;                        //// backend being used
  bool isDirected;                            //// type of graph (directed/undirected)
  std::map<int32_t, std::vector<edge>> edges; //// stores out-neighbours. key=node, value=out-neighbours

public:
  int32_t *indexofNodes;          //// stores offsets of edges in CSR
  int32_t *diff_indexofNodes;     //// stores offsets of edges in diffCSR
  int32_t *rev_indexofNodes;      //// stores offsets of edges in CSC
  int32_t *diff_rev_indexofNodes; //// stores offsets of edges in diffCSC
  int32_t *edgeList;              //// stores edges in CSR
  int32_t *diff_edgeList;         //// stores edges in diffCSR
  int32_t *srcList;               //// stores edges in CSC
  int32_t *diff_rev_edgeList;     //// stores edges in diffCSC
  std::vector<edge> graph_edge;   //// stores all the edges
  int32_t *perNodeCSRSpace;       //// stores available space in CSR
  int32_t *perNodeRevCSRSpace;    //// stores available space in CSC
  int32_t *edgeMap;               ////

  graph(char *file, std::string bkend = "openmp", bool dir = true)
  {
    filePath = file;
    backend = bkend;
    isDirected = dir;
    nodesTotal = 0;
    edgesTotal = 0;
    edgeLen = NULL;
    diff_edgeLen = NULL;
    rev_edgeLen = NULL;
    diff_rev_edgeLen = NULL;
    indexofNodes = NULL;
    diff_indexofNodes = NULL;
    rev_indexofNodes = NULL;
    diff_rev_indexofNodes = NULL;
    edgeList = NULL;
    diff_edgeList = NULL;
    srcList = NULL;
    diff_rev_edgeList = NULL;
    perNodeCSRSpace = NULL;
    perNodeRevCSRSpace = NULL;
    edgeMap = NULL;
  }

  void parseEdges()
  {
    std::ifstream infile;
    infile.open(filePath);
    std::string line;

    while (std::getline(infile, line))
    {
      if (line.length() == 0 || line[0] < '0' || line[0] > '9')
        continue;

      std::stringstream ss(line);
      edgesTotal++;

      edge e;
      int32_t source;
      int32_t destination;
      // int32_t weightVal;

      ss >> source;
      if (source > nodesTotal)
        nodesTotal = source;

      ss >> destination;
      if (destination > nodesTotal)
        nodesTotal = destination;

      // ss >> weightVal; // for edgelists having weight too

      e.source = source;
      e.destination = destination;
      e.weight = 1; // weightVal

      edges[source].push_back(e);
      graph_edge.push_back(e);
    }

    infile.close();
  }

  void parseGraph()
  {
    parseEdges();

    #pragma omp parallel for
    for (int i = 0; i <= nodesTotal; i++)
    {
      std::vector<edge> &edgeOfVertex = edges[i];

      sort(edgeOfVertex.begin(), edgeOfVertex.end(),
           [](const edge &e1, const edge &e2)
           {
             if (e1.source != e2.source)
               return e1.source < e2.source;
             return e1.destination < e2.destination;
           });
    }

    indexofNodes = new int32_t[nodesTotal + 2];
    rev_indexofNodes = new int32_t[nodesTotal + 2];
    diff_indexofNodes = new int32_t[nodesTotal + 2];
    diff_rev_indexofNodes = new int32_t[nodesTotal + 2];
    edgeList = new int32_t[edgesTotal];
    srcList = new int32_t[edgesTotal];
    edgeLen = new int32_t[edgesTotal];
    rev_edgeLen = new int32_t[edgesTotal];
    edgeMap = new int32_t[edgesTotal];
    perNodeCSRSpace = new int32_t[nodesTotal + 1];
    perNodeRevCSRSpace = new int32_t[nodesTotal + 1];
    int *edgeMapInter = new int32_t[edgesTotal]; // CSC to CSR edge mapping
    int *vertexInter = new int32_t[edgesTotal];  // store the original content of srcList before sorting srcList
    int edge_no = 0;

    // form CSR (edgeList) & compute offsets for CSR (indexofNodes)
    for (int i = 0; i <= nodesTotal; i++)
    {
      std::vector<edge> edgeofVertex = edges[i];
      indexofNodes[i] = edge_no;
      diff_indexofNodes[i] = 0;
      diff_rev_indexofNodes[i] = 0;
      std::vector<edge>::iterator itr;
      for (itr = edgeofVertex.begin(); itr != edgeofVertex.end(); itr++)
      {
        edgeList[edge_no] = (*itr).destination;
        edgeLen[edge_no] = (*itr).weight;
        edge_no++;
      }

      perNodeCSRSpace[i] = 0;
      perNodeRevCSRSpace[i] = 0;
    }
    indexofNodes[nodesTotal + 1] = edge_no;
    diff_indexofNodes[nodesTotal + 1] = 0;
    diff_rev_indexofNodes[nodesTotal + 1] = 0;

    // form CSC (srcList) & compute offsets for CSC (rev_indexofNodes)

    #pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal + 1; i++)
      rev_indexofNodes[i] = 0;

    // count indegrees
    int32_t *edge_indexinrevCSR = new int32_t[edgesTotal];

    #pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
      {
        int dest = edgeList[j];
        int temp = __sync_fetch_and_add(&rev_indexofNodes[dest], 1);
        edge_indexinrevCSR[j] = temp;
      }
    }

    // compute offsets for CSC (rev_indexofNodes)
    int prefix_sum = 0;
    for (int i = 0; i <= nodesTotal; i++)
    {
      int temp = prefix_sum;
      prefix_sum = prefix_sum + rev_indexofNodes[i];
      rev_indexofNodes[i] = temp;
    }
    rev_indexofNodes[nodesTotal + 1] = prefix_sum;

    // store the sources in srcList
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
      {
        int dest = edgeList[j];
        int index_in_srcList = rev_indexofNodes[dest] + edge_indexinrevCSR[j];
        srcList[index_in_srcList] = i;
        rev_edgeLen[index_in_srcList] = edgeLen[j];
        edgeMapInter[index_in_srcList] = j;
        vertexInter[index_in_srcList] = srcList[index_in_srcList];
      }
    }

    #pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      std::vector<int> vect;
      vect.insert(vect.begin(), srcList + rev_indexofNodes[i], srcList + rev_indexofNodes[i + 1]);
      std::sort(vect.begin(), vect.end());
      for (int j = 0; j < vect.size(); j++)
        srcList[j + rev_indexofNodes[i]] = vect[j];

      int srcListIndex;
      for (int j = 0; j < vect.size(); j++)
      {
        srcListIndex = j + rev_indexofNodes[i];
        for (int k = 0; k < vect.size(); k++)
        {
          if (vertexInter[k + rev_indexofNodes[i]] == srcList[srcListIndex])
          {
            edgeMap[srcListIndex] = edgeMapInter[k + rev_indexofNodes[i]];
            break;
          }
        }
      }
      vect.clear();
    }

    free(vertexInter);
    free(edgeMapInter);
    free(edge_indexinrevCSR);
  }

  std::map<int, std::vector<edge>> getEdges()
  {
    return edges;
  }

  int *getEdgeLen()
  {
    return edgeLen;
  }

  int num_nodes()
  {
    return nodesTotal + 1;
  }

  int num_edges()
  {
    if (diff_edgeList != NULL)
      return (edgesTotal + diff_indexofNodes[nodesTotal + 1]);
    else
      return edgesTotal;
  }

  int num_edges_CSR()
  {
    return edgesTotal;
  }

  int num_edges_diffCSR()
  {
    if (diff_edgeList == NULL)
      return 0;
    else
      return diff_indexofNodes[nodesTotal + 1];
  }

  int num_edges_diffCSC()
  {
    if (diff_rev_edgeList == NULL)
      return 0;
    else
      return diff_rev_indexofNodes[nodesTotal + 1];
  }

  bool isGraphDirected()
  {
    return isDirected;
  }

  edge getEdge(int s, int d)
  {
    for (edge e : getNeighbors(s))
    {
      if (e.destination == d)
        return e;
    }
  }

  // function to check whether candidate vertex is in the path from root to dest in SPT
  bool inRouteFromSource(int candidate, int dest, int *parent)
  {
    if (candidate == dest)
      return true;

    while (parent[dest] != -1)
    {
      if (parent[dest] == candidate)
        return true;
      dest = parent[dest];
    }
    return false;
  }

  bool check_if_nbr(int s, int d)
  {
    for (edge e : getNeighbors(s))
    {
      if (e.destination == d)
        return true;
    }
    return false;
  }

  int common_nbrscount(int node1, int node2)
  {
    int count = 0;
    int i = indexofNodes[node1];
    int j = indexofNodes[node2];
    int a = indexofNodes[node1 + 1];
    int b = indexofNodes[node2 + 1];

    while (i < a && j < b)
    {
      int n = edgeList[i];
      int m = edgeList[j];

      if (n == m)
      {
        i++;
        j++;
        count++;
      }
      else if (n < m)
        i++;
      else
        j++;
    }

    return count;
  }

  // for adding one edge into CSR (one update at a time)
  void addEdge(int src, int dest)
  {
    int startIndex = indexofNodes[src];
    int endIndex = indexofNodes[src + 1];
    int nbrsCount = endIndex - startIndex;
    int insertAt = 0;

    // find the correct index to insert
    if (edgeList[startIndex] >= dest || nbrsCount == 0)
      insertAt = startIndex;
    else if (edgeList[endIndex - 1] <= dest)
      insertAt = endIndex;
    else
    {
      for (int i = startIndex; i < endIndex - 1; i++)
      {
        if (edgeList[i] <= dest && edgeList[i + 1] >= dest)
        {
          insertAt = i + 1;
          break;
        }
      }
    }

    edgeList = (int32_t *)realloc(edgeList, sizeof(int32_t) * (edgesTotal + 1));
    edgeLen = (int32_t *)realloc(edgeLen, sizeof(int32_t) * (edgesTotal + 1));

    // shift the elements
    for (int i = edgesTotal - 1; i >= insertAt; i--)
    {
      edgeList[i + 1] = edgeList[i];
      edgeLen[i + 1] = edgeLen[i];
    }

    edgeList[insertAt] = dest;
    edgeLen[insertAt] = 1; // to be changed. the weight should be from paramters.

    // update the CSR offset array.
    #pragma omp parallel for
    for (int i = src + 1; i <= nodesTotal + 1; i++)
    {
      indexofNodes[i] += 1;
    }

    edge newEdge;
    newEdge.source = src;
    newEdge.destination = dest;
    edges[src].push_back(newEdge);
    edgesTotal++;
  }

  // for deleting one edge from CSR (one update at a time)
  void delEdge(int src, int dest)
  {
    int startEdge = indexofNodes[src];
    int endEdge = indexofNodes[src + 1] - 1;
    int mid;

    while (startEdge <= endEdge)
    {
      mid = (startEdge + endEdge) / 2;

      if (edgeList[mid] == dest)
        break;
      if (dest < edgeList[mid])
        endEdge = mid - 1;
      else
        startEdge = mid + 1;
    }

    edgeLen[mid] = INT_MAX / 2;
    // printf("src %d dest %d mid %d\n", src, dest, mid);
  }

  std::vector<edge> getNeighbors(int node)
  {

    std::vector<edge> out_edges;

    for (int i = indexofNodes[node]; i < indexofNodes[node + 1]; i++)
    {
      int nbr = edgeList[i];
      if (nbr != INT_MAX / 2)
      {
        edge e;
        e.source = node;
        e.destination = nbr;
        e.weight = edgeLen[i];
        e.id = i;
        //  printf(" weight %d\n", e.weight);
        out_edges.push_back(e);
      }
    }

    if (diff_edgeList != NULL)
    {
      for (int j = diff_indexofNodes[node]; j < diff_indexofNodes[node + 1]; j++)
      {
        int nbr = diff_edgeList[j];
        if (nbr != INT_MAX / 2)
        {
          edge e;
          e.source = node;
          e.destination = nbr;
          e.weight = diff_edgeLen[j];
          e.id = edgesTotal + j;
          // printf(" weight %d\n", e.weight);
          out_edges.push_back(e);
        }
      }
    }

    return out_edges;
  }

  std::vector<edge> getInNeighbors(int node)
  {

    std::vector<edge> in_edges;

    for (int i = rev_indexofNodes[node]; i < rev_indexofNodes[node + 1]; i++)
    {
      int nbr = srcList[i];
      if (nbr != INT_MAX / 2)
      {
        edge e;
        e.source = node;
        e.destination = nbr;
        e.weight = rev_edgeLen[i];
        in_edges.push_back(e);
      }
    }

    if (diff_rev_edgeList != NULL)
    {
      for (int j = diff_rev_indexofNodes[node]; j < diff_rev_indexofNodes[node + 1]; j++)
      {
        int nbr = diff_rev_edgeList[j];
        if (nbr != INT_MAX / 2)
        {
          edge e;
          e.source = node;
          e.destination = nbr;
          e.weight = diff_rev_edgeLen[j];
          in_edges.push_back(e);
        }
      }
    }

    return in_edges;
  }

  std::vector<update> parseUpdates(char *updateFile)
  {
    std::vector<update> update_vec = parseUpdateFile(updateFile, isDirected);
    return update_vec;
  }

  // GPU specific check for neighbours for TC algorithm
  __device__ bool findNeighborSorted(int s, int d, int *gpu_OA, int *gpu_edgeList)
  {
    int startEdge = gpu_OA[s];
    int endEdge = gpu_OA[s + 1] - 1;

    if (gpu_edgeList[startEdge] == d)
      return true;
    if (gpu_edgeList[endEdge] == d)
      return true;

    int mid = (startEdge + endEdge) / 2;

    while (startEdge <= endEdge)
    {
      if (gpu_edgeList[mid] == d)
        return true;

      if (d < gpu_edgeList[mid])
        endEdge = mid - 1;
      else
        startEdge = mid + 1;

      mid = (startEdge + endEdge) / 2;
    }

    return false;
  }

  /******************************|| Dynamic Graph Libraries ||********************************/
  void updateCSR_Del(std::vector<update> &batchUpdate, int k, int size)
  {
    if (backend == "openmp")
      updateCSR_Del_omp(batchUpdate, k, size);
    else if (backend == "cuda")
      updateCSR_Del_cuda(batchUpdate, k, size);
  }

  void updateCSR_Add(std::vector<update> &batchUpdate, int k, int size)
  {
    if (backend == "openmp")
      updateCSR_Add_omp(batchUpdate, k, size);
    else if (backend == "cuda")
      updateCSR_Add_cuda(batchUpdate, k, size);
  }

  void updateCSR_Del_omp(std::vector<update> &batchUpdate, int k, int size)
  {
    int num_nodes = nodesTotal + 1;
    std::vector<std::pair<int, int>> perNodeUpdateInfo;
    std::vector<std::pair<int, int>> perNodeUpdateRevInfo;
    std::vector<update> slicedUpdates;
    if (rev_edgeLen == NULL)
      rev_edgeLen = new int[edgesTotal];
    for (int i = 0; i < num_nodes; i++)
    {
      perNodeUpdateInfo.push_back({0, 0});
      perNodeUpdateRevInfo.push_back({0, 0});
    }

    // printf("size %d \n", size);
    // perNode bookkeeping of updates and deletions

    for (int i = 0; i < size; i++)
    {
      int pos = k + i;
      update u = batchUpdate[pos];
      int source = u.source;
      int destination = u.destination;
      char type = u.type;
      if (type == 'a')
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
    for (int i = 0; i < edgesTotal; i++)
    {
      /* int e = edgeMap[i];
        int weight = edgeLen[e];*/
      rev_edgeLen[i] = 1; // weight;
    }

    updateCSRDel_omp(num_nodes, edgesTotal, indexofNodes, edgeList, rev_indexofNodes, srcList, edgeLen, diff_edgeLen,
                     diff_indexofNodes, diff_edgeList, diff_rev_indexofNodes, diff_rev_edgeList, rev_edgeLen,
                     diff_rev_edgeLen, perNodeUpdateInfo, perNodeUpdateRevInfo, perNodeCSRSpace, perNodeRevCSRSpace, slicedUpdates);

    perNodeUpdateInfo.clear();
    perNodeUpdateRevInfo.clear();
  }

  void updateCSR_Add_omp(std::vector<update> &batchUpdate, int k, int size)
  {
    int num_nodes = nodesTotal + 1;

    std::vector<std::pair<int, int>> perNodeUpdateInfo;
    std::vector<std::pair<int, int>> perNodeUpdateRevInfo;
    std::vector<update> slicedUpdates;

    for (int i = 0; i < num_nodes; i++)
    {
      perNodeUpdateInfo.push_back({0, 0});
      perNodeUpdateRevInfo.push_back({0, 0});
    }

    for (int i = 0; i < size; i++)
    {
      int pos = k + i;
      update u = batchUpdate[pos];
      int source = u.source;
      int destination = u.destination;
      char type = u.type;
      if (type == 'a')
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

    updateCSRAdd_omp(num_nodes, edgesTotal, indexofNodes, edgeList, rev_indexofNodes, srcList, edgeLen, &diff_edgeLen,
                     &diff_indexofNodes, &diff_edgeList, &diff_rev_indexofNodes, &diff_rev_edgeList, rev_edgeLen,
                     &diff_rev_edgeLen, perNodeUpdateInfo, perNodeUpdateRevInfo, perNodeCSRSpace, perNodeRevCSRSpace, slicedUpdates);

    perNodeUpdateInfo.clear();
    perNodeUpdateRevInfo.clear();
  }

  void updateCSR_Del_cuda(std::vector<update> &batchUpdate, int k, int size)
  {
    // data on host
    int32_t V = num_nodes();
    int32_t E = num_edges_CSR();
    int32_t diffE = num_edges_diffCSR();
    int32_t diffRevE = num_edges_diffCSC();

    // data on device
    update *d_batch;
    int32_t *d_indexofNodes;
    int32_t *d_edgeList;
    int32_t *d_rev_indexofNodes;
    int32_t *d_srcList;
    int32_t *d_diff_indexofNodes;
    int32_t *d_diff_edgeList;
    int32_t *d_diff_rev_indexofNodes;
    int32_t *d_diff_rev_edgeList;

    // allocate memory on device
    cudaMalloc(&d_batch, sizeof(update) * (size));
    cudaMalloc(&d_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_edgeList, sizeof(int32_t) * (E));
    cudaMalloc(&d_rev_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_srcList, sizeof(int32_t) * (E));
    cudaMalloc(&d_diff_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_diff_edgeList, sizeof(int32_t) * (diffE));
    cudaMalloc(&d_diff_rev_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_diff_rev_edgeList, sizeof(int32_t) * (diffRevE));

    // copy to device
    cudaMemcpy(d_batch, &(batchUpdate[k]), sizeof(update) * (size), cudaMemcpyHostToDevice);
    cudaMemcpy(d_indexofNodes, indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_edgeList, edgeList, sizeof(int32_t) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rev_indexofNodes, rev_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_srcList, srcList, sizeof(int32_t) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_indexofNodes, diff_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_edgeList, diff_edgeList, sizeof(int32_t) * (diffE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_rev_indexofNodes, diff_rev_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_rev_edgeList, diff_rev_edgeList, sizeof(int32_t) * (diffRevE), cudaMemcpyHostToDevice);

    // launch config
    const int numThreads = 1024;
    const int numBlocks = (size + numThreads - 1) / numThreads;

    // call kernel
    del_edges_CSR<<<numBlocks, numThreads>>>(size, d_batch, d_indexofNodes, d_edgeList,
                                             d_rev_indexofNodes, d_srcList);
    if (diff_edgeList != NULL)
      del_edges_diffCSR<<<numBlocks, numThreads>>>(size, d_batch,
                                                   d_diff_indexofNodes, d_diff_edgeList,
                                                   d_diff_rev_indexofNodes, d_diff_rev_edgeList);

    // copy to host
    cudaMemcpy(edgeList, d_edgeList, sizeof(int32_t) * (E), cudaMemcpyDeviceToHost);
    cudaMemcpy(srcList, d_srcList, sizeof(int32_t) * (E), cudaMemcpyDeviceToHost);
    cudaMemcpy(diff_edgeList, d_diff_edgeList, sizeof(int32_t) * (diffE), cudaMemcpyDeviceToHost);
    cudaMemcpy(diff_rev_edgeList, d_diff_rev_edgeList, sizeof(int32_t) * (diffRevE), cudaMemcpyDeviceToHost);

    // free up the memory
    cudaFree(d_batch);
    cudaFree(d_indexofNodes);
    cudaFree(d_edgeList);
    cudaFree(d_rev_indexofNodes);
    cudaFree(d_srcList);
    cudaFree(d_diff_indexofNodes);
    cudaFree(d_diff_edgeList);
    cudaFree(d_diff_rev_indexofNodes);
    cudaFree(d_diff_rev_edgeList);
  }

  void updateCSR_Add_cuda(std::vector<update> &batchUpdate, int k, int size)
  {
    // data on host
    int32_t V = num_nodes();
    int32_t E = num_edges_CSR();
    int32_t diffE = num_edges_diffCSR();
    int32_t diffRevE = num_edges_diffCSC();

    // data on device
    update *d_batch;
    int32_t *d_indexofNodes;
    int32_t *d_edgeList;
    int32_t *d_edgeLen;
    int32_t *d_rev_indexofNodes;
    int32_t *d_srcList;
    int32_t *d_rev_edgeLen;
    int32_t *d_diff_indexofNodes;
    int32_t *d_diff_edgeList;
    int32_t *d_diff_edgeLen;
    int32_t *d_diff_rev_indexofNodes;
    int32_t *d_diff_rev_edgeList;
    int32_t *d_diff_rev_edgeLen;

    // allocate memory on device
    cudaMalloc(&d_batch, sizeof(update) * (size));
    cudaMalloc(&d_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_edgeList, sizeof(int32_t) * (E));
    cudaMalloc(&d_edgeLen, sizeof(int32_t) * (E));
    cudaMalloc(&d_rev_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_srcList, sizeof(int32_t) * (E));
    cudaMalloc(&d_rev_edgeLen, sizeof(int32_t) * (E));
    cudaMalloc(&d_diff_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_diff_edgeList, sizeof(int32_t) * (diffE));
    cudaMalloc(&d_diff_edgeLen, sizeof(int32_t) * (diffE));
    cudaMalloc(&d_diff_rev_indexofNodes, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_diff_rev_edgeList, sizeof(int32_t) * (diffRevE));
    cudaMalloc(&d_diff_rev_edgeLen, sizeof(int32_t) * (diffRevE));

    // copy to device
    cudaMemcpy(d_batch, &(batchUpdate[k]), sizeof(update) * (size), cudaMemcpyHostToDevice);
    cudaMemcpy(d_indexofNodes, indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_edgeList, edgeList, sizeof(int32_t) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_edgeLen, edgeLen, sizeof(int32_t) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rev_indexofNodes, rev_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_srcList, srcList, sizeof(int32_t) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rev_edgeLen, rev_edgeLen, sizeof(int32_t) * (E), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_indexofNodes, diff_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_edgeList, diff_edgeList, sizeof(int32_t) * (diffE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_edgeLen, diff_edgeLen, sizeof(int32_t) * (diffE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_rev_indexofNodes, diff_rev_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_rev_edgeList, diff_rev_edgeList, sizeof(int32_t) * (diffRevE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_diff_rev_edgeLen, diff_rev_edgeLen, sizeof(int32_t) * (diffRevE), cudaMemcpyHostToDevice);

    // launch config
    const int numThreads = 1024;
    const int numBlocksB = (size + numThreads - 1) / numThreads;
    const int numBlocksV = (V + numThreads - 1) / numThreads;

    // new diffCSR
    int *d_diffCsrOffsets_next, *d_diffCsrCords_next, *d_diffCsrWeights_next;
    int *d_diffCsrOffsetsR_next, *d_diffCsrCordsR_next, *d_diffCsrWeightsR_next;
    cudaMalloc(&d_diffCsrOffsets_next, sizeof(int32_t) * (V + 1));
    cudaMalloc(&d_diffCsrOffsetsR_next, sizeof(int32_t) * (V + 1));
    init_kernel<int><<<numBlocksV + 1, numThreads>>>(d_diffCsrOffsets_next, 0, V + 1);
    init_kernel<int><<<numBlocksV + 1, numThreads>>>(d_diffCsrOffsetsR_next, 0, V + 1);

    // try adding add edges to CSR
    add_edges_CSR<<<numBlocksB, numThreads>>>(size, d_batch, d_indexofNodes, d_edgeList, d_edgeLen,
                                              d_rev_indexofNodes, d_srcList, d_rev_edgeLen,
                                              d_diffCsrOffsets_next, d_diffCsrOffsetsR_next);

    // get the count of valid edges in old diffCSR (for computing offsets for new diffCSR)
    add_diff_edge_counts<<<numBlocksV, numThreads>>>(V, d_diff_indexofNodes, d_diff_edgeList, d_diff_edgeLen,
                                                     d_diff_rev_indexofNodes, d_diff_rev_edgeList, d_diff_rev_edgeLen,
                                                     d_diffCsrOffsets_next, d_diffCsrOffsetsR_next);

    // compute offsets
    int *d_tempArr, *d_tempArrR;
    cudaMalloc(&d_tempArr, sizeof(int) * (V + 1));
    cudaMalloc(&d_tempArrR, sizeof(int) * (V + 1));
    copy<int><<<numBlocksV + 1, numThreads>>>(d_tempArr, d_diffCsrOffsets_next, V + 1);
    copy<int><<<numBlocksV + 1, numThreads>>>(d_tempArrR, d_diffCsrOffsetsR_next, V + 1);

    for (int off = 1; off <= V; off *= 2)
    {
      prefix_sum<<<numBlocksV + 1, numThreads>>>(V, off, d_diffCsrOffsets_next,
                                                 d_diffCsrOffsetsR_next, d_tempArr, d_tempArrR);
      copy<int><<<numBlocksV + 1, numThreads>>>(d_tempArr, d_diffCsrOffsets_next, V + 1);
      copy<int><<<numBlocksV + 1, numThreads>>>(d_tempArrR, d_diffCsrOffsetsR_next, V + 1);
    }

    cudaFree(d_tempArr);
    cudaFree(d_tempArrR);

    // populate new diffCSR
    cudaMemcpy(&diffE, &d_diffCsrOffsets_next[V], sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&diffRevE, &d_diffCsrOffsetsR_next[V], sizeof(int), cudaMemcpyDeviceToHost);
    cudaMalloc(&d_diffCsrCords_next, sizeof(int) * (diffE));
    cudaMalloc(&d_diffCsrWeights_next, sizeof(int) * (diffE));
    cudaMalloc(&d_diffCsrCordsR_next, sizeof(int) * (diffRevE));
    cudaMalloc(&d_diffCsrWeightsR_next, sizeof(int) * (diffRevE));
    init_kernel<int><<<((diffE) + numThreads) / numThreads, numThreads>>>(d_diffCsrWeights_next, INT_MAX, diffE);
    init_kernel<int><<<((diffRevE) + numThreads) / numThreads, numThreads>>>(d_diffCsrWeightsR_next, INT_MAX, diffRevE);

    // copy edges from previous diffCSR to new diffCSr
    copy_edges_diffcsr<<<numBlocksV, numThreads>>>(V, d_diff_indexofNodes, d_diff_edgeList, d_diff_edgeLen,
                                                   d_diff_rev_indexofNodes, d_diff_rev_edgeList, d_diff_rev_edgeLen,
                                                   d_diffCsrOffsets_next, d_diffCsrCords_next, d_diffCsrWeights_next,
                                                   d_diffCsrOffsetsR_next, d_diffCsrCordsR_next, d_diffCsrWeightsR_next);

    cudaFree(d_diff_indexofNodes);
    d_diff_indexofNodes = d_diffCsrOffsets_next;
    cudaFree(d_diff_edgeList);
    d_diff_edgeList = d_diffCsrCords_next;
    cudaFree(d_diff_edgeLen);
    d_diff_edgeLen = d_diffCsrWeights_next;
    cudaFree(d_diff_rev_indexofNodes);
    d_diff_rev_indexofNodes = d_diffCsrOffsetsR_next;
    cudaFree(d_diff_rev_edgeList);
    d_diff_rev_edgeList = d_diffCsrCordsR_next;
    cudaFree(d_diff_rev_edgeLen);
    d_diff_rev_edgeLen = d_diffCsrWeightsR_next;

    // add remaining edges to new diffCSR
    add_edges_diffcsr<<<numBlocksB, numThreads>>>(size, d_batch, d_diff_indexofNodes,
                                                  d_diff_edgeList, d_diff_edgeLen,
                                                  d_diff_rev_indexofNodes, d_diff_rev_edgeList,
                                                  d_diff_rev_edgeLen);

    // copy to host
    cudaMemcpy(indexofNodes, d_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyDeviceToHost);
    cudaMemcpy(edgeList, d_edgeList, sizeof(int32_t) * (E), cudaMemcpyDeviceToHost);
    cudaMemcpy(edgeLen, d_edgeLen, sizeof(int32_t) * (E), cudaMemcpyDeviceToHost);
    cudaMemcpy(rev_indexofNodes, d_rev_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyDeviceToHost);
    cudaMemcpy(srcList, d_srcList, sizeof(int32_t) * (E), cudaMemcpyDeviceToHost);
    cudaMemcpy(rev_edgeLen, d_rev_edgeLen, sizeof(int32_t) * (E), cudaMemcpyDeviceToHost);
    cudaMemcpy(diff_indexofNodes, d_diff_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyDeviceToHost);
    diff_edgeList = (int32_t*)realloc(diff_edgeList, sizeof(int32_t)*(diffE));
    diff_edgeLen = (int32_t*)realloc(diff_edgeLen, sizeof(int32_t)*(diffE));
    cudaMemcpy(diff_edgeList, d_diff_edgeList, sizeof(int32_t) * (diffE), cudaMemcpyDeviceToHost);
    cudaMemcpy(diff_edgeLen, d_diff_edgeLen, sizeof(int32_t) * (diffE), cudaMemcpyDeviceToHost);
    cudaMemcpy(diff_rev_indexofNodes, d_diff_rev_indexofNodes, sizeof(int32_t) * (V + 1), cudaMemcpyDeviceToHost);
    diff_rev_edgeList = (int32_t*)realloc(diff_rev_edgeList, sizeof(int32_t)*(diffRevE));
    diff_rev_edgeLen = (int32_t*)realloc(diff_rev_edgeLen, sizeof(int32_t)*(diffRevE));
    cudaMemcpy(diff_rev_edgeList, d_diff_rev_edgeList, sizeof(int32_t) * (diffRevE), cudaMemcpyDeviceToHost);
    cudaMemcpy(diff_rev_edgeLen, d_diff_rev_edgeLen, sizeof(int32_t) * (diffRevE), cudaMemcpyDeviceToHost);

    // free up the memory
    cudaFree(d_batch);
    cudaFree(d_indexofNodes);
    cudaFree(d_edgeList);
    cudaFree(d_edgeLen);
    cudaFree(d_rev_indexofNodes);
    cudaFree(d_srcList);
    cudaFree(d_rev_edgeLen);
    cudaFree(d_diff_indexofNodes);
    cudaFree(d_diff_edgeList);
    cudaFree(d_diff_edgeLen);
    cudaFree(d_diff_rev_indexofNodes);
    cudaFree(d_diff_rev_edgeList);
    cudaFree(d_diff_rev_edgeLen);
  }
};