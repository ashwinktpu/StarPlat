#include "deviceVarsAnalyser.h"

#include <string.h>
#include <unordered_map>
#include "../ast/ASTHelper.cpp"


// Statement Analyser
// Current assuming filter expression gets evaluated in GPU

lattice deviceVarsAnalyser::analyseForAll(forallStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  lattice outMap = inMap;

  usedVariables vars = getVarsForAll(stmt);
  for (Identifier *iden : vars.getVariables(READ))
  {
    outMap.meet(iden, lattice::GPU_READ);
  }
  for (Identifier *iden : vars.getVariables(WRITE))
  {
    outMap.meet(iden, lattice::GPU_WRITE);
  }

  wrapNode->outMap = outMap;
  return outMap;
}

lattice deviceVarsAnalyser::analyseDeclaration(declaration *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  wrapNode->inMap ^= inMap;

  wrapNode->outMap ^= inMap;
  if(stmt->isInitialized())
  {
    Expression* expr = stmt->getExpressionAssigned();
    usedVariables usedVars = getVarsExpr(expr);

    for (Identifier *iden : usedVars.getVariables(READ)){
      wrapNode->outMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : usedVars.getVariables(WRITE)){
      wrapNode->outMap.meet(iden, lattice::CPU_WRITE);
    }
  }

  wrapNode->outMap.setVarType(stmt->getdeclId(), lattice::CPU_ONLY);
  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseAssignment(assignment *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  usedVariables usedVars = getVarsAssignment(stmt);

  wrapNode->inMap ^= inMap;
  wrapNode->outMap ^= inMap;

  for (Identifier *iden : usedVars.getVariables(READ))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : usedVars.getVariables(WRITE))
  {
    wrapNode->outMap.meet(iden, lattice::CPU_WRITE);
  }

  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseWhile(whileStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap ^= inMap;

  Expression *cond = stmt->getCondition();
  usedVariables exprVars = seperatePropAccess(analyserUtils::getVarsExpr(cond));

  lattice condOut = inMap;
  for (Identifier *iden : exprVars.getVariables(READ))
  {
    condOut.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : exprVars.getVariables(WRITE))
  {
    condOut.meet(iden, lattice::CPU_WRITE);
  }

  do
  {
    wrapNode->outMap = condOut;
    lattice bodyOut = deviceVarsAnalyser::analyseStatement(stmt->getBody(), condOut);

    condOut = (wrapNode->inMap) ^ (bodyOut);
    for (Identifier *iden : exprVars.getVariables(READ))
    {
      condOut.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : exprVars.getVariables(WRITE))
    {
      condOut.meet(iden, lattice::CPU_WRITE);
    }

  } while ((wrapNode->outMap != condOut));

  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseDoWhile(dowhileStmt *stmt, lattice &inMap)
{
  ASTNodeList *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  Expression *cond = stmt->getCondition();
  usedVariables exprVars = seperatePropAccess(analyserUtils::getVarsExpr(cond));
  lattice currMap = wrapNode->outMap;

  do
  {
    wrapNode->outMap = currMap;

    currMap = wrapNode->outMap ^ wrapNode->inMap;
    currMap = deviceVarsAnalyser::analyseStatement(stmt->getBody(), currMap);

    ASTNodeWrap *condWrap = getWrapNode(stmt->getCondition(), currMap);
    condWrap->inMap = currMap;

    for (Identifier *iden : exprVars.getVariables(READ))
    {
      currMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : exprVars.getVariables(WRITE))
    {
      currMap.meet(iden, lattice::CPU_WRITE);
    }
    condWrap->outMap = currMap;

  } while (wrapNode->outMap != currMap);
}

// TODO: Handle source field and source procedure call
lattice deviceVarsAnalyser::analyseFor(forallStmt *stmt, lattice &inMap)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  lattice exprMap = inMap;
  Expression *filterExpr = stmt->getfilterExpr();

  if (filterExpr != nullptr)
  {
    ASTNodeWrap *exprNode = getWrapNode(filterExpr, inMap);
    exprNode->inMap = exprMap;

    usedVariables exprVars = seperatePropAccess(analyserUtils::getVarsExpr(filterExpr));
    for (Identifier *iden : exprVars.getVariables(READ))
    {
      exprMap.meet(iden, lattice::CPU_READ);
    }
    for (Identifier *iden : exprVars.getVariables(WRITE))
    {
      exprMap.meet(iden, lattice::CPU_WRITE);
    }
    exprNode->outMap = exprMap;
  }
  lattice currMap = wrapNode->outMap;

  do
  {
    wrapNode->outMap = currMap;

    currMap = exprMap ^ wrapNode->outMap;
    currMap = deviceVarsAnalyser::analyseStatement(stmt->getBody(), currMap);

  } while (wrapNode->outMap != currMap);

  return wrapNode->outMap;
}

lattice deviceVarsAnalyser::analyseIfElse(ifStmt *stmt, lattice &inMap)
{
  // cout<<"In if else"<<endl;

  Expression *cond = stmt->getCondition();
  ASTNodeWrap *wrapNode = getWrapNode(cond, inMap);
  wrapNode->inMap = inMap;

  lattice condOut = inMap;
  usedVariables exprVars = seperatePropAccess(analyserUtils::getVarsExpr(cond));

  for (Identifier *iden : exprVars.getVariables(READ))
  {
    condOut.meet(iden, lattice::CPU_READ);
  }
  for (Identifier *iden : exprVars.getVariables(WRITE))
  {
    condOut.meet(iden, lattice::CPU_WRITE);
  }

  wrapNode->outMap = condOut;

  lattice ifOut = analyseStatement(stmt->getIfBody(), condOut);
  ASTNodeWrap *stmtWrap;

  if (stmt->getElseBody() != nullptr)
  {
    lattice elseOut = analyseStatement(stmt->getElseBody(), condOut);
    stmtWrap = getWrapNode(stmt, ifOut ^ elseOut);
  }
  else
    stmtWrap = getWrapNode(stmt, ifOut ^ condOut);

  stmtWrap->outMap = stmtWrap->inMap;
  return stmtWrap->outMap;
}

lattice deviceVarsAnalyser::analyseUnary(unary_stmt *stmt, lattice &inMap)
{
  Expression *unaryVar = stmt->getUnaryExpr()->getUnaryExpr();

  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  lattice outMap = inMap;
  if (unaryVar->isIdentifierExpr())
  {
    outMap.meet(unaryVar->getId(), lattice::CPU_WRITE);
  }
  else if (unaryVar->isPropIdExpr())
  {
    PropAccess *propId = unaryVar->getPropId();
    outMap.meet(propId->getIdentifier1(), lattice::CPU_READ);
    outMap.meet(propId->getIdentifier2(), lattice::CPU_WRITE);
  }
  wrapNode->outMap = outMap;

  return outMap;
}

/*lattice deviceVarsAnalyser::analyseWhile(whileStmt *stmt, lattice &inp)
{
}

lattice deviceVarsAnalyser::analyseDoWhile(dowhileStmt *stmt, lattice &inp)
{
}

lattice deviceVarsAnalyser::analyseFixedPoint(fixedPointStmt *stmt, lattice &inp)
{
}

lattice deviceVarsAnalyser::analyseItrBFS(iterateBFS *stmt, lattice &inp)
{
}

lattice deviceVarsAnalyser::analyseFor(forallStmt *stmt, lattice &inp)
{
}

lattice deviceVarsAnalyser::analyseReduction(reductionCallStmt *stmt, lattice &inp)
{

}*/

lattice deviceVarsAnalyser::analyseStatement(statement *stmt, lattice &inMap)
{
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
    /*else
      return analyseFor((forallStmt*)stmt, inMap);*/
  }
  case NODE_BLOCKSTMT:
    return analyseBlock((blockStatement *)stmt, inMap);
  case NODE_UNARYSTMT:
    return analyseUnary((unary_stmt *)stmt, inMap);
  case NODE_IFSTMT:
    return analyseIfElse((ifStmt *)stmt, inMap);

    /*
    case NODE_WHILESTMT:
      return analyseWhile((whileStmt *)stmt, inMap);
    case NODE_PROCCALLSTMT:
      // TODO : Add proc call statment
      return inMap;
    case NODE_REDUCTIONCALLSTMT:
      return analyseReduction((reductionCallStmt *)stmt, inMap);
    case NODE_ITRBFS:
      return analyseItrBFS((iterateBFS *)stmt, inMap);
    case NODE_DOWHILESTMT:
      return analyseDoWhile((dowhileStmt *)stmt, inMap);
    case NODE_FIXEDPTSTMT:
      return analyseFixedPoint((fixedPointStmt *)stmt, inMap);*/
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
    // printf("%p %d\n", bstmt, bstmt->getTypeofNode());
    lastOut = analyseStatement(bstmt, lastOut);
    if (bstmt->getTypeofNode() == NODE_DECL)
      declaredVars.push_back(((declaration *)bstmt)->getdeclId());
  }
  for (Identifier *iden : declaredVars)
    lastOut.removeVariable(iden);

  wrapNode->outMap = lastOut;
  return lastOut;
}

void printTabs(int tabSpace)
{
  while (tabSpace--)
    cout << '\t';
}

void deviceVarsAnalyser::printBlock(blockStatement *stmt, int tabSpace)
{
  printTabs(tabSpace);
  cout << "Block Begin\n";
  tabSpace++;
  for (statement *bstmt : stmt->returnStatements())
  {
    printStatement(bstmt, tabSpace);
  }
  tabSpace--;
  printTabs(tabSpace);
  cout << "Block End\n";
}

void deviceVarsAnalyser::printDeclaration(declaration *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "Declaration: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printAssignment(assignment *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "Assignment: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printIfElse(ifStmt *stmt, int tabSpace)
{
  printTabs(tabSpace);
  cout << "If Statement\n";

  tabSpace++;
  printTabs(tabSpace);
  cout << "Condition: ";

  Expression *cond = stmt->getCondition();
  ASTNodeWrap *wrapNode = getWrapNode(cond);
  (wrapNode->outMap).print();

  printTabs(tabSpace);
  cout << "If Body\n";
  tabSpace++;
  printStatement(stmt->getIfBody(), tabSpace);
  tabSpace--;

  if (stmt->getElseBody() != nullptr)
  {
    printTabs(tabSpace);
    cout << "Else Body\n";
    tabSpace++;
    printStatement(stmt->getElseBody(), tabSpace);
    tabSpace--;
  }

  ASTNodeWrap *stmtWrap = getWrapNode(stmt);

  printTabs(tabSpace);
  cout << "Merge point: ";
  (stmtWrap->outMap).print();
  tabSpace--;
}

void deviceVarsAnalyser::printUnary(unary_stmt *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "Unary statment: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printForAll(forallStmt *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "For All stmt: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printStatement(statement *stmt, int tabSpace)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
    printDeclaration((declaration *)stmt, tabSpace);
    break;
  case NODE_ASSIGN:
    printAssignment((assignment *)stmt, tabSpace);
    break;
  case NODE_FORALLSTMT:
  {
    forallStmt *forStmt = (forallStmt *)stmt;
    if (forStmt->isForall())
      printForAll((forallStmt *)stmt, tabSpace);
    break;
  }
  case NODE_BLOCKSTMT:
    printBlock((blockStatement *)stmt, tabSpace);
    break;
  case NODE_UNARYSTMT:
    printUnary((unary_stmt *)stmt, tabSpace);
    break;
  case NODE_IFSTMT:
    printIfElse((ifStmt *)stmt, tabSpace);
    break;
  }
}

void deviceVarsAnalyser::analyseFunc(ASTNode *proc)
{
  Function *func = (Function *)proc;

  lattice inpLattice;
  analyseStatement(func->getBlockStatement(), inpLattice);
  printStatement(func->getBlockStatement(), 0);
  return;
}

void deviceVarsAnalyser::analyse()
{
  list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
    analyseFunc(func);
}