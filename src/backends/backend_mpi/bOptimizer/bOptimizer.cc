#include "bOptimizer.h"

bOptimizer::bOptimizer (statement * stmt) {
  this->stmt = stmt ;
} 

statement * bOptimizer::improveEdgeForAll (forAllStmt * stmt, int analysisStatus) { 
  addvIdxDeclAbove (stmt) ;
  addvIdxInc (stmt) ;
  replaceDeclToSkipper (stmt) ;
  return stmt ;
}
