#ifndef GENCPP_TRIANGLE_COUNT_DSL_H
#define GENCPP_TRIANGLE_COUNT_DSL_H
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdbool>
#include <fstream>
#include <sys/time.h>
#include <boost/mpi.hpp>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
#include "graph.hpp"

using namespace std;
namespace mpi = boost::mpi;

auto Compute_TC(graph g);
bool check_if_nbr(int s, int d, int* indexofNodes,int* edgeList, int startv)
{
    int startEdge=indexofNodes[s - startv];
    int endEdge=indexofNodes[s - startv + 1]-1;
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
bool check_if_nbr_new(int s, int d, vector <int>& s_List, vector <int>& d_List)
{
    int startEdge=0;
    int endEdge=s_List.size() - 1;
    if(s_List[startEdge]==d)
      return true;
    if(s_List[endEdge]==d)
      return true;
    int mid = (startEdge+endEdge)/2;
    while(startEdge<=endEdge)
    {
        if(s_List[mid]==d)
          return true;
        if(d<s_List[mid])
          endEdge=mid-1;
        else
          startEdge=mid+1;
        mid = (startEdge+endEdge)/2;
    }
    return false;
}

#endif
