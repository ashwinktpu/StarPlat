#include "deviceVarsAnalyser.h"
#include <string.h>
#include <unordered_map>

usedVariables deviceVarsAnalyser::getVarsWhile(whileStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getBody()));

  return currVars;
}
usedVariables deviceVarsAnalyser::getVarsDoWhile(dowhileStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getBody()));

  return currVars;
}
usedVariables deviceVarsAnalyser::getVarsAssignment(assignment *stmt)
{
  usedVariables currVars;
  if (stmt->lhs_isProp())
  {
    PropAccess *propId = stmt->getPropId();
    currVars.addPropAccess(propId, WRITE);
  }
  else if (stmt->lhs_isIdentifier())
    currVars.addVariable(stmt->getId(), WRITE);

  currVars.merge(getVarsExpr(stmt->getExpr()));
  return currVars;
}
usedVariables deviceVarsAnalyser::getVarsIf(ifStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(getVarsStatement(stmt->getElseBody()));

  return currVars;
}
usedVariables deviceVarsAnalyser::getVarsFixedPoint(fixedPointStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getDependentProp());
  currVars.addVariable(stmt->getFixedPointId());
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}
// TODO : Handle this atlast
usedVariables deviceVarsAnalyser::getVarsReduction(reductionCallStmt *stmt)
{
  usedVariables currVars;
  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsUnary(unary_stmt *stmt)
{
  Expression *unaryExpr = stmt->getUnaryExpr();
  Expression *varExpr = unaryExpr->getUnaryExpr();

  usedVariables currUsed;
  if (varExpr->isIdentifierExpr())
    currUsed.addVariable(varExpr->getId(), READ_WRITE);
  else if (varExpr->isPropIdExpr())
    currUsed.addPropAccess(varExpr->getPropId(), READ_WRITE);

  return currUsed;
}

usedVariables deviceVarsAnalyser::getVarsBFS(iterateBFS *stmt)
{
  usedVariables currVars = getVarsStatement(stmt->getBody());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(RBFSstmt->getBFSFilter());
    currVars.merge(RBFSstmt->getBody());
  }

  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsFor(forallStmt *stmt)
{
  usedVariables currVars;
  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsBlock(blockStatement *blockStmt)
{
  list<statement *> stmtList = blockStmt->returnStatements();
  usedVariables currVars;
  list<Identifier *> declVars;

  for (statement *stmt : stmtList)
  {
    if (stmt->getTypeofNode() == NODE_DECL)
    {
      declaration *decl = (declaration *)stmt;
      declVars.push_back(decl->getdeclId());
    }
    else
    {
      usedVariables vars = getVarsStatement(stmt);
      vars.removeVariable(declVars);
      currVars.merge(vars);
    }
  }

  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsStatement(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    return getVarsBlock(stmt);

  case NODE_WHILESTMT:
    return getVarsWhile((whileStmt *)stmt);

  case NODE_DOWHILESTMT:
    return getVarsDoWhile((dowhileStmt *)stmt);

  case NODE_FIXEDPTSTMT:
    return getVarsFixedPoint((fixedPointStmt *)stmt);

  case NODE_IFSTMT:
    return getVarsIf((ifStmt *)stmt);

  case NODE_ITRBFS:
    return getVarsBFS((iterateBFS *)stmt);

  case NODE_REDUCTIONCALLSTMT:
    return getVarsReduction((reductionCallStmt *)stmt);
  }

  return usedVariables();
}

void deviceVarsAnalyser::analyseForAll(forallStmt *stmt)
{
  Identifier *itr = stmt->getIterator();
  Identifier *graph = stmt->getSourceGraph();

  usedVariables vars = getVarsStatment(stmt->getBody());
}

void deviceVarsAnalyser::analyseStatement(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    for (statement *stmtB : ((blockStatement *)stmt)->returnStatements())
      analyseStatement(stmtB);
    break;

  case NODE_WHILESTMT:
    analyseStatement(((whileStmt *)stmt)->getBody());
    break;

  case NODE_DOWHILESTMT:
    analyseStatement(((dowhileStmt *)stmt)->getBody());
    break;

  case NODE_FIXEDPTSTMT:
    analyseStatement(((fixedPointStmt *)stmt)->getBody());
    break;

  case NODE_IFSTMT:
    statement *ifBody = ((ifStmt *)stmt)->getIfBody();
    statement *elseBody = ((ifStmt *)stmt)->getElseBody();
    if (ifBody != NULL)
      analyseStatement(ifBody);
    if (elseBody != NULL)
      analyseStatement(elseBody);
    break;

  case NODE_ITRBFS:
    analyseStatement(((iterateBFS *)stmt)->getBody());
    iterateReverseBFS *revBFS = ((iterateBFS *)stmt)->getRBFS();
    if (revBFS != NULL)
      analyseStatement(revBFS->getBody());
    break;

  case NODE_FORALLSTMT:
    analyseForAll((forallStmt *)stmt);
    break;
  }
}

lattice deviceVarsAnalyser::analyseDeclaration(declaration *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseAssignment(assignment *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseWhile(whileStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseDoWhile(dowhileStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseFixedPoint(fixedPointStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseIfElse(ifStmt *stmt, lattice &inMap)
{
  Expression* cond = stmt->getCondition();
  
}

lattice deviceVarsAnalyser::analyseItrBFS(iterateBFS *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseFor(forallStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseForAll(forallStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseReduction(reductionCallStmt *stmt, lattice &inMap)
{

}

lattice deviceVarsAnalyser::analyseUnary(unary_stmt *stmt, lattice &inMap)
{
  Expression* unaryVar = stmt->getUnaryExpr()->getUnaryExpr();

  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  lattice outMap = inMap;
  if(unaryVar->isIdentifierExpr()){
    outMap.meet(unaryVar->getId(), lattice::CPU_ONLY);
  }
  else if(unaryVar->isPropIdExpr())
  {
    PropAccess* propId = unaryVar->getPropId();
    outMap.meet(propId->getIdentifier1(), lattice::CPU_GPU_SHARED);
    outMap.meet(propId->getIdentifier2(), lattice::CPU_ONLY);
  }
  wrapNode->outMap = outMap;

  return outMap;
}

lattice deviceVarsAnalyser::analyseStatement(statement *stmt, lattice &inMap)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
    return analyseDeclaration((declaration *)stmt, inMap);
  case NODE_ASSIGN:
    return analyseAssignment((assignment *)stmt, inMap);
  case NODE_FIXEDPTSTMT:
    return analyseFixedPoint((fixedPointStmt *)stmt, inMap);
  case NODE_FORALLSTMT:
    {
      forallStmt* forStmt = (forallStmt*) stmt;

      if(forStmt->isForall())
        return analyseForAll((forallStmt*)stmt, inMap);
      else
        return analyseFor((forallStmt*)stmt, inMap);
    }
  case NODE_BLOCKSTMT:
    return analyseBlock((blockStatement *)stmt, inMap);
  case NODE_WHILESTMT:
    return analyseWhile((whileStmt *)stmt, inMap);
  case NODE_IFSTMT:
    return analyseIfElse((ifStmt *)stmt, inMap);
  case NODE_PROCCALLSTMT:
    // TODO : Add proc call statment
    return inMap;
  case NODE_REDUCTIONCALLSTMT:
    return analyseReduction((reductionCallStmt *)stmt, inMap);
  case NODE_ITRBFS:
    return analyseItrBFS((iterateBFS *)stmt, inMap);
  case NODE_DOWHILESTMT:
    return analyseDoWhile((dowhileStmt *)stmt, inMap);
  case NODE_UNARYSTMT:
    return analyseUnary((unary_stmt *)stmt, inMap);
  }

  return inMap;
}

lattice deviceVarsAnalyser::analyseBlock(blockStatement *stmt, lattice &inMap)
{
  list<statement *> currStmts = stmt->returnStatements();

  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  lattice lastOut = wrapNode->inMap;
  list<Identifier *> declaredVars;

  for (statement *bstmt : currStmts)
  {
    lastOut = analyseStatement(bstmt, lastOut);
    if (bstmt->getTypeofNode() == NODE_DECL)
      declaredVars.push_back(((declaration *)bstmt)->getdeclId());
  }
  wrapNode->outMap = lastOut;

  for (Identifier *iden : declaredVars)
    lastOut.removeVariable(iden);

  return lastOut;
}

void deviceVarsAnalyser::analyseFunc(ASTNode *proc)
{
  Function *func = (Function *)proc;
  analyseStatement(func->getBlockStatement());
  return;
}

void deviceVarsAnalyser::analyse()
{
  list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
    analyseFunc(func);
}