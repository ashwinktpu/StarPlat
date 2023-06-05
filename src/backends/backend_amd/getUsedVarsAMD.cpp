#include "analyserUtil.cpp"

usedVariables getVarsStatementAMD(statement* stmt);
usedVariables getVarsExprAMD(Expression *expr)
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
            result = getVarsExprAMD(expr->getUnaryExpr());
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
        result = getVarsExprAMD(expr->getLeft());
        result.merge(getVarsExprAMD(expr->getRight()));
    }
    return result;
}


usedVariables getVarsWhileAMD(whileStmt *stmt)
{
  usedVariables currVars = getVarsExprAMD(stmt->getCondition());
  currVars.merge(getVarsStatementAMD(stmt->getBody()));

  return currVars;
}


usedVariables getVarsDoWhileAMD(dowhileStmt *stmt)
{
  usedVariables currVars = getVarsExprAMD(stmt->getCondition());
  currVars.merge(getVarsStatementAMD(stmt->getBody()));

  return currVars;
}

usedVariables getVarsAssignmentAMD(assignment *stmt)
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

  usedVariables exprVars = getVarsExprAMD(stmt->getExpr());
  currVars.merge(exprVars);
  return currVars;
}

usedVariables getVarsIfAMD(ifStmt *stmt)
{
  usedVariables currVars = getVarsExprAMD(stmt->getCondition());
  currVars.merge(getVarsStatementAMD(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(getVarsStatementAMD(stmt->getElseBody()));

  return currVars;
}
/*
usedVariables getVarsFixedPoint(fixedPointStmt *stmt)
{
  usedVariables currVars = getVarsExprAMD(stmt->getDependentProp());
  currVars.addVariable(stmt->getFixedPointId());
  currVars.merge(getVarsStatementAMD(stmt->getBody()));
  return currVars;
}
// TODO : Handle this atlast
*/

usedVariables getVarsReductionAMD(reductionCallStmt *stmt)
{
  usedVariables currVars;

  auto getVarsReductionCall = [&currVars](reductionCall* callExpr) -> void
  {
    for(argument* arg: callExpr->getargList()){
      if(arg->isExpr())
        currVars.merge(getVarsExprAMD(arg->getExpr()));
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
      currVars.merge(getVarsExprAMD(stmt->getRightSide()));
    }
    else
    {
      PropAccess* propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), READ);
      currVars.addVariable(propId->getIdentifier2(), READ_WRITE);

      currVars.merge(getVarsExprAMD(stmt->getRightSide()));
    }
  }
  return currVars;
}

usedVariables getVarsUnaryAMD(unary_stmt *stmt)
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
  usedVariables currVars = getVarsStatementAMD(stmt->getBody());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(RBFSstmt->getBFSFilter());
    currVars.merge(RBFSstmt->getBody());
  }

  return currVars;
}*/

usedVariables getVarsForAllAMD(forallStmt *stmt)
{
  usedVariables currVars = getVarsStatementAMD(stmt->getBody());
  currVars.removeVariable(stmt->getIterator(), READ_WRITE);

  if(stmt->isSourceProcCall())
  {
      proc_callExpr *expr = stmt->getExtractElementFunc();
        for(argument* arg: expr->getArgList()){
            if(arg->getExpr() != nullptr)
                currVars.merge(getVarsExprAMD(arg->getExpr()));
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
    currVars.merge(getVarsExprAMD(stmt->getfilterExpr()));

  return currVars;
}

usedVariables getVarsBlockAMD(blockStatement *blockStmt)
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
        usedVariables exprVars = getVarsExprAMD(decl->getExpressionAssigned());
        for (Identifier *dVars : declVars)
          exprVars.removeVariable(dVars, READ_WRITE);

        currVars.merge(exprVars);
      }
    }
    else
    {
      usedVariables stmtVars = getVarsStatementAMD(stmt);
      for (Identifier *dVars : declVars)
        stmtVars.removeVariable(dVars, READ_WRITE);

      currVars.merge(stmtVars);
    }
  }

  return currVars;
}
/* Function: getDeclaredPropertyVarsOfBlock
 * Return all the Property variables declared in a the block!
 * Used for cudaFree those variables
 * usedVariables may be a misnomer?! for this function.
 * --rajesh
 */
usedVariables getDeclaredPropertyVarsOfBlockAMD(blockStatement *blockStmt)
{
  list<statement *> stmtList = blockStmt->returnStatements();
  list<Identifier *> declVars;

  usedVariables currVars;
  for (statement *stmt : stmtList)
  {
    if (stmt->getTypeofNode() == NODE_DECL) {
      declaration *decl = (declaration *)stmt;
      if(decl->getType()->isPropNodeType()) {
        currVars.addVariable(decl->getdeclId(),READ_WRITE); //2nd arg may be not used by us
      }
    }
  }

  return currVars;
}

usedVariables getVarsStatementAMD(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    return getVarsBlockAMD((blockStatement *)stmt);

  case NODE_ASSIGN:
    return getVarsAssignmentAMD((assignment *)stmt);

  case NODE_UNARYSTMT:
    return getVarsUnaryAMD((unary_stmt *)stmt);

  case NODE_IFSTMT:
    return getVarsIfAMD((ifStmt *)stmt);

  case NODE_WHILESTMT:
      return getVarsWhileAMD((whileStmt *)stmt);

  case NODE_DOWHILESTMT:
      return getVarsDoWhileAMD((dowhileStmt *)stmt);

  case NODE_FORALLSTMT:
      return getVarsForAllAMD((forallStmt *) stmt);

  case NODE_REDUCTIONCALLSTMT:
    return getVarsReductionAMD((reductionCallStmt *)stmt);

    /*case NODE_ITRBFS:
      return getVarsBFS((iterateBFS *)stmt);

    case NODE_FIXEDPTSTMT:
      return getVarsFixedPoint((fixedPointStmt *)stmt);*/
  default:
    ; // added to fix warning!
  }

  return usedVariables();
}
