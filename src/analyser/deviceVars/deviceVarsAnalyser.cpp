#include "deviceVarsAnalyser.h"
#include <string.h>
#include <list>

// Statement Analyser

// Current assuming filter expression gets evaluated in GPU
// Required to handle case if we iterate through a set

//extern FrontEndContext frontEndContext;

lattice deviceVarsAnalyser::analyseForAll(forallStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  wrapNode->outMap = wrapNode->inMap;
  for (Identifier *iden : wrapNode->usedVars.getVariables(READ))
  {
    wrapNode->outMap.meet(iden, lattice::GPU_READ);
  }
  for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE))
  {
    wrapNode->outMap.meet(iden, lattice::GPU_WRITE);
  }

  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseDeclaration(declaration *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  wrapNode->outMap = inMap;
  if(stmt->isInitialized())
  {
    for (Identifier *iden : wrapNode->usedVars.getVariables(READ)){
      wrapNode->outMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE)){
      wrapNode->outMap.meet(iden, lattice::CPU_WRITE);
    }
  }

  wrapNode->outMap.setVarType(stmt->getdeclId(), lattice::CPU_ONLY);
  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseAssignment(assignment *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);

  wrapNode->inMap = inMap;
  wrapNode->outMap = inMap;

  for (Identifier *iden : wrapNode->usedVars.getVariables(READ))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_WRITE);
  }
 // cout<<"hello"<<endl;
  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseIfElse(ifStmt *stmt, lattice &inMap)
{

  Expression *cond = stmt->getCondition();
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  ASTNodeWrap *condNode = getWrapNode(cond);

  condNode->inMap = inMap;
  condNode->outMap = condNode->inMap;

  for (Identifier *iden : condNode->usedVars.getVariables(READ))
  {
    condNode->outMap.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : condNode->usedVars.getVariables(WRITE))
  {
    condNode->outMap.meet(iden, lattice::CPU_WRITE);
  }

  lattice ifOut = analyseStatement(stmt->getIfBody(), condNode->outMap);
  if (stmt->getElseBody() != nullptr)
  {
    lattice elseOut = analyseStatement(stmt->getElseBody(), condNode->outMap);
    wrapNode->inMap = ifOut ^ elseOut;
  }
  else
    wrapNode->inMap = ifOut ^ condNode->outMap;

  wrapNode->outMap = wrapNode->inMap;
  return wrapNode->outMap;
}


lattice deviceVarsAnalyser::analyseWhile(whileStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  Expression *cond = stmt->getCondition();
  ASTNodeWrap *condNode = getWrapNode(cond);
  
  bool hasChanged;
  do
  {
    condNode->inMap = (condNode->hasForAll) ? (wrapNode->outMap & wrapNode->inMap) : (wrapNode->outMap ^ wrapNode->inMap);
    condNode->outMap = condNode->inMap;

    for (Identifier *iden : condNode->usedVars.getVariables(READ))
    {
      condNode->outMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : condNode->usedVars.getVariables(WRITE))
    {
      condNode->outMap.meet(iden, lattice::CPU_WRITE);
    }
    
    lattice bodyOut = analyseStatement(stmt->getBody(), condNode->outMap);
    if(wrapNode->outMap != bodyOut)
    {
      hasChanged = true;
      wrapNode->outMap = bodyOut;
    }
    else 
      hasChanged = false;

  } while (hasChanged);

  return condNode->outMap;
}

lattice deviceVarsAnalyser::analyseDoWhile(dowhileStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  Expression *cond = stmt->getCondition();
  ASTNodeWrap *condNode = getWrapNode(cond);
  
  bool hasChanged;
  do
  {
    wrapNode->outMap = (wrapNode->hasForAll) ? (condNode->outMap & wrapNode->inMap) : (condNode->outMap ^ wrapNode->inMap);
    lattice bodyOut = analyseStatement(stmt->getBody(), wrapNode->outMap);
    
    condNode->inMap = bodyOut;

    lattice prevOut = condNode->outMap;
    condNode->outMap = condNode->inMap;
    for (Identifier *iden : condNode->usedVars.getVariables(READ))
    {
      condNode->outMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : condNode->usedVars.getVariables(WRITE))
    {
      condNode->outMap.meet(iden, lattice::CPU_WRITE);
    }
    hasChanged = (condNode->outMap != prevOut);
    
  } while (hasChanged);

  return condNode->outMap;
}

lattice deviceVarsAnalyser::analyseFixedPoint(fixedPointStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  Identifier *iden = stmt->getFixedPointId();
  ASTNodeWrap *condNode = getWrapNode(iden);
  
  bool hasChanged;
  do
  {
    condNode->inMap = (wrapNode->hasForAll) ? (wrapNode->outMap & wrapNode->inMap) : (wrapNode->outMap ^ wrapNode->inMap);
    condNode->outMap = condNode->inMap;

    for (Identifier *iden : condNode->usedVars.getVariables(READ))
    {
      condNode->outMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : condNode->usedVars.getVariables(WRITE))
    {
      condNode->outMap.meet(iden, lattice::CPU_WRITE);
    }
    
    lattice bodyOut = analyseStatement(stmt->getBody(), condNode->outMap);
    if(wrapNode->outMap != bodyOut)
    {
      hasChanged = true;
      wrapNode->outMap = bodyOut;
    }
    else 
      hasChanged = false;

  } while (hasChanged);

  return condNode->outMap;
}

lattice deviceVarsAnalyser::analyseProcCall(proc_callStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);

  wrapNode->inMap = inMap;
  wrapNode->outMap = inMap;

  for (Identifier *iden : wrapNode->usedVars.getVariables(READ))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_WRITE);
  }
                          /* stmt_expr */
  for (argument *arg : stmt->getProcCallExpr()->getArgList())
  {
      if (arg->isAssignExpr())
      {
          assignment *asgn = arg->getAssignExpr();
          wrapNode->outMap.meet(asgn->getId(), lattice::GPU_WRITE);
      }
  }
  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseFor(forallStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  lattice outTemp = wrapNode->inMap;

  if(stmt->isSourceProcCall())
  {
    // Nothing to Handle
  }
  else if(stmt->isSourceField())
  {
    PropAccess* sourceProp = stmt->getPropSource();
    outTemp.meet(sourceProp->getIdentifier1(), lattice::CPU_READ);
  }
  else
  {
    Identifier* srcId = stmt->getSource();
    outTemp.meet(srcId, lattice::CPU_READ);
  }

  if(stmt->hasFilterExpr()){
    for (Identifier *iden : wrapNode->usedVars.getVariables(READ))
    {
      outTemp.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE))
    {
      outTemp.meet(iden, lattice::CPU_WRITE);
    }
  }

  wrapNode->outMap = outTemp;

  bool hasChanged;
  do
  {
    
    lattice bodyIn = wrapNode->outMap;
    bodyIn.addVariable(stmt->getIterator(), lattice::CPU_ONLY);

    lattice bodyOut = analyseStatement(stmt->getBody(), bodyIn);
    bodyOut.removeVariable(stmt->getIterator());

    lattice newOutMap = (wrapNode->hasForAll) ? (bodyOut & outTemp) : (bodyOut ^ outTemp);

    if(wrapNode->outMap != newOutMap){
      wrapNode->outMap = newOutMap;
      hasChanged = true;
    }
    else
      hasChanged = false;

  }while(hasChanged);

  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseItrBFS(iterateBFS* stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  wrapNode->outMap = wrapNode->inMap;
  for (Identifier *iden : wrapNode->usedVars.getVariables(READ)){
    wrapNode->outMap.meet(iden, lattice::GPU_READ);
  }
  for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE)){
    wrapNode->outMap.meet(iden, lattice::GPU_WRITE);
  }

  if(stmt->getRBFS() != nullptr)
  {
    ASTNodeWrap *revNode = getWrapNode(stmt->getRBFS());

    for (Identifier *iden : revNode->usedVars.getVariables(READ)){
      wrapNode->outMap.meet(iden, lattice::GPU_READ);
    }
    for (Identifier *iden : revNode->usedVars.getVariables(WRITE)){
      wrapNode->outMap.meet(iden, lattice::GPU_WRITE);
    }
  }

  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseUnary(unary_stmt *stmt, lattice &inMap)
{
  Expression *unaryVar = stmt->getUnaryExpr()->getUnaryExpr();

  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  wrapNode->outMap = inMap;
  if (unaryVar->isIdentifierExpr())
  {
    wrapNode->outMap.meet(unaryVar->getId(), lattice::CPU_WRITE);
  }
  else if (unaryVar->isPropIdExpr())
  {
    PropAccess *propId = unaryVar->getPropId();
    wrapNode->outMap.meet(propId->getIdentifier1(), lattice::CPU_READ);
    wrapNode->outMap.meet(propId->getIdentifier2(), lattice::CPU_WRITE);
  }
  return wrapNode->outMap;
}
lattice deviceVarsAnalyser::analyseReduction(reductionCallStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);

  wrapNode->inMap = inMap;
  wrapNode->outMap = inMap;

  for (Identifier *iden : wrapNode->usedVars.getVariables(READ))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : wrapNode->usedVars.getVariables(WRITE))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_WRITE);
  }
  return wrapNode->outMap;
}
lattice deviceVarsAnalyser::analyseStatement(statement *stmt, lattice &inMap)
{
  printf("Analysing %p %d\n", stmt, stmt->getTypeofNode());
  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
    return analyseDeclaration((declaration *)stmt, inMap);
  case NODE_ASSIGN:
    return analyseAssignment((assignment *)stmt, inMap);
  case NODE_FORALLSTMT:
  {
    forallStmt *forStmt = (forallStmt *)stmt;

    if (forStmt->isForall())
      return analyseForAll((forallStmt *)stmt, inMap);
    else
      return analyseFor((forallStmt*)stmt, inMap);
  }
  case NODE_BLOCKSTMT:
    return analyseBlock((blockStatement *)stmt, inMap);
  case NODE_UNARYSTMT:
    return analyseUnary((unary_stmt *)stmt, inMap);
  case NODE_IFSTMT:
    return analyseIfElse((ifStmt *)stmt, inMap);
  case NODE_WHILESTMT:
    return analyseWhile((whileStmt *)stmt, inMap);
  case NODE_DOWHILESTMT:
    return analyseDoWhile((dowhileStmt *)stmt, inMap);
  case NODE_FIXEDPTSTMT:
    return analyseFixedPoint((fixedPointStmt *)stmt, inMap);
  case NODE_PROCCALLSTMT:
    return analyseProcCall((proc_callStmt*)stmt, inMap);
  case NODE_REDUCTIONCALLSTMT:
    return analyseReduction((reductionCallStmt *)stmt, inMap);
  case NODE_ITRBFS:
      return analyseItrBFS((iterateBFS *)stmt, inMap);
    /*
    case NODE_REDUCTIONCALLSTMT:
      return analyseReduction((reductionCallStmt *)stmt, inMap);
    */
  }

  return inMap;
}

lattice deviceVarsAnalyser::analyseBlock(blockStatement *stmt, lattice &inMap)
{
  list<statement *> currStmts = stmt->returnStatements();
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap = inMap;

  lattice lastOut = wrapNode->inMap;
  list<Identifier *> declaredVars;

  for (statement *bstmt : currStmts)
  {
    //printf("%p %d\n", bstmt, bstmt->getTypeofNode());
    lastOut = analyseStatement(bstmt, lastOut);
    if (bstmt->getTypeofNode() == NODE_DECL)
      declaredVars.push_back(((declaration *)bstmt)->getdeclId());
  }
  for (Identifier *iden : declaredVars)
    lastOut.removeVariable(iden);

  wrapNode->outMap = lastOut;
  return lastOut;
}

void deviceVarsAnalyser::analyseFunc(ASTNode *proc)
{
  Function *func = (Function *)proc;

  list<Identifier*> paramVars;
  for(formalParam* fParam: func->getParamList()){
    paramVars.push_back(fParam->getIdentifier());
  }
  initStatement(func->getBlockStatement(), paramVars);
  for (Identifier* id: gpuUsedVars.getVariables()) {
    id->getSymbolInfo()->setInParallelSection(true);
  }

  lattice inpLattice;
  for(Identifier* iden: paramVars){
    inpLattice.addVariable(iden, lattice::CPU_ONLY);
  }

  lattice outLattice = analyseStatement(func->getBlockStatement(), inpLattice);
  unordered_map<TableEntry*, lattice::PointType> outMap = outLattice.getLattice();
  //printStatement(func->getBlockStatement(), 0);
  blockStatement* newBody = (blockStatement*) transferVarsBlock(func->getBlockStatement(), nullptr);
  for(Identifier* iden: paramVars)
  {
    Type* type = iden->getSymbolInfo()->getType();
    if(type->isPropNodeType() && (outMap[iden->getSymbolInfo()] == lattice::GPU_ONLY))
    {
      varTransferStmt* transferStmt = new varTransferStmt(iden, 0);
      newBody->addStmtToBlock(transferStmt);
    }
  }
  func->setBlockStatement(newBody);

  printAST().printStatement(func->getBlockStatement());

  return;
}

void deviceVarsAnalyser::analyse(list<Function*> funcList)
{
  //list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
    analyseFunc(func);
}