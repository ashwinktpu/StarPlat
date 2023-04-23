#include "blockVarsAnalyser.h"

usedVariables_t blockVarsAnalyser::getVarsExpr(Expression *expr)
{
    usedVariables_t result;

    if (expr->isIdentifierExpr())
    {
        Identifier *iden = expr->getId();
        result.addVariable(iden, USED);
    }
    else if (expr->isPropIdExpr())
    {
        PropAccess *propExpr = expr->getPropId();
        result.addVariable(propExpr->getIdentifier1(), USED);
        result.addVariable(propExpr->getIdentifier2(), USED);
    }
    else if (expr->isUnary())
    {
        if (expr->getOperatorType() == OPERATOR_NOT)
            result = getVarsExpr(expr->getUnaryExpr());
        else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC))
        {
            Expression *uExpr = expr->getUnaryExpr();
            if (uExpr->isIdentifierExpr())
                result.addVariable(uExpr->getId(), USED_DEFINED);
            else if (uExpr->isPropIdExpr())
            {
                PropAccess* propId = uExpr->getPropId();
                result.addVariable(propId->getIdentifier1(), USED);
                result.addVariable(propId->getIdentifier2(), USED_DEFINED);
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

usedVariables_t blockVarsAnalyser::getVarsWhile(whileStmt *stmt)
{
  usedVariables_t currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsDoWhile(dowhileStmt *stmt)
{
  usedVariables_t currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsDeclaration(declaration *stmt)
{
  usedVariables_t currVars;
  currVars.addVariable(stmt->getdeclId(), DEFINED);
  if(stmt->isInitialized())
  {
    usedVariables_t exprVars = getVarsExpr(stmt->getExpressionAssigned());
    currVars.merge(exprVars);
  }

  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsAssignment(assignment *stmt)
{
  usedVariables_t currVars;
  if (stmt->lhs_isProp())
  {
    PropAccess *propId = stmt->getPropId();
    currVars.addVariable(propId->getIdentifier1(), USED);
    currVars.addVariable(propId->getIdentifier2(), DEFINED);
  }
  else if (stmt->lhs_isIdentifier())
    currVars.addVariable(stmt->getId(), DEFINED);

  usedVariables_t exprVars = getVarsExpr(stmt->getExpr());
  currVars.merge(exprVars);
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsIf(ifStmt *stmt)
{
  usedVariables_t currVars = getVarsExpr(stmt->getCondition());
  currVars.merge(getVarsStatement(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(getVarsStatement(stmt->getElseBody()));

  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsFixedPoint(fixedPointStmt *stmt)
{
  usedVariables_t currVars = getVarsExpr(stmt->getDependentProp());
  currVars.addVariable(stmt->getFixedPointId(), USED_DEFINED);
  currVars.merge(getVarsStatement(stmt->getBody()));
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsUnary(unary_stmt *stmt)
{
  Expression *unaryExpr = stmt->getUnaryExpr();
  Expression *varExpr = unaryExpr->getUnaryExpr();

  usedVariables_t currUsed;
  if (varExpr->isIdentifierExpr())
    currUsed.addVariable(varExpr->getId(), USED_DEFINED);
  else if (varExpr->isPropIdExpr())
  {
    currUsed.addVariable(varExpr->getPropId()->getIdentifier1(), USED);
    currUsed.addVariable(varExpr->getPropId()->getIdentifier2(), DEFINED);
  }

  return currUsed;
}

usedVariables_t blockVarsAnalyser::getVarsBFS(iterateBFS *stmt)
{
  usedVariables_t currVars = getVarsStatement(stmt->getBody());
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsRBFS(iterateReverseBFS *stmt)
{
  usedVariables_t currVars = getVarsStatement(stmt->getBody());
  if(stmt->hasFilter())
  {
    currVars.merge(getVarsExpr(stmt->getBFSFilter()));
  }

  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsForAll(forallStmt *stmt)
{
  usedVariables_t currVars = getVarsStatement(stmt->getBody());
  currVars.removeVariable(stmt->getIterator(), USED_DEFINED);

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
      currVars.addVariable(iden, USED);
  }
  else
  {
      PropAccess *propId = stmt->getPropSource();
      currVars.addVariable(propId->getIdentifier1(), USED);
  }

  if (stmt->hasFilterExpr())
    currVars.merge(getVarsExpr(stmt->getfilterExpr()));

  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsBlock(blockStatement *blockStmt)
{
  list<statement *> stmtList = blockStmt->returnStatements();
  list<Identifier *> declVars;

  usedVariables_t currVars;
  for (statement *stmt : stmtList)
  {
    if (stmt->getTypeofNode() == NODE_DECL)
    {
      declaration *decl = (declaration *)stmt;
      declVars.push_back(decl->getdeclId());

      if (decl->isInitialized())
      {
        usedVariables_t exprVars = getVarsExpr(decl->getExpressionAssigned());
        currVars.merge(exprVars);
      }
    }
    else
    {
      usedVariables_t stmtVars = getVarsStatement(stmt);
      currVars.merge(stmtVars);
    }
  }

  for (Identifier *dVars : declVars)
    currVars.removeVariable(dVars, USED_DEFINED);

  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsReduction(reductionCallStmt *stmt)
{
  usedVariables_t currVars;

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
        currVars.addVariable(stmt->getLeftId(), DEFINED);
        getVarsReductionCall(stmt->getReducCall());
    }
    else if(stmt->getLhsType() == 2)
    {
      PropAccess* propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), USED);
      currVars.addVariable(propId->getIdentifier2(), DEFINED);

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
          currVars.addVariable(iden, DEFINED);
          affectedId = iden;
        }
        else if(node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess* propId = (PropAccess*)node;
          currVars.addVariable(propId->getIdentifier1(), USED);
          currVars.addVariable(propId->getIdentifier2(), DEFINED);
          affectedId = propId->getIdentifier2();
        }

        if(affectedId->getSymbolInfo()->getId()->get_fp_association()) {
          Identifier* fpId = affectedId->getSymbolInfo()->getId()->get_fpIdNode();
          currVars.addVariable(fpId, DEFINED);
        }
      }
      getVarsReductionCall(stmt->getReducCall());
      
      for(ASTNode* node: stmt->getRightList())
      {
        if(node->getTypeofNode() == NODE_ID)
        {
          Identifier* iden = (Identifier*)node;
          currVars.addVariable(iden, DEFINED);
        }
        else if(node->getTypeofNode() == NODE_PROPACCESS)
        {
          PropAccess* propId = (PropAccess*)node;
          currVars.addVariable(propId->getIdentifier1(), USED);
          currVars.addVariable(propId->getIdentifier2(), DEFINED);
        }
      }
    }
  }
  else
  {
    if(stmt->isLeftIdentifier())
    {
      currVars.addVariable(stmt->getLeftId(), USED_DEFINED);
      currVars.merge(getVarsExpr(stmt->getRightSide()));
    }
    else
    {
      PropAccess* propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), USED);
      currVars.addVariable(propId->getIdentifier2(), USED_DEFINED);

      currVars.merge(getVarsExpr(stmt->getRightSide()));
    }
  }
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsProcCall(proc_callStmt *procCall)
{
  proc_callExpr *callExpr = procCall->getProcCallExpr();
  return getVarsExprProcCall(callExpr);
}

usedVariables_t blockVarsAnalyser::getVarsExprProcCall(proc_callExpr *procCall)
{
  usedVariables_t currVars;
  // if(procCall->getId1() != NULL)
  //   currVars.addVariable(procCall->getId1(), USED);
  for (argument *arg : procCall->getArgList())
  {
    if (arg->isExpr())
      currVars.merge(getVarsExpr(arg->getExpr()));
    else if (arg->isAssignExpr())
      currVars.merge(getVarsAssignment(arg->getAssignExpr()));
  }
  
  return currVars;
}

usedVariables_t blockVarsAnalyser::getVarsStatement(statement *stmt)
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

  return usedVariables_t();
}