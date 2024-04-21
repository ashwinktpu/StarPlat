#include "bAnalyzer.h"

bAnalyzer::bAnalyzer (statmemt * stmt) {
  
  TraverseAST (statement * stmt) ;
}

void bAnalyzer::TraverseAST (statement * stmt) {
  
  // What all is to be done for the statment .

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

int bAnalyzer::analyzeForAllStmt (forallStmt * forAll) {
  
  list<statement*> stmtList = (blockStatement*)(forAll->getBody());
  if (forAll->isSourceProcCall()) {
    Identifier* sourceGraph = forAll->getSourceGraph();
    proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
    Identifier* iteratorMethodId = extractElemFunc->getMethodId();
    if (strcmp (iteratorMethodId->getIdentifier (), "neighbors") == 0) {
      for (auto &stmt:stmtList) {
        if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
          analyzeForAllStmt ((forallStmt*) stmt) 
          continue ;
        }
        if (stmt->getTypeofNode() == NODE_ASSIGN) {
          return canImproveEdge (stmt) ;
        }
      }
    }
  }
}


int canImproveEdge (declaration* stmt) {
  Type* type = decl->getType();
  if (type->isPropType()) {
    if (decl->isInitialized()) {
      Expr * expr = decl->getExpressionAssigned();
      if (expr->isProcCallExpr()) {
        proc_callExpr* proc = (proc_callExpr*)expr;
        string methodId(proc->getMethodId()->getIdentifier());
        if (methodId == "get_edge") {
                
          list<argument*> argList = proc->getArgList();
          assert(argList.size() == 2);
      
          Identifier* srcId = argList.front()->getExpr()->getId();
          Identifier* destId = argList.back()->getExpr()->getId();
          Identifier* objectId = proc->getId1();
          if (srcId->getIdentifier() == u and destId->getIdentifier () == v) {
            return 1 ;
          }
          if (srcId->getIdentifier() == v or destId->getIdentifier () == u) {
            return 2 ; 
          }
        }
      }
    }
  }
  return 0 ;  
}
