#include "bAnalyzer.h"

bAnalyzer::bAnalyzer () {
  newStatement = NULL ;
  analysisStatus = 0 ;
  counter = 0 ;
  newStatement = new blockStatement () ; 
  newStatement->setTypeofNode(NODE_BLOCKSTMT) ;
}

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
  bool needsInc = false ;
  if (forAll->isSourceProcCall()) {
    counter++ ;
    forAllNesting[counter] = forAll ;
    char itVar [1024] ;
    char * prefix = "_t" ;
    sprintf (itVar, "%s", prefix) ;
    sprintf (itVar + strlen (itVar) , "%d", counter) ;
    this->iterVar.push (itVar) ;
    Identifier* vIdx = Identifier::createIdNode (itVar) ;
    Identifier* sourceGraph = forAll->getSourceGraph();
    Identifier* iterator = forAll->getIterator();
    proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
    list<argument*> argList = extractElemFunc->getArgList();
    assert(argList.size() == 1);
    Identifier* iteratorMethodId = extractElemFunc->getMethodId();
    Identifier* nodeNbr = argList.front()->getExpr()->getId();
    if (strcmp (iteratorMethodId->getIdentifier (), "neighbors") == 0) {
      for (auto &stmt:stmtList) {
        if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
          analyzeForAllStmt ((forallStmt*) stmt)  ;
        }
        else if (stmt->getTypeofNode() == NODE_DECL) {
          int status = canImproveEdge ((declaration*)stmt, iterator->getIdentifier(), nodeNbr->getIdentifier()) ;  
          if (status) {
            analysisStatus = 1 ;
            stmt = createNewEdgeStatement ((declaration*)stmt, status, itVar) ;
            needsInc = true ;
          }
        }
        newStatement->addStmtToBlock (stmt) ;
      }
    }
    // Create a unary expression to add to the statement of the block.
    /*if (needsInc){
      unary_stmt * incrementer = new unary_stmt () ; 
      newStatement.addStmtToBlock (
      */
    this->lastIter = iterVar.top () ;
    this->iterVar.pop () ;
  }
  return 0 ;
}



statement * bAnalyzer::createNewEdgeStatement (declaration * stmt, int status, const char * vIdx)  {

  assert (status == 1 || status == 2) ;

  // assert (stmt->isInitialized != 0) ;
  Expression * expr = stmt->getExpressionAssigned () ;

  assert (expr->isProcCallExpr()) ;
  proc_callExpr * proc = (proc_callExpr *) expr ;
  list<argument *> argList = proc->getArgList () ;

  Identifier * newMethodId ;
  Identifier * src = argList.front()->getExpr()->getId() ;
  Identifier * dest = argList.back()->getExpr()->getId() ;
  
  Identifier * newIdx = Identifier::createIdNode (vIdx) ;
  Expression * idxExpr = Expression::nodeForIdentifier (newIdx) ;
  list<argument *> newArgList ;

  argument * a1 = new argument () ;
  argument * a2 = new argument () ;

  if (status == 1 ) {
    newMethodId = Identifier::createIdNode("get_edge_r_i") ;
    a1->setExpression (idxExpr) ;
    a2->setExpression (argList.back()->getExpr ()) ;
  } else if (status == 2) {
    newMethodId = Identifier::createIdNode("get_edge_i") ;
    a1->setExpression (argList.front ()->getExpr ()) ;
    a2->setExpression (idxExpr) ;
  } else {
    assert (false) ;
  }
  newArgList.push_back (a1) ;  
  newArgList.push_back (a2) ;  
//   argListList.emplace_back(newArgList) ;
  proc_callExpr * newExpression = proc_callExpr::nodeForProc_Call(proc->getId1(), proc->getId2(),  newMethodId, newArgList, proc->getIndexExpr()) ;
  stmt = declaration::assign_Declaration(stmt->getType(), stmt->getdeclId(), newExpression) ;

  return (statement*) stmt ;
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

blockStatement * bAnalyzer::getNewBody () {
  return this->newStatement ;  
}

int bAnalyzer::getAnalysisStatus () {
  return this->analysisStatus ;
}

char * bAnalyzer::getIteratorVar () {
  return this->lastIter ;
}

void bAnalyzer::clearAllAnalysis () {
  newStatement = NULL ;
  newStatement = new blockStatement () ; 
  counter = 0 ;
  analysisStatus = 0 ;
  newArgList.clear () ;
  iterVar = stack<char*> ();
  lastIter = NULL ;
}
