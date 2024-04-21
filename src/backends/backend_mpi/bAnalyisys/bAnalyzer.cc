#include "bAnalyzer.h"

void bAnalyzer::TraverseAST (statement * stmt) {
  

  if (stmt->getTypeofNode() == NODE_BLOCKSTMT) {
    list<statement*> stmtList = (blockStatement*)blockStmt->returnStatements();
    for (auto &stmt:stmtList) {
      TraverseAST (stmtList) ;
    }
  }
  if (stmt->getTypeofNode() == NODE_DECL) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_ASSIGN)  {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_WHILESTMT) {
    TraverseAST ((whileStmt*) stmt->getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_IFSTMT)  {
    TraverseAST ((ifStmt*) stmt->getIfBody()) ;
    TraverseAST ((ifStmt*) stmt->getElseBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_DOWHILESTMT) {
    TraverseAST ((dowhileStmt*) stmt->getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
    TraverseAST ((forallStmt*) stmt-> getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) {
    TraverseAST ((fixedPointStmt*) stmt->getBody ()) ;
  } 
  if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
    TraverseAST ((reductionCallStmt*) stmt->getBody ()) ;
  }
  if (stmt->getTypeofNode() == NODE_ITRBFS) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_PROCCALLSTMT)  {
    TraverseAST ((proc_callStmt*) stmt->getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
    TraverseAST ((unary_stmt*) stmt->getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_RETURN)  {
    return ;
  }
}

void bAnalyzer::analyzeForAllStmt (forallStmt * forAll) {
  
  list<statement*> stmtList = (blockStatement*)(forAll->getBody());

  for (auto &stmt:stmtList) {
    if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
      analyzeForAllStmt (forallStmt *forAll) 
    }
  }
}

void bAnalyzer::analuzeStatement (statement * ) {

}
