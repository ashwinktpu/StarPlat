#include "getUsedVarsCpu.h"
#include "analyserUtil.cpp"
usedVariables getVarsStatementCpu(statement *stmt);
usedVariables getVarsExprCpu(Expression *expr)
{
  usedVariables result;

  if (expr->isIdentifierExpr())
  {
    Identifier *iden = expr->getId();
    result.addVariable(iden, READ);
  }
  else if (expr->isPropIdExpr())
  {
    PropAccess *propExpr = expr->getPropId();
    result.addVariable(propExpr->getIdentifier1(), READ);
    result.addVariable(propExpr->getIdentifier2(), READ);
  }
  else if (expr->isUnary())
  {
    if (expr->getOperatorType() == OPERATOR_NOT)
      result = getVarsExprCpu(expr->getUnaryExpr());
    else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC))
    {
      Expression *uExpr = expr->getUnaryExpr();
      if (uExpr->isIdentifierExpr())
        result.addVariable(uExpr->getId(), READ_WRITE);
      else if (uExpr->isPropIdExpr())
      {
        PropAccess *propId = uExpr->getPropId();
        result.addVariable(propId->getIdentifier1(), READ);
        result.addVariable(propId->getIdentifier2(), READ_WRITE);
      }
    }
  }
  else if (expr->isLogical() || expr->isArithmetic() || expr->isRelational())
  {
    result = getVarsExprCpu(expr->getLeft());
    result.merge(getVarsExprCpu(expr->getRight()));
  }
  return result;
}

usedVariables getVarsWhileCpu(whileStmt *stmt)
{
  usedVariables currVars = getVarsExprCpu(stmt->getCondition());
  currVars.merge(getVarsStatementCpu(stmt->getBody()));

  return currVars;
}

usedVariables getVarsDoWhileCpu(dowhileStmt *stmt)
{
  usedVariables currVars = getVarsExprCpu(stmt->getCondition());
  currVars.merge(getVarsStatementCpu(stmt->getBody()));

  return currVars;
}

usedVariables getVarsAssignmentCpu(assignment *stmt)
{
  usedVariables currVars;
  if (stmt->lhs_isProp())
  {
    PropAccess *propId = stmt->getPropId();
    currVars.addVariable(propId->getIdentifier1(), READ);
    currVars.addVariable(propId->getIdentifier2(), WRITE);
  }
  else if (stmt->lhs_isIdentifier())
    currVars.addVariable(stmt->getId(), WRITE);

  usedVariables exprVars = getVarsExprCpu(stmt->getExpr());
  currVars.merge(exprVars);
  return currVars;
}

usedVariables getVarsIfCpu(ifStmt *stmt)
{
  usedVariables currVars = getVarsExprCpu(stmt->getCondition());
  currVars.merge(getVarsStatementCpu(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(getVarsStatementCpu(stmt->getElseBody()));

  return currVars;
}

usedVariables getVarsFixedPointCpu(fixedPointStmt *stmt)
{
  usedVariables currVars = getVarsExprCpu(stmt->getDependentProp());
  currVars.addVariable(stmt->getFixedPointId(), READ);
  currVars.merge(getVarsStatementCpu(stmt->getBody()));
  return currVars;
}
// TODO : Handle this atlast

usedVariables getVarsReductionCpu(reductionCallStmt *stmt)
{
  usedVariables currVars;

  auto getVarsReductionCpuCall = [&currVars](reductionCall *callExpr) -> void
  {
    for (argument *arg : callExpr->getargList())
    {
      if (arg->isExpr())
        currVars.merge(getVarsExprCpu(arg->getExpr()));
    }
  };

  if (stmt->is_reducCall())
  {
    if (stmt->getLhsType() == 1)
    {
      currVars.addVariable(stmt->getLeftId(), WRITE);
      getVarsReductionCpuCall(stmt->getReducCall());
    }
    else if (stmt->getLhsType() == 2)
    {
      PropAccess *propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), READ);
      currVars.addVariable(propId->getIdentifier2(), WRITE);

      getVarsReductionCpuCall(stmt->getReducCall());
    }
    else if (stmt->getLhsType() == 3)
    {
      for (ASTNode *node : stmt->getLeftList())
      {
        if (node->getTypeofNode() == NODE_ID)
        {
          Identifier *iden = (Identifier *)node;
          currVars.addVariable(iden, WRITE);
        }
        else if (node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess *propId = (PropAccess *)node;
          currVars.addVariable(propId->getIdentifier1(), READ);
          currVars.addVariable(propId->getIdentifier2(), WRITE);
        }
      }
      getVarsReductionCpuCall(stmt->getReducCall());

      for (ASTNode *node : stmt->getRightList())
      {
        if (node->getTypeofNode() == NODE_ID)
        {
          Identifier *iden = (Identifier *)node;
          currVars.addVariable(iden, WRITE);
        }
        else if (node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess *propId = (PropAccess *)node;
          currVars.addVariable(propId->getIdentifier1(), READ);
          currVars.addVariable(propId->getIdentifier2(), WRITE);
        }
      }
    }
  }
  else
  {
    if (stmt->isLeftIdentifier())
    {
      currVars.addVariable(stmt->getLeftId(), READ_WRITE);
      currVars.merge(getVarsExprCpu(stmt->getRightSide()));
    }
    else
    {
      PropAccess *propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), READ);
      currVars.addVariable(propId->getIdentifier2(), READ_WRITE);

      currVars.merge(getVarsExprCpu(stmt->getRightSide()));
    }
  }
  return currVars;
}

usedVariables getVarsUnaryCpu(unary_stmt *stmt)
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
usedVariables getVarsBFS(iterateBFS *stmt)
{
  usedVariables currVars = getVarsStatementCpu(stmt->getBody());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(RBFSstmt->getBFSFilter());
    currVars.merge(RBFSstmt->getBody());
  }

  return currVars;
}*/

usedVariables getVarsForAllCpu(forallStmt *stmt)
{
  usedVariables currVars = getVarsStatementCpu(stmt->getBody());
  currVars.removeVariable(stmt->getIterator(), READ_WRITE);

  if (stmt->isSourceProcCall())
  {
    proc_callExpr *expr = stmt->getExtractElementFunc();
    for (argument *arg : expr->getArgList())
    {
      if (arg->getExpr() != nullptr)
        currVars.merge(getVarsExprCpu(arg->getExpr()));
    }
  }
  else if (!stmt->isSourceField())
  {
    Identifier *iden = stmt->getSource();
    currVars.addVariable(iden, READ);
  }
  else
  {
    PropAccess *propId = stmt->getPropSource();
    currVars.addVariable(propId->getIdentifier1(), READ);
  }

  if (stmt->hasFilterExpr())
    currVars.merge(getVarsExprCpu(stmt->getfilterExpr()));

  return currVars;
}

usedVariables getVarsBlockCpu(blockStatement *blockStmt)
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

      if (decl->isInitialized())
      {
        usedVariables exprVars = getVarsExprCpu(decl->getExpressionAssigned());
        for (Identifier *dVars : declVars)
          exprVars.removeVariable(dVars, READ_WRITE);

        currVars.merge(exprVars);
      }
    }
    else
    {
      usedVariables stmtVars = getVarsStatementCpu(stmt);
      for (Identifier *dVars : declVars)
        stmtVars.removeVariable(dVars, READ_WRITE);

      currVars.merge(stmtVars);
    }
  }
  return currVars;
}
/* Function: getDeclaredPropertyVarsOfBlockCpu
 * Return all the Property variables declared in a the block!
 * Used for cudaFree those variables
 * usedVariables may be a misnomer?! for this function.
 * --rajesh
 */
usedVariables getDeclaredPropertyVarsOfBlockCpu(blockStatement *blockStmt)
{
  list<statement *> stmtList = blockStmt->returnStatements();
  list<Identifier *> declVars;

  usedVariables currVars;
  for (statement *stmt : stmtList)
  {
    if (stmt->getTypeofNode() == NODE_DECL)
    {
      declaration *decl = (declaration *)stmt;
      if (decl->getType()->isPropType())
      {
        currVars.addVariable(decl->getdeclId(), READ_WRITE); // 2nd arg may be not used by us
      }
    }
  }
 
  
  return currVars;
}

usedVariables getVarsStatementCpu(statement *stmt)
{
  cout << stmt->getTypeofNode() << endl;
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:{
   
    return getVarsBlockCpu((blockStatement *)stmt);
  }
  case NODE_ASSIGN:
    return getVarsAssignmentCpu((assignment *)stmt);

  case NODE_UNARYSTMT:
    return getVarsUnaryCpu((unary_stmt *)stmt);

  case NODE_IFSTMT:
    return getVarsIfCpu((ifStmt *)stmt);

  case NODE_WHILESTMT:
    return getVarsWhileCpu((whileStmt *)stmt);

  case NODE_DOWHILESTMT:
    return getVarsDoWhileCpu((dowhileStmt *)stmt);

  case NODE_REDUCTIONCALLSTMT:
    return getVarsReductionCpu((reductionCallStmt *)stmt);

    /*case NODE_ITRBFS:
      return getVarsBFS((iterateBFS *)stmt);*/

  case NODE_FIXEDPTSTMT:
    return getVarsFixedPointCpu((fixedPointStmt *)stmt);
  default:; // added to fix warning!
  }

  return usedVariables();
}