#include "ASTAnalyser1.h"
#include <string.h>

usedVariables ASTAnalyser1::getVarsExpr(Expression* expr)
{
  usedVariables result;

  if(expr->isIdentifierExpr())
  {
    Identifier* iden = expr->getId();
    result.addVariable(iden);
  }
  else if(expr->isPropIdExpr())
  {
    PropAccess* propExpr = expr->getPropId();
    result.addVariable(propExpr->getIdentifier2());
  }
  else if(expr->isUnary())
  {
    result = getVarsExpr(expr->getUnaryExpr());
  }
  else if(expr->isLogical() || expr->isArithmetic() || expr->isLogical())
  {
    result = getVarsExpr(expr->getLeft());
    result.merge(getVarsExpr(expr->getRight()));
  }
  return result;
}

usedVariables ASTAnalyser1::getVarsWhile(whileStmt* stmt)
{
    usedVariables currVars = getVarsExpr(stmt->getCondition());
    currVars.merge(getVarsStatement(stmt->getBody()));

    return currVars;
}
usedVariables ASTAnalyser1::getVarsDoWhile(dowhileStmt* stmt)
{
    usedVariables currVars = getVarsExpr(stmt->getCondition());
    currVars.merge(getVarsStatement(stmt->getBody()));

    return currVars;
}
usedVariables ASTAnalyser1::getVarsAssignment(assignment* stmt)
{
  usedVariables currVars;
  if(stmt->lhs_isProp())
  {
    PropAccess* propId = stmt->getPropId();
    currVars.addVariable(propId->getIdentifier2());
  }
  else if(stmt->lhs_isIdentifier())
    currVars.addVariable(stmt->getId());
  
  currVars.merge(getVarsExpr(stmt->getExpr()));

  return currVars;
}
usedVariables ASTAnalyser1::getVarsIf(ifStmt* stmt)
{
    usedVariables currVars = getVarsExpr(stmt->getCondition());
    currVars.merge(getVarsStatement(stmt->getIfBody()));
    if(stmt->getElseBody() != NULL)
      currVars.merge(getVarsStatement(stmt->getElseBody()));
    
    return currVars;
}
usedVariables ASTAnalyser1::getVarsFixedPoint(fixedPointStmt* stmt)
{
    usedVariables currVars = getVarsExpr(stmt->getDependentProp());
    currVars.addVariable(stmt->getFixedPointId());
    currVars.merge(getVarsStatement(stmt->getBody()));
    return currVars;
}
usedVariables ASTAnalyser1::getVarsReduction(reductionCallStmt* stmt)
{
    usedVariables currVars;

    return currVars;
}
usedVariables ASTAnalyser1::getVarsBFS(iterateBFS* stmt)
{
    usedVariables currVars;

    return currVars;
}

usedVariables ASTAnalyser1::getVarsStatement(statement* stmt)
{
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
      return getVarsBlock(stmt);
    
    case NODE_WHILESTMT:
      return getVarsWhile((whileStmt*) stmt);

    case NODE_DOWHILESTMT:
      return getVarsDoWhile((dowhileStmt*) stmt);

    case NODE_FIXEDPTSTMT:
      return getVarsFixedPoint((fixedPointStmt*) stmt);

    case NODE_IFSTMT:
      return getVarsIf((ifStmt*) stmt);

    case NODE_ITRBFS:
      return getVarsBFS((iterateBFS*) stmt);

    case NODE_REDUCTIONCALLSTMT:
      return getVarsReduction((reductionCallStmt*) stmt);
    }

    return usedVariables();
}

usedVariables ASTAnalyser1::getVarsBlock(blockStatement *blockStmt)
{
    list<statement*> stmtList = blockStmt->returnStatements();
    usedVariables currVars;
    list<Identifier*> declVars;

    for(statement* stmt: stmtList)
    {
      if(stmt->getTypeofNode() == NODE_DECL)
      {
          declaration* decl = (declaration*) stmt;
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

void ASTAnalyser1::analyseForAll(forallStmt* stmt)
{
    Identifier* itr = stmt->getIterator();
    Identifier* graph = stmt->getSourceGraph();

    usedVariables vars = getVarsStatment(stmt->getBody());
}

void ASTAnalyser1::analyseStatement(statement* stmt)
{  
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
      for(statement* stmtB: ((blockStatement*) stmt)->returnStatements())
        analyseStatement(stmtB);
      break;
    
    case NODE_WHILESTMT:
      analyseStatement(((whileStmt*) stmt)->getBody());
      break;

    case NODE_DOWHILESTMT:
      analyseStatement(((dowhileStmt*) stmt)->getBody());
      break;

    case NODE_FIXEDPTSTMT:
      analyseStatement(((fixedPointStmt*) stmt)->getBody());
      break;

    case NODE_IFSTMT:
      statement* ifBody = ((ifStmt*) stmt)->getIfBody();
      statement* elseBody = ((ifStmt*) stmt)->getElseBody();
      if(ifBody != NULL) analyseStatement(ifBody);
      if(elseBody != NULL) analyseStatement(elseBody);
      break;

    case NODE_ITRBFS:
      analyseStatement(((iterateBFS*) stmt)->getBody());
      iterateReverseBFS* revBFS = ((iterateBFS*) stmt)->getRBFS();
      if(revBFS != NULL) analyseStatement(revBFS->getBody());
      break;

    case NODE_FORALLSTMT:
      analyseForAll((forallStmt*) stmt);
      break;
    }
}



void ASTAnalyser1::analyseFunc(ASTNode* proc)
{
    Function *func = (Function *)proc;
    analyseStatement(func->getBlockStatement());
    return;
}

void ASTAnalyser1::analyse()
{
    list<Function*> funcList = frontEndContext.getFuncList();
    for(Function* func: funcList)
        analyseFunc(func);
}