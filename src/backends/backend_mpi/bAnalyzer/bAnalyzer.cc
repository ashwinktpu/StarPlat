#include "bAnalyzer.h"

bAnalyzer::bAnalyzer () {}

void bAnalyzer::TraverseAST (statement * stmt) {
  
  // What all is to be done for the statment .

  if (stmt->getTypeofNode() == NODE_BLOCKSTMT) {
    list<statement*> stmtList = ((blockStatement*)stmt)->returnStatements();
    for (auto &stmt:stmtList) {
      TraverseAST (stmt) ;
    }
  }
  if (stmt->getTypeofNode() == NODE_DECL) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_ASSIGN)  {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_WHILESTMT) {
    TraverseAST (((whileStmt*) stmt)->getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_IFSTMT)  {
    TraverseAST (((ifStmt*) stmt)->getIfBody()) ;
    TraverseAST (((ifStmt*) stmt)->getElseBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_DOWHILESTMT) {
    TraverseAST (((dowhileStmt*) stmt)->getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
    TraverseAST (((forallStmt*) stmt)-> getBody()) ;
  }
  if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) {
    TraverseAST (((fixedPointStmt*) stmt)->getBody ()) ;
  } 
  if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_ITRBFS) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_PROCCALLSTMT)  {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_RETURN)  {
    return ;
  }
}

int bAnalyzer::analyzeForAllStmt (forallStmt * forAll) {
  
  list<statement*> stmtList = (((blockStatement*)(forAll)->getBody()))->returnStatements () ;
  if (forAll->isSourceProcCall()) {
    counter++ ;
    Identifier* vIdx = createIdNode (strcat ("_t", to_string (vIdx))) ;
    Identifier* sourceGraph = forAll->getSourceGraph();
    Identifier* iterator = forAll->getIterator();
    proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
    list<argument*> argList = extractElemFunc->getArgList();
    assert(argList.size() == 1);
    Identifier* iteratorMethodId = extractElemFunc->getMethodId();
    Identifier* nodeNbr = argList.front()->getExpr()->getId();
    if (strcmp (iteratorMethodId->getIdentifier (), "neighbors") == 0) {
      for (auto &stmt:stmtList) {
        Statement * currStmt = NULL ;
        if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
          analyzeForAllStmt ((forallStmt*) stmt)  ;
        }
        else if (stmt->getTypeofNode() == NODE_DECL) {
          int status = canImproveEdge ((declaration*)stmt, iterator->getIdentifier(), nodeNbr->getIdentifier())  
          if (status) {
            stmt = createNewEdgeStatement (stmt, status, counter) ;
          }
        }
        newStatement.addStmtToBlock (stmt) ;
      }
    }
    // Create a unary expression to add to the statement of the block.
    newStatement.addStmtToBlock (
  }
  return 0 ;
}



statement * createNewEdgeStatement (declaration * stmt, int status)  {

  assert (status == 1 || status == 2) ;

  assert (decl->isInitialized) ;
  Expression * expr = decl->getExpressionAssigned () ;

  assert (expr->isPropCallExpr()) ;
  proc_callExpr * proc = (proc_callExpr *) expr ;
  Identifier * newMethodId ;

  if (status == 1 ) {
    newMethodId = createIdNode("get_edge_r_i") ;
  } else if (status == 2) {
    newMethodId = createIdNode("get_edge_i") ;
  } else {
    assert (false) ;
  }
  propc_call * newExpression = nodeForProc_Call(proc->getId1, proc->getId2, newMethodId, proc->getArgList, proc->getExpr)
  stmt = assign_Declaration(stmt->getType(), stmt->getdeclId(), newExpression) 

  return stmt ;
}

int bAnalyzer::canImproveEdge (declaration* decl, char * u, char * v) {

  Type* type = decl->getType();

  if (type->isNodeEdgeType()) {

    if (decl->isInitialized()) {

      Expression * expr = decl->getExpressionAssigned();
      if (expr->isProcCallExpr()) {
      
        proc_callExpr* proc = (proc_callExpr*)expr;
        string methodId(proc->getMethodId()->getIdentifier());
        if (methodId == "get_edge") {
                
          list<argument*> argList = proc->getArgList();
          assert(argList.size() == 2);
      
          Identifier* srcId = argList.front()->getExpr()->getId();
          Identifier* destId = argList.back()->getExpr()->getId();
          Identifier* objectId = proc->getId1();
          if (!strcmp(srcId->getIdentifier(),u) and !strcmp(destId->getIdentifier (), v)) {
            return 1 ;
          }
          if (!strcmp(srcId->getIdentifier(),v) and !strcmp(destId->getIdentifier (), u)) {
            return 2 ; 
          }
        }
      }
    }
  }
  return 0 ;  
}

statement *getNewBody () {
  return newStatement ;  
}

int analysisStatus () {
  return analysisStatus ;
}
