#include "../backend_cuda/analyserUtil.cpp"

usedVariables GetVarsStatement(statement* stmt);
usedVariables GetVarsExpr(Expression *expr)
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
            result = GetVarsExpr(expr->getUnaryExpr());
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
        result = GetVarsExpr(expr->getLeft());
        result.merge(GetVarsExpr(expr->getRight()));
    }
    return result;
}


usedVariables GetVarsWhile(whileStmt *stmt)
{
  usedVariables currVars = GetVarsExpr(stmt->getCondition());
  currVars.merge(GetVarsStatement(stmt->getBody()));

  return currVars;
}


usedVariables GetVarsDoWhile(dowhileStmt *stmt)
{
  usedVariables currVars = GetVarsExpr(stmt->getCondition());
  currVars.merge(GetVarsStatement(stmt->getBody()));

  return currVars;
}

usedVariables GetVarsAssignment(assignment *stmt)
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

  usedVariables exprVars = GetVarsExpr(stmt->getExpr());
  currVars.merge(exprVars);
  return currVars;
}

usedVariables GetVarsIf(ifStmt *stmt)
{
  usedVariables currVars = GetVarsExpr(stmt->getCondition());
  currVars.merge(GetVarsStatement(stmt->getIfBody()));
  if (stmt->getElseBody() != NULL)
    currVars.merge(GetVarsStatement(stmt->getElseBody()));

  return currVars;
}

usedVariables GetVarsFixedPoint(fixedPointStmt *stmt)
{
  usedVariables currVars = GetVarsExpr(stmt->getDependentProp());
  currVars.addVariable(stmt->getFixedPointId(), READ);
  currVars.merge(GetVarsStatement(stmt->getBody()));
  return currVars;
}
// TODO : Handle this atlast

usedVariables GetVarsReduction(reductionCallStmt *stmt)
{
  usedVariables currVars;

  auto getVarsReductionCall = [&currVars](reductionCall* callExpr) -> void
  {
    for(argument* arg: callExpr->getargList()){
      if(arg->isExpr())
        currVars.merge(GetVarsExpr(arg->getExpr()));
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
      currVars.merge(GetVarsExpr(stmt->getRightSide()));
    }
    else
    {
      PropAccess* propId = stmt->getPropAccess();
      currVars.addVariable(propId->getIdentifier1(), READ);
      currVars.addVariable(propId->getIdentifier2(), READ_WRITE);

      currVars.merge(GetVarsExpr(stmt->getRightSide()));
    }
  }
  return currVars;
}

usedVariables GetVarsUnary(unary_stmt *stmt)
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


usedVariables getVarsBFS(iterateBFS *stmt)
{
  usedVariables currVars = GetVarsStatement(stmt->getBody());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(GetVarsExpr(RBFSstmt->getBFSFilter()));
    currVars.merge(GetVarsStatement(RBFSstmt->getBody()));
  }

  return currVars;
}

usedVariables getVarsBFS(iterateBFS2 *stmt)
{
  usedVariables currVars = GetVarsStatement(stmt->getBody());
  if (stmt->getRBFS() != nullptr)
  {
    iterateReverseBFS *RBFSstmt = stmt->getRBFS();
    if (RBFSstmt->getBFSFilter() != nullptr)
      currVars.merge(GetVarsExpr(RBFSstmt->getBFSFilter()));
    currVars.merge(GetVarsStatement(RBFSstmt->getBody()));
  }

  return currVars;
}

usedVariables GetVarsForAll(forallStmt *stmt)
{
  usedVariables currVars = GetVarsStatement(stmt->getBody());
  currVars.removeVariable(stmt->getIterator(), READ_WRITE);

  if(stmt->isSourceProcCall())
  {
      proc_callExpr *expr = stmt->getExtractElementFunc();
        for(argument* arg: expr->getArgList()){
            if(arg->getExpr() != nullptr)
                currVars.merge(GetVarsExpr(arg->getExpr()));
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
    currVars.merge(GetVarsExpr(stmt->getfilterExpr()));

  return currVars;
}

usedVariables GetVarsBlock(blockStatement *blockStmt)
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
        usedVariables exprVars = GetVarsExpr(decl->getExpressionAssigned());
        for (Identifier *dVars : declVars)
          exprVars.removeVariable(dVars, READ_WRITE);

        currVars.merge(exprVars);
      }
    }
    else
    {
      usedVariables stmtVars = GetVarsStatement(stmt);
      for (Identifier *dVars : declVars)
        stmtVars.removeVariable(dVars, READ_WRITE);

      currVars.merge(stmtVars);
    }
  }

  return currVars;
}
/* Function: getDeclaredPropertyVarsOfBlock
 * Return all the Property variables declared in a the block!
 * Used for hipFree those variables
 * usedVariables may be a misnomer?! for this function.
 * --rajesh
 */
usedVariables GetDeclaredPropertyVarsOfBlock(blockStatement *blockStmt)
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

usedVariables GetVarsStatement(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    return GetVarsBlock((blockStatement *)stmt);

  case NODE_ASSIGN:
    return GetVarsAssignment((assignment *)stmt);

  case NODE_UNARYSTMT:
    return GetVarsUnary((unary_stmt *)stmt);

  case NODE_IFSTMT:
    return GetVarsIf((ifStmt *)stmt);

  case NODE_WHILESTMT:
      return GetVarsWhile((whileStmt *)stmt);

  case NODE_DOWHILESTMT:
      return GetVarsDoWhile((dowhileStmt *)stmt);

  case NODE_FORALLSTMT:
      return GetVarsForAll((forallStmt *) stmt);

  case NODE_REDUCTIONCALLSTMT:
    return GetVarsReduction((reductionCallStmt *)stmt);

    /*case NODE_ITRBFS:
      return getVarsBFS((iterateBFS *)stmt);*/

  case NODE_FIXEDPTSTMT:
      return GetVarsFixedPoint((fixedPointStmt *)stmt);
  default:
    ; // added to fix warning!
  }

  return usedVariables();
}
