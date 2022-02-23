#include "deviceVarsAnalyser.h"
#include <string.h>
#include <unordered_map>
#include "analyserUtil.hpp"

/*
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
}*/

usedVariables seperatePropAccess(usedVariables inp)
{
  list<PropAccess*> ReadPropId = inp.getPropAcess(READ);
  list<PropAccess*> WritePropId = inp.getPropAcess(WRITE);

  usedVariables out;

  for(Identifier* iden: inp.getVariables(READ)){
    out.addVariable(iden, READ);
  }

  for(Identifier* iden: inp.getVariables(WRITE)){
    out.addVariable(iden, WRITE);
  }

  for(PropAccess* prop: inp.getPropAcess(READ)){
    out.addVariable(prop->getIdentifier1(), READ);
    out.addVariable(prop->getIdentifier2(), READ);
  }

  for(PropAccess* prop: inp.getPropAcess(WRITE)){
    out.addVariable(prop->getIdentifier1(), READ);
    out.addVariable(prop->getIdentifier2(), WRITE);
  }

  return out;
}

usedVariables deviceVarsAnalyser::getVarsAssignment(assignment *stmt)
{
  usedVariables currVars;
  if (stmt->lhs_isProp())
  {
    PropAccess *propId = stmt->getPropId();
    currVars.addPropAccess(propId->getIdentifier1(), READ);
    currVars.addPropAccess(propId->getIdentifier2(), WRITE);
  }
  else if (stmt->lhs_isIdentifier())
    currVars.addVariable(stmt->getId(), WRITE);

  usedVariables exprVars = seperatePropAccess(getVarsExpr(stmt->getExpr));
  currVars.merge(exprVars);
  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsIf(ifStmt *stmt)
{
  usedVariables currVars = seperatePropAccess(getVarsExpr(stmt->getCondition()));
  currVars.merge(getVarsStatement(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(getVarsStatement(stmt->getElseBody()));

  return currVars;
}
/*
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
}*/

usedVariables deviceVarsAnalyser::getVarsUnary(unary_stmt *stmt)
{
  Expression *unaryExpr = stmt->getUnaryExpr();
  Expression *varExpr = unaryExpr->getUnaryExpr();

  usedVariables currUsed;
  if (varExpr->isIdentifierExpr())
    currUsed.addVariable(varExpr->getId(), READ_WRITE);
  else if (varExpr->isPropIdExpr())
  {
    currUsed.addVariable(varExpr->getPropId()->getIdentifier1(), READ);
    currUsed.addVariable(varExpr->getPropId()->getIdentifier2(), WRITE);
  }

  return currUsed;
}

/*
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
}*/

usedVariables deviceVarsAnalyser::getVarsForAll(forallStmt *stmt)
{
  usedVariables currVars = getVarsStatement(stmt->getBody());
  currVars.removeVariable(stmt->getIterator(), READ_WRITE);

  if(stmt->hasFilterExpr())
    currVars.merge(seperatePropAccess(getVarsExpr(stmt->getfilterExpr())));
  
  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsBlock(blockStatement *blockStmt)
{
  list<statement *> stmtList = blockStmt->returnStatements();
  list<Identifier *> declVars;

  usedVariables currVars;
  for (statement *stmt : stmtList)
  {
    if (stmt->getTypeofNode() == NODE_DECL)
    {
      declaration *decl = (declaration *)stmt;
      declVars.push_back(decl->getdeclId());

      if(decl->isInitialized())
      {
        usedVariables exprVars = getVarsExpr(decl->getExpressionAssigned());
        exprVars = seperatePropAccess(exprVars);

        for(Identifier* dVars: declVars){
          exprVars.removeVariable(dVars, READ_WRITE);
        }
        currVars.addVariable(exprVars);
      }
    }
    else
    {
      usedVariables stmtVars = getVarsStatement(stmt);
      for(Identifier* dVars: declVars){
        stmtVars.removeVariable(dVars, READ_WRITE);
      }

      currVars.merge(stmtVars);
    }
  }

  return currVars;
}

usedVariables deviceVarsAnalyser::getVarsStatement(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    return getVarsBlock((blockStatement*) stmt);

  case NODE_ASSIGN:
    return getVarsAssignment((assignment*) stmt);

  case NODE_UNARYSTMT:
    return getVarsUnary((unary_stmt*) stmt);

  case NODE_IFSTMT:
    return getVarsIf((ifStmt *)stmt);

  /*case NODE_ITRBFS:
    return getVarsBFS((iterateBFS *)stmt);

  case NODE_REDUCTIONCALLSTMT:
    return getVarsReduction((reductionCallStmt *)stmt);

  case NODE_WHILESTMT:
    return getVarsWhile((whileStmt *)stmt);

  case NODE_DOWHILESTMT:
    return getVarsDoWhile((dowhileStmt *)stmt);

  case NODE_FIXEDPTSTMT:
    return getVarsFixedPoint((fixedPointStmt *)stmt);*/
  }

  return usedVariables();
}

//Statement Analyser
//Current assuming filter expression gets evaluated in GPU

void deviceVarsAnalyser::analyseForAll(forallStmt *stmt, lattice& inp)
{
  ASTNodeWrap* wrapNode = getWrapNode(stmt, inp);
  wrapNode->inMap = inp;

  lattice outMap = inp;

  usedVariables vars = getVarsForAll(stmt);
  for(Identifier* iden: vars.getVariables(READ)){
    outMap.meet(iden, lattice::GPU_READ);
  }
  for(Identifier* iden: vars.getVariables(READ)){
    outMap.meet(iden, lattice::GPU_WRITE);
  }

  wrapNode->outMap = outMap;
  return outMap;
}

lattice deviceVarsAnalyser::analyseDeclaration(declaration *stmt, lattice &inMap)
{
  ASTNodeWrap* wrapNode = getWrapNode(stmt, inMap);
  
  lattice outMap = inMap;
  outMap.addVariable(stmt->getdeclId(), lattice::CPU_ONLY);

  wrapNode->outMap = outMap;
  return outMap;
}

lattice deviceVarsAnalyser::analyseAssignment(assignment *stmt, lattice &inMap)
{
  ASTNodeWrap* wrapNode = getWrapNode(stmt, inMap);
  usedVariables usedVars = getVarsAssignment(stmt);

  wrapNode->inMap = inMap;

  lattice outMap = wrapNode->inMap;
  for(Identifier* iden: usedVars.getVariables(READ)){
    outMap.meet(iden, lattice::CPU_READ);  
  }
  for(Identifier* iden: usedVars.getVariables(WRITE)){
    outMap.meet(iden, lattice::CPU_WRITE);
  }
}

lattice deviceVarsAnalyser::analyseIfElse(ifStmt *stmt, lattice &inMap)
{
  Expression* cond = stmt->getCondition();
  ASTNodeWrap* wrapNode = getWrapNode(cond);
  wrapNode->inMap = inMap;

  lattice condOut = inMap;
  usedVariables exprVars = seperatePropAccess(getVarsExpr(cond));

  for(Identifier* iden: exprVars.getVariables(READ)){
    condOut.meet(iden, lattice::CPU_READ);
  }
  for(Identifier* iden: exprVars.getVariables(WRITE)){
    condOut.meet(iden, lattice::CPU_WRITE);
  }

  wrapNode->outMap = condOut;

  lattice ifOut = analyseStatement(stmt->getIfBody(), condOut);
  ASTNodeWrap* stmtWrap;

  if(stmt->getElseBody() != nullptr)
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
  Expression* unaryVar = stmt->getUnaryExpr()->getUnaryExpr();

  ASTNodeWrap *wrapNode = getWrapNode(stmt, inMap);
  wrapNode->inMap = inMap;

  lattice outMap = inMap;
  if(unaryVar->isIdentifierExpr()){
    outMap.meet(unaryVar->getId(), lattice::CPU_WRITE);
  }
  else if(unaryVar->isPropIdExpr())
  {
    PropAccess* propId = unaryVar->getPropId();
    outMap.meet(propId->getIdentifier1(), lattice::CPU_READ);
    outMap.meet(propId->getIdentifier2(), lattice::CPU_WRITE);
  }
  wrapNode->outMap = outMap;

  return outMap;
}

/*lattice deviceVarsAnalyser::analyseWhile(whileStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseDoWhile(dowhileStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseFixedPoint(fixedPointStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseItrBFS(iterateBFS *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseFor(forallStmt *stmt, lattice &inMap)
{
}

lattice deviceVarsAnalyser::analyseReduction(reductionCallStmt *stmt, lattice &inMap)
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
    forallStmt* forStmt = (forallStmt*) stmt;

    if(forStmt->isForall())
      return analyseForAll((forallStmt*)stmt, inMap);
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
  while(tabSpace--)
    cout<<'\t';
}

void deviceVarsAnalyser::printBlock(blockStatement *stmt, int tabSpace)
{
  printTabs(tabSpace);
  cout<<"Block Begin\n";
  tabSpace++;
  for (statement *bstmt : stmt->returnStatements()){
    printStatement(bstmt);
  }
  tabSpace--;
  printTabs(tabSpace);
  cout<<"Block End\n";
}


void deviceVarsAnalyser::printDeclaration(declaration *stmt, int tabSpace)
{
  ASTNodeWrap* wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout<<"Declaration: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printAssignment(assignment *stmt, int tabSpace)
{
  ASTNodeWrap* wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout<<"Assignment: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printIfElse(ifStmt *stmt, int tabSpace)
{
  printTabs(tabSpace);
  cout<<"If Statement\n";

  tabSpace++;
  printTabs(tabSpace);
  cout<<"Condition: ";

  Expression* cond = stmt->getCondition();
  ASTNodeWrap* wrapNode = getWrapNode(cond);
  (wrapNode->outMap).print();

  cout<<"If Body\n";
  tabSpace++;
  printStatement(stmt->getIfBody(), tabSpace);
  tabSpace--;

  if(stmt->getElseBody() != nullptr)
  {
    cout<<"Else Body\n";
    tabSpace++;
    printStatement(stmt->getElseBody(), tabSpace);
    tabSpace--;
  }
  
  printTabs();
  (stmtWrap->outMap).print();
  tabSpace--;
}

void deviceVarsAnalyser::printUnary(unary_stmt *stmt)
{
  ASTNodeWrap* wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout<<"Unary statment: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printStatement(statement *stmt, int tabSpace)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
    printDeclaration((declaration *)stmt, tabSpace);
  case NODE_ASSIGN:
    printAssignment((assignment *)stmt, tabSpace);
  case NODE_FORALLSTMT:
  {
    forallStmt* forStmt = (forallStmt*) stmt;

    if(forStmt->isForall())
      printForAll((forallStmt*)stmt, tabSpace);
  }
  case NODE_BLOCKSTMT:
    printBlock((blockStatement *)stmt, tabSpace);
  case NODE_UNARYSTMT:
    printUnary((unary_stmt *)stmt, tabSpace);
  case NODE_IFSTMT:
    printIfElse((ifStmt *)stmt, tabSpace);
  }

}



void deviceVarsAnalyser::analyseFunc(ASTNode *proc)
{
  Function *func = (Function *)proc;

  lattice inpLattice;
  analyseStatement(func->getBlockStatement(), inpLattice);
  return;
}

void deviceVarsAnalyser::analyse()
{
  list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
    analyseFunc(func);
}