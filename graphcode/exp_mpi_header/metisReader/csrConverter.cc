
/* 

class csrConverter {
  private:
    std::vector<std::vector<std::vector<int> > > residualGraph ;
    int localVertices ;
    int localEdges ;
    int32_t * indexNodes, destList, destProc, revIndexNodes, srcList, revMap srcProc;
    
  public :
    csrConverter (std::vector<std::vector<std::vector<int> > > residualGraph, int localVertices, int localEdges ) ;
    void convert () ;
    int32_t * getNodeOffsets () ;
    int32_t * getCSRArr () ;
    int32_t * getDestProc () ;
    int32_t * getEdgeWeights () ;
    int32_t * getSrcProc () ;
    int32_t * getRevIdxNodes () ;
    int32_t * getSrcList () ;
    int32_t * getRevMap () ;
} ;
*/
#include "csrConverter.h"

int32_t * csrConverter::getNodeOffsets () {
  return indexNodes ;
}

int32_t * csrConverter::getCSRArr () {
  return destList ;
}

int32_t * csrConverter::getDestProc () {
  return destProc ;
}

int32_t * csrConverter::getEdgeWeights () {
  return weights ;
}

int32_t * csrConverter::getRevIdxNodes () {
  return revIndexNodes ;
}

int32_t * csrConverter::getSrcList () {
  return srcList ;
}

int32_t * csrConverter::getRevMap () {
  return revMap ;
}
int32_t * csrConverter::getSrcProc () {
  return srcProc;
}

csrConverter::csrConverter (std::vector<std::vector<std::vector<int> > > residualGraph, std::unordered_map<int,int> localToGlobal, int localVertices, int localEdgees, int pNo) {
  this->residualGraph = residualGraph ; 
  this->localVertices = localVertices ;
  this->localToGlobal = localToGlobal ;
  this->localEdges = localEdges ;
  this->indexNodes = new int32_t[this->localVertices] ;
  this->destList = new int32_t[this->localEdges] ;
  this->destProc = new int32_t[this->localEdges] ;
  this->srcProc = new int32_t[this->localVertices] ;
  this->revIndexNodes = new int32_t[this->localVertices] ; 
  this->srcList = new int32_t[this->localEdges] ;
  this->weights = new int32_t[this->localEdges] ;
  this->revMap = new int32_t[this->localEdges] ;
  this->pNo = pNo ;
}

void csrConverter::convert () {
  for (int u = 0 ; u < this->localVertices ; u++) {
    for (auto &edgeDetails:residualGraph[u]) {
      int v = edgeDetails[0] ; // vertex v.
      int c = edgeDetails[1] ; // capacity of edge between u and v.
      int i = edgeDetails[2] ; // reverse mapping of edge between u and v.
      int d = edgeDetails[3] ; // direction of edge from u to v.
      int r = edgeDetails[4] ; // process rank where v resides.
      if (d == 0) { // d == 0 stands for a FORWARD edge
        destList[indexNodes[u]] = localToGlobal[v] ; 
        revMap[indexNodes[u]] = i ; 
        weights[indexNodes[u]] = c ; 
        destProc[indexNodes[u]] = r ; 
        indexNodes[u+1] ++ ;
      }
    }
  }
  for (int v = 0 ; v < this->localVertices; v++) {
    for (auto &revEdgeDetails:residualGraph[v]) {
      int u = revEdgeDetails[0] ; // vertex v.
      int c = revEdgeDetails[1] ; // capacity of edge between u and v.
      int i = revEdgeDetails[2] ; // reverse mapping of edge between u and v.
      int d = revEdgeDetails[3] ; // direction of edge from u to v.
      int r = revEdgeDetails[4] ; // process rank where v resides.
      if (d == 1) { // d == 1 stands for a BACKWARD edge
        destList[indexNodes[v]] = localToGlobal[u] ; 
        revMap[indexNodes[v]] = i ; 
        weights[indexNodes[v]] = c ; 
        srcProc[indexNodes[v]] = this->pNo ; 
        indexNodes[v+1] ++ ;
      }
    }
  }
}
