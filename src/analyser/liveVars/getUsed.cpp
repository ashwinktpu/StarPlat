#include "liveVarsAnalyser.h"

usedVariables liveVarsAnalyser::getVarsExpr(Expression *expr)
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
            result = getVarsExpr(expr->getUnaryExpr());
        else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC))
        {
            Expression *uExpr = expr->getUnaryExpr();
            if (uExpr->isIdentifierExpr())
                result.addVariable(uExpr->getId(), READ_WRITE);
            else if (uExpr->isPropIdExpr())
            {
                PropAccess* propId = uExpr->getPropId();
                result.addVariable(propId->getIdentifier1(), READ);
                result.addVariable(propId->getIdentifier2(), READ_WRITE);
            }
        }
    }
    else if (expr->isLogical() || expr->isArithmetic() || expr->isRelational())
    {
        result = getVarsExpr(expr->getLeft());
        result.merge(getVarsExpr(expr->getRight()));
    }
    else if (expr->isProcCallExpr()) {
        result = getVarsExprProcCall((proc_callExpr*) expr);
    }
    return result;
}

usedVariables liveVarsAnalyser::getVarsWhile(whileStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsDoWhile(dowhileStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsDeclaration(declaration *stmt)
{
  usedVariables currVars;
  currVars.addVariable(stmt->getdeclId(), WRITE);
  if(stmt->isInitialized())
  {
    usedVariables exprVars = getVarsExpr(stmt->getExpressionAssigned());
    currVars.merge(exprVars);
  }

  return currVars;
}

usedVariables liveVarsAnalyser::getVarsAssignment(assignment *stmt)
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

  usedVariables exprVars = getVarsExpr(stmt->getExpr());
  currVars.merge(exprVars);
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsIf(ifStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(getVarsStatement(stmt->getElseBody()));

  return currVars;
}

usedVariables liveVarsAnalyser::getVarsFixedPoint(fixedPointStmt *stmt)
{
  usedVariables currVars = getVarsExpr(stmt->getDependentProp());
  currVars.addVariable(stmt->getFixedPointId(), READ_WRITE);
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsUnary(unary_stmt *stmt)
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

usedVariables liveVarsAnalyser::getVarsBFS(iterateBFS *stmt)
{
  usedVariables currVars = getVarsStatement(stmt->getBody());
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsRBFS(iterateReverseBFS *stmt)
{
  usedVariables currVars = getVarsStatement(stmt->getBody());
  if(stmt->hasFilter())
  {
    currVars.merge(getVarsExpr(stmt->getBFSFilter()));
  }

  return currVars;
}

usedVariables liveVarsAnalyser::getVarsForAll(forallStmt *stmt)
{
  usedVariables currVars = getVarsStatement(stmt->getBody());
  currVars.removeVariable(stmt->getIterator(), READ_WRITE);

  if(stmt->isSourceProcCall())
  {
      proc_callExpr *expr = stmt->getExtractElementFunc();
        for(argument* arg: expr->getArgList()){
            if(arg->getExpr() != nullptr) 
                currVars.merge(getVarsExpr(arg->getExpr()));
        }
  }
  else if(!stmt->isSourceField())
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
    currVars.merge(getVarsExpr(stmt->getfilterExpr()));

  return currVars;
}

usedVariables liveVarsAnalyser::getVarsBlock(blockStatement *blockStmt)
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
        usedVariables exprVars = getVarsExpr(decl->getExpressionAssigned());
        currVars.merge(exprVars);
      }
    }
    else
    {
      usedVariables stmtVars = getVarsStatement(stmt);
      currVars.merge(stmtVars);
    }
  }

  for (Identifier *dVars : declVars)
    currVars.removeVariable(dVars, READ_WRITE);

  return currVars;
}

usedVariables liveVarsAnalyser::getVarsReduction(reductionCallStmt *stmt)
{
  usedVariables currVars;

  auto getVarsReductionCall = [this, &currVars](reductionCall* callExpr) -> void
  {
    for(argument* arg: callExpr->getargList()){
      if(arg->isExpr())
        currVars.merge(getVarsExpr(arg->getExpr()));
    }
  };

  if(stmt->is_reducCall())
  {
    if(stmt->getLhsType() == 1)
    {
        currVars.addVariable(stmt->getLeftId(), WRITE);
        getVarsReductionCall(stmt->getReducCall());
    }
    else if(stmt->getLhsType() == 2)
    {
      PropAccess* propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), READ);
      currVars.addVariable(propId->getIdentifier2(), WRITE);

      getVarsReductionCall(stmt->getReducCall());
    }
    else if(stmt->getLhsType() == 3)
    {
      for(ASTNode* node: stmt->getLeftList())
      {
        Identifier* affectedId = NULL;
        if(node->getTypeofNode() == NODE_ID)
        {
          Identifier* iden = (Identifier*)node;
          currVars.addVariable(iden, WRITE);
          affectedId = iden;
        }
        else if(node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess* propId = (PropAccess*)node;
          currVars.addVariable(propId->getIdentifier1(), READ);
          currVars.addVariable(propId->getIdentifier2(), WRITE);
          affectedId = propId->getIdentifier2();
        }

        if(affectedId->getSymbolInfo()->getId()->get_fp_association()) {
          Identifier* fpId = affectedId->getSymbolInfo()->getId()->get_fpIdNode();
          currVars.addVariable(fpId, WRITE);
        }
      }
      getVarsReductionCall(stmt->getReducCall());
      
      for(ASTNode* node: stmt->getRightList())
      {
        if(node->getTypeofNode() == NODE_ID)
        {
          Identifier* iden = (Identifier*)node;
          currVars.addVariable(iden, WRITE);
        }
        else if(node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess* propId = (PropAccess*)node;
          currVars.addVariable(propId->getIdentifier1(), READ);
          currVars.addVariable(propId->getIdentifier2(), WRITE);
        }
      }
    }
  }
  else
  {
    if(stmt->isLeftIdentifier())
    {
      currVars.addVariable(stmt->getLeftId(), READ_WRITE);
      currVars.merge(getVarsExpr(stmt->getRightSide()));
    }
    else
    {
      PropAccess* propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), READ);
      currVars.addVariable(propId->getIdentifier2(), READ_WRITE);

      currVars.merge(getVarsExpr(stmt->getRightSide()));
    }
  }
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsProcCall(proc_callStmt *procCall)
{
  proc_callExpr *callExpr = procCall->getProcCallExpr();
  return getVarsExprProcCall(callExpr);
}

usedVariables liveVarsAnalyser::getVarsExprProcCall(proc_callExpr *procCall)
{
  usedVariables currVars;
  // if(procCall->getId1() != NULL)
  //   currVars.addVariable(procCall->getId1(), READ);
  for (argument *arg : procCall->getArgList())
  {
    if (arg->isExpr())
      currVars.merge(getVarsExpr(arg->getExpr()));
    else if (arg->isAssignExpr())
      currVars.merge(getVarsAssignment(arg->getAssignExpr()));
  }
  
  return currVars;
}

usedVariables liveVarsAnalyser::getVarsStatement(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    return getVarsBlock((blockStatement *)stmt);

  case NODE_DECL:
    return getVarsDeclaration((declaration *)stmt);

  case NODE_ASSIGN:
    return getVarsAssignment((assignment *)stmt);

  case NODE_UNARYSTMT:
    return getVarsUnary((unary_stmt *)stmt);

  case NODE_IFSTMT:
    return getVarsIf((ifStmt *)stmt);

  case NODE_WHILESTMT:
      return getVarsWhile((whileStmt *)stmt);

  case NODE_DOWHILESTMT:
      return getVarsDoWhile((dowhileStmt *)stmt);
  
  case NODE_FORALLSTMT:
      return getVarsForAll((forallStmt *) stmt);

  case NODE_REDUCTIONCALLSTMT:
    return getVarsReduction((reductionCallStmt *)stmt);

  case NODE_ITRBFS:
    return getVarsBFS((iterateBFS *)stmt);

  case NODE_ITRRBFS:
    return getVarsRBFS((iterateReverseBFS *)stmt);

  case NODE_FIXEDPTSTMT:
    return getVarsFixedPoint((fixedPointStmt *)stmt);

  case NODE_PROCCALLSTMT:
    return getVarsProcCall((proc_callStmt *)stmt);
  }

  return usedVariables();
}