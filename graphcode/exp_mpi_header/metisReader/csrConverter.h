#include <vector>
#include <unordered_map>
#include <stdint.h>

class csrConverter {
  private:
    std::vector<std::vector<std::vector<int> > > residualGraph ;
    std::unordered_map<int,int> localToGlobal;
    int localVertices ;
    int localEdges ;
    int pNo ;
    int32_t * indexNodes,* destList, *destProc, *srcProc, *revIndexNodes, *srcList, *revMap , *weights;
    
  public :
    csrConverter (std::vector<std::vector<std::vector<int> > > residualGraph, std::unordered_map<int,int> globalToLocal, int localVertices, int localEdges, int pNo) ;
    void convert () ;
    int32_t * getNodeOffsets () ;
    int32_t * getCSRArr () ;
    int32_t * getDestProc () ;
    int32_t * getEdgeWeights () ;
    int32_t * getRevIdxNodes () ;
    int32_t * getSrcList () ;
    int32_t * getSrcProc () ;
    int32_t * getRevMap () ;
} ;
