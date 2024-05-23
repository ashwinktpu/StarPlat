#include "bAnalyzer.h"
#include <iostream>
#include <sstream>

#ifndef DEBUG_H
#define DEBUG_H

// Uncomment the line below in any file or define this in your build system to enable debug output
#define DEBUG

#ifdef DEBUG
    #define DEBUG_ENABLED 1
#else
    #define DEBUG_ENABLED 0
#endif

// Basic debug message logging
#define DEBUG_LOG(msg) \
    do { \
        if (DEBUG_ENABLED) { \
            std::ostringstream os_; \
            os_ << "DEBUG: " << __FILE__ << "(" << __LINE__ << ") " << msg << std::endl; \
            std::cerr << os_.str(); \
        } \
    } while (0)

// Debug logging with variable information
#define DEBUG_VAR(var) \
    do { \
        if (DEBUG_ENABLED) { \
            std::ostringstream os_; \
            os_ << "DEBUG: " << __FILE__ << "(" << __LINE__ << ") " #var " = " << (var) << std::endl; \
            std::cerr << os_.str(); \
        } \
    } while (0)

// Assertion with debug
#define DEBUG_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::ostringstream os_; \
            os_ << "ASSERTION FAILED: " << __FILE__ << "(" << __LINE__ << ") " \
                << msg << " [" #cond "]" << std::endl; \
            std::cerr << os_.str(); \
            std::abort(); \
        } \
    } while (0)

#endif // DEBUG_H



#define DEBUG

bAnalyzer::bAnalyzer () {
  newStatement = NULL ;
  analysisStatus = 0 ;
  counter = 0 ;
  this->newStatement = new blockStatement () ; 
  this->newStatement->setTypeofNode(NODE_BLOCKSTMT) ;
  this->statementForAll = new blockStatement () ;
  this->statementForAll->setTypeofNode(NODE_BLOCKSTMT) ;
  this->statementWhile = new blockStatement () ;
  this->statementWhile->setTypeofNode(NODE_BLOCKSTMT) ;
  this->lastIter = new char[10] ;
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
    return ; }
  if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
    return ;
  }
  if (stmt->getTypeofNode() == NODE_RETURN)  {
    return ;
  }
}

bool bAnalyzer::checkSignature (forallStmt *forAll) {
  
  if (forAll->isSourceProcCall()) {
    Identifier* sourceGraph = forAll->getSourceGraph();
    Identifier* iterator = forAll->getIterator();
    proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
    list<argument*> argList = extractElemFunc->getArgList();
    Identifier* iteratorMethodId = extractElemFunc->getMethodId();
    if (strcmp (iteratorMethodId->getIdentifier (), "nodes") == 0) return true ;
  }
  return false ;
}

int bAnalyzer::analyzeForAllStmt (forallStmt * forAll) {
  list<statement*> stmtList = (((blockStatement*)(forAll)->getBody()))->returnStatements () ;
  bool needsInc = false ;
  printf ("Trying counter %d with address %p\n", counter, (void*)forAll) ;
    for (auto &forAllEntry:forAllNesting) {
      int entryOne = forAllEntry.first ;
      auto entryTwo = forAllEntry.second ;
      if (entryTwo == forAll) {
        printf ("entry one = %d, entry two = %p\n", entryOne, (void*)entryTwo) ;
        printf ("done this already\n") ;
        return 0 ;
      }
    }
    forAllNesting[counter++] = forAll ;
    bool reArrangedV = false ;
  if (forAll->isSourceProcCall()) {
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
    Identifier* iteratorMethodId = extractElemFunc->getMethodId();
    printf ("This particular analysis is for forAll %s\n", iteratorMethodId->getIdentifier ()) ;
    if (strcmp (iteratorMethodId->getIdentifier (), "neighbors") == 0) {
      Identifier* nodeNbr = argList.front()->getExpr()->getId();
      assert(argList.size() == 1);
      for (auto &stmt:stmtList) {
        if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
          analyzeForAllStmt ((forallStmt*) stmt)  ;
        }
        else if (stmt->getTypeofNode() == NODE_DECL) {
          int status = canImproveEdge ((declaration*)stmt, iterator->getIdentifier(), nodeNbr->getIdentifier()) ;  
          if (status) {
            analysisStatus = 1 ;
            if (!reArrangedV) newStatement->addStmtToBlock (createNewV((declaration*)stmt, nodeNbr, iterator, itVar)) ;
            stmt = createNewEdgeStatement ((declaration*)stmt, status, itVar) ;
            needsInc = true ;
          }
        }
        newStatement->addStmtToBlock (stmt) ;
      }
    }
    // Create a unary expression to add to the statement of the block.
    if (needsInc){
      Identifier * idxVal = Identifier::createIdNode (itVar) ;
      Expression * idxExpr = Expression::nodeForIdentifier (idxVal) ;
      unary_stmt* incrementer = (unary_stmt*)Util::createNodeForUnaryStatements(Util::createNodeForUnaryExpr(idxExpr, OPERATOR_INC)); 
      incrementer->setTypeofNode (NODE_UNARYSTMT) ;
      newStatement->addStmtToBlock ((statement*)incrementer) ;
    } 
    if (!iterVar.empty())
    strcpy (this->lastIter, iterVar.top ()) ;
    this->lastIter[9]='\0' ; // To prevent stack smashing.
    this->iterVar.pop () ;
  }
  return 0 ;
}

statement * bAnalyzer::createNewV (declaration * stmt, Identifier * u,  Identifier * v, const char * _tx) {

  /*Switch this to a proper assignment statement with expression handling.*/

  Expression * expr = stmt->getExpressionAssigned () ;
  assert (expr->isProcCallExpr()) ;
  proc_callExpr * proc = (proc_callExpr *) expr ;
  list<argument *> argList = proc->getArgList () ;
  Identifier * newMethodId = Identifier::createIdNode ("get_other_vertex") ;
  argument * a1 = new argument () ;
  argument * a2 = new argument () ;
  Identifier * uVal = u ;
  Expression * uExpr = Expression::nodeForIdentifier (uVal) ;
  Identifier * idxVal = Identifier::createIdNode (_tx) ;
  Expression * idxExpr = Expression::nodeForIdentifier (idxVal) ;
  list<argument *> newArgList ;
  a1->setExpression (uExpr) ;
  a2->setExpression (idxExpr) ;
  newArgList.push_back (a1) ;  
  newArgList.push_back (a2) ;  
  proc_callExpr * newExpression = proc_callExpr::nodeForProc_Call(proc->getId1(), proc->getId2(),  newMethodId, newArgList, proc->getIndexExpr()) ;
  ASTNode * newStmt = Util::createAssignmentNode ((ASTNode*) v, (ASTNode*) newExpression) ;
  // Make symbol Table entry 
  Type *typeNode = Type::createForPrimitive (TYPE_INT, 1) ;
  TableEntry newEntry (v, typeNode) ;
  // declaration * newStmt = declaration::assign_Declaration((Type*)Util::createNodeEdgeTypeNode(TYPE_NODE), v, newExpression) ;
  return (statement*)newStmt ;
}

statement * bAnalyzer::createFrontierPopStatement () {
  // Barenya : Hard coding g. Remove later. 
  std::string graphName = "g" ;
  const char * graphNameChar = graphName.c_str () ;
  Identifier * graphNameId = (Identifier *) Util::createIdentifierNode (graphNameChar) ;

  std::string methodName = "frontier_pop" ;
  const char * methodNameChar = methodName.c_str () ;
  Identifier * methodNameId = (Identifier *) Util::createIdentifierNode (methodNameChar) ;

  list <argument *> noArgs;
  auto procId = Util::createPropIdNode (graphNameId, methodNameId) ;
  auto procNode = Util::createNodeForProcCall(procId, noArgs, NULL) ;

  statement * addIntoQueue = (statement *) Util::createNodeForProcCallStmt(procNode) ;

  return addIntoQueue ;
}

void bAnalyzer::generateNewRelationCheck (blockStatement * oldBlockChunk) {
  if (((statement *)oldBlockChunk)->getTypeofNode() != NODE_BLOCKSTMT) {
    blockStatement * newBlockChunk = new blockStatement () ;
    newBlockChunk->addStmtToBlock ((statement*)oldBlockChunk) ;
    oldBlockChunk = newBlockChunk ;
  }
  for (auto &stmt:oldBlockChunk->returnStatements()) {
    if (stmt->getTypeofNode() == NODE_ASSIGN) {
      // check if propId.  
      if (((assignment *)stmt)->lhs_isProp()) {
        PropAccess * id = ((assignment*)stmt)->getPropId() ;
        Identifier * concernedId = id->getIdentifier1 () ;
        Identifier * propertyId = id->getIdentifier2 () ;
        const char * varName = concernedId->getIdentifier () ;
        const char * methodName = propertyId->getIdentifier () ; 
        std::string tVar = varName ;
        std::string mVar = methodName ;
        if (std::find (updatedNodes.begin (), updatedNodes.end (), mVar) != updatedNodes.end () ) {
          if (std::find (trackNodes.begin (), trackNodes.end (), tVar) == trackNodes.end ()) {
            printf ("adding %s to trackNodes\n", tVar.c_str()) ;
            trackNodes.push_back (tVar) ;
          }
        }
      }
    }
    else if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
      if (((reductionCallStmt*)stmt)->is_reducCall()) {
        // Need to fail analysis if this happens. 
      } else if (((reductionCallStmt*)stmt)->isLeftIdentifier()){

      } else if (((reductionCallStmt*)stmt)->isContainerReduc()) {

      } else {
        PropAccess * id = ((reductionCallStmt*)stmt)->getPropAccess() ;
        Identifier * concernedId = id->getIdentifier1 () ;
        Identifier * propertyId = id->getIdentifier2 () ;
        const char * varName = concernedId->getIdentifier () ;
        const char * methodName = propertyId->getIdentifier () ; 
        std::string tVar = varName ;
        std::string mVar = methodName ;
        if (std::find (updatedNodes.begin (), updatedNodes.end (), mVar) != updatedNodes.end () ) {
          if (std::find (trackNodes.begin (), trackNodes.end (), tVar) == trackNodes.end ()) {
            printf ("adding %s to trackNodes\n", tVar.c_str()) ;
            trackNodes.push_back (tVar) ;
          }
        }
      }
    }
    else {
      if (stmt->getTypeofNode() == NODE_IFSTMT) {
        DEBUG_LOG ("FOUND IF STMT\n") ;
        statement * pusher = ((ifStmt*) stmt)->getIfBody() ;
        /*
        if (pusher->getTypeofNode() != NODE_BLOCKSTMT) {
          blockStatement * newPusher = new blockStatement () ;
          newPusher->addStmtToBlock (pusher) ;
          generateNewRelationCheck (newPusher);
        } else {*/
          generateNewRelationCheck ((blockStatement*)pusher) ;
        // }
        pusher = ((ifStmt*) stmt)->getElseBody() ;
        /*if (pusher->getTypeofNode() != NODE_BLOCKSTMT) {
          blockStatement * newPusher = new blockStatement () ;
          newPusher->addStmtToBlock (pusher) ;
          generateNewRelationCheck (newPusher);
        } else {*/
        if (pusher != NULL) 
          generateNewRelationCheck ((blockStatement*)pusher) ;
        // }
      }
      else if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
        // generateNewRelationCheck (((whileStmt*)stmt)->getBody()) ;
        DEBUG_LOG ("FOUND NESTED FORALL\n") ;
        statement * forAllBody = ((forallStmt *)stmt)->getBody () ;
        generateNewRelationCheck ((blockStatement *) forAllBody) ;
      } else {
        DEBUG_LOG ("Just skipped statement\n") ;
      }
    }
  }
}


Expression * bAnalyzer::filterExprDeepCopy (Expression * condition, const char * node) {
  // traverse the expression and be sure all rhs are constants.
  if (condition->isRelational ()) {
    Expression * leftExpr = condition->getLeft () ;
    Expression * rightExpr = condition->getRight () ;
    Identifier * newPropId = (Identifier *) Util::createIdentifierNode (node) ;
    Expression * newLeftExpr ;

    if (leftExpr->isPropIdExpr()) {
      PropAccess * propId = leftExpr->getPropId() ;
      Identifier * methodId = propId->getIdentifier2 () ;
      newLeftExpr = Expression::nodeForPropAccess(PropAccess::createPropAccessNode (newPropId, methodId)) ;
    } else {
      newLeftExpr = Expression::nodeForIdentifier (newPropId) ;  
    }
    return Expression::nodeForRelationalExpr (newLeftExpr, rightExpr, condition->getOperatorType ()) ;
  } else if (condition->isLogical()) {
    Expression * a1 = filterExprDeepCopy(condition->getLeft (), node) ;
    Expression * a2 = filterExprDeepCopy(condition->getRight (), node) ;
    return Expression::nodeForLogicalExpr (a1, a2, condition->getOperatorType ()) ;
  } else {
    assert (false) ;
  }
}


statement * createFrontierPushStatement (const char* vertexNameChar) {
  
  std::string graphName = "g" ;
  const char * graphNameChar = graphName.c_str () ;
  Identifier * graphNameId = (Identifier *) Util::createIdentifierNode (graphNameChar) ;

  std::string methodName = "frontier_push" ;
  const char * methodNameChar = methodName.c_str () ;
  Identifier * methodNameId = (Identifier *) Util::createIdentifierNode (methodNameChar) ;

  Identifier * vertexNameId = (Identifier *) Util::createIdentifierNode (vertexNameChar) ;

  argument * arg1 = new argument () ;
  arg1->setExpression (Expression::nodeForIdentifier (vertexNameId)) ;
  arg1->setExpressionFlag() ;
  argList * paramsForFrontierPush = Util::createAList (arg1) ;

  auto procId = Util::createPropIdNode (graphNameId, methodNameId) ;
  auto procNode = Util::createNodeForProcCall(procId, paramsForFrontierPush->AList, NULL) ;

  statement * addIntoQueue = (statement *) Util::createNodeForProcCallStmt(procNode) ;
  return addIntoQueue ;
}

void bAnalyzer::createNewFrontierStatement (Expression * filterExpr, blockStatement * oldBlockChunk) {
  // Traverse the entire block till for changes in all property methods encountered. 
  generateNewRelationCheck (oldBlockChunk) ;
  for (auto &node:trackNodes) {
    printf ("Generating expression copies for %s\n", node.c_str()) ;
    Expression * tExpr = filterExprDeepCopy (filterExpr, node.c_str()) ;
    ifStmt * tCond = ifStmt::create_ifStmt (tExpr, createFrontierPushStatement (node.c_str()), NULL) ;
    oldBlockChunk->addStmtToBlock ((statement *)tCond) ;
  }
  proc_callStmt * popStmt = (proc_callStmt*)createFrontierPopStatement () ; 
  proc_callExpr * procedure = popStmt->getProcCallExpr () ;

  whileStmt * newChunk = whileStmt::create_whileStmt ((Expression*)procedure, oldBlockChunk) ; 
  statementWhile = ((statement *)newChunk) ;
}


void bAnalyzer::createNewForAllStmtBlock () {
  // generate just g.frontier_push (all v values) 
  // Hardcoding name of the graph.
  std::string graphName = "g" ;
  const char * graphNameChar = graphName.c_str () ;
  Identifier * graphNameId = (Identifier *) Util::createIdentifierNode (graphNameChar) ;

  std::string methodName = "frontier_push" ;
  const char * methodNameChar = methodName.c_str () ;
  Identifier * methodNameId = (Identifier *) Util::createIdentifierNode (methodNameChar) ;


  std::string vertexName = "v" ;
  const char * vertexNameChar = vertexName.c_str () ;
  Identifier * vertexNameId = (Identifier *) Util::createIdentifierNode (vertexNameChar) ;

  argument * arg1 = new argument () ;
  arg1->setExpression (Expression::nodeForIdentifier (vertexNameId)) ;
  arg1->setExpressionFlag() ;
  paramsForFrontierPush = Util::createAList (arg1) ;

  auto procId = Util::createPropIdNode (graphNameId, methodNameId) ;
  auto procNode = Util::createNodeForProcCall(procId, paramsForFrontierPush->AList, NULL) ;

  statement * addIntoQueue = (statement *) Util::createNodeForProcCallStmt(procNode) ;

  statementForAll->addStmtToBlock ((statement*)addIntoQueue) ;
}

void bAnalyzer::setFilterAnalysisStatement1 (blockStatement * forAllLoopBlock) {
  // New class variable
  
  return ;
}
void bAnalyzer::setFilterAnalysisStatement2 (whileStmt * forAllLoopBlock) {
  // New class variable
  return ;
}

blockStatement * bAnalyzer::getStatementForLoop () {
  return this->statementForAll ;
}

statement * bAnalyzer::getStatementWithinWhileLoop () {
  return this->statementWhile ;
}


void bAnalyzer::evaluateFilter (forallStmt * oldStmt, blockStatement * oldBlockChunk, Expression * condition) {
  if (filterAnalysis (condition) && checkSignature (oldStmt)) { 
    /*DEBUG_LOG ("filter Analysis aproves further optimisation") ;
    this->filterAnalysisStatus = true ;
    createNewForAllStmtBlock () ;
    createNewFrontierStatement (condition, oldBlockChunk) ;*/
  } else {
    this->filterAnalysisStatus = false ;
    statementForAll = (blockStatement *) oldStmt->getBody() ;
  }
}
  
bool bAnalyzer::filterAnalysis (Expression * condition) {
  // traverse the expression and be sure all rhs are constants.
  if (condition->isRelational ()) {
    Expression * leftExpr = condition->getLeft () ;
    Expression * rightExpr = condition->getRight () ;

    if (leftExpr->isPropIdExpr()) {
      PropAccess * propId = leftExpr->getPropId() ;
      Identifier * methodId = propId->getIdentifier2 () ;
      const char * methodName = methodId->getIdentifier () ;
      std::string methodName1 = methodName; 
      if (std::find(updatedNodes.begin (), updatedNodes.end(), methodName1) == updatedNodes.end()) {
        updatedNodes.push_back (methodName1) ;
      }
    }
    if (!rightExpr->isLiteral ()) return false ;
    if (leftExpr->isIdentifierExpr() || leftExpr->isPropIdExpr () || leftExpr->isProcCallExpr ()) return true ;
  } else if (condition->isLogical()) {
    bool a1 = filterAnalysis (condition->getLeft ()) ;
    bool a2 = filterAnalysis (condition->getRight ()) ;
    return a1 || a2 ;
  } else {
    return false ;
  }
}

statement * bAnalyzer::createNewEdgeStatement (declaration * stmt, int status, const char * vIdx)  {
  
  DEBUG_LOG ("Call to createNewEdgeStatement atleast\n") ;
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
    DEBUG_LOG ("Making reverse edge idx\n") ;
    newMethodId = Identifier::createIdNode("get_edge_r_i") ;
    a1->setExpression (idxExpr) ;
    a2->setExpression (argList.back()->getExpr ()) ;
  } else if (status == 2) {
    DEBUG_LOG ("Making forward edge idx\n") ;
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

bool bAnalyzer::getFilterAnalysisStatus () {
  return this->filterAnalysisStatus ;
}

void bAnalyzer::clearAllAnalysis () {
  newStatement = NULL ;
  newStatement = new blockStatement () ; 
  newStatement->setTypeofNode(NODE_BLOCKSTMT) ;
  counter = 0 ;
  analysisStatus = 0 ;
  newArgList.clear () ;
  iterVar = stack<char*> ();
  lastIter[0] = '\0';
}
