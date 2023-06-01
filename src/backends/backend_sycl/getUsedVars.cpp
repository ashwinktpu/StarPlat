#include "analyserUtil.cpp"

usedVariables getVariablesStatement(statement *stmt);
usedVariables getVariablesExpr(Expression *expr)
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
            result = getVariablesExpr(expr->getUnaryExpr());
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
        result = getVariablesExpr(expr->getLeft());
        result.merge(getVariablesExpr(expr->getRight()));
    }
    return result;
}

usedVariables getVariablesWhile(whileStmt *stmt)
{
    usedVariables currVars = getVariablesExpr(stmt->getCondition());
    currVars.merge(getVariablesStatement(stmt->getBody()));

    return currVars;
}

usedVariables getVariablesDoWhile(dowhileStmt *stmt)
{
    usedVariables currVars = getVariablesExpr(stmt->getCondition());
    currVars.merge(getVariablesStatement(stmt->getBody()));

    return currVars;
}

usedVariables getVariablesAssignment(assignment *stmt)
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

    usedVariables exprVars = getVariablesExpr(stmt->getExpr());
    currVars.merge(exprVars);
    return currVars;
}

usedVariables getVariablesIf(ifStmt *stmt)
{
    usedVariables currVars = getVariablesExpr(stmt->getCondition());
    currVars.merge(getVariablesStatement(stmt->getIfBody()));
    if (stmt->getElseBody() != NULL)
        currVars.merge(getVariablesStatement(stmt->getElseBody()));

    return currVars;
}
/*
// usedVariables getVariablesFixedPoint(fixedPointStmt *stmt)
// {
//   usedVariables currVars = getVariablesExpr(stmt->getDependentProp());
//   currVars.addVariable(stmt->getFixedPointId());
//   currVars.merge(getVariablesStatement(stmt->getBody()));
//   return currVars;
// }
// // TODO : Handle this atlast
// */

usedVariables getVariablesReduction(reductionCallStmt *stmt)
{
    usedVariables currVars;

    auto getVariablesReductionCall = [&currVars](reductionCall *callExpr) -> void
    {
        for (argument *arg : callExpr->getargList())
        {
            if (arg->isExpr())
                currVars.merge(getVariablesExpr(arg->getExpr()));
        }
    };

    if (stmt->is_reducCall())
    {
        if (stmt->getLhsType() == 1)
        {
            currVars.addVariable(stmt->getLeftId(), WRITE);
            getVariablesReductionCall(stmt->getReducCall());
        }
        else if (stmt->getLhsType() == 2)
        {
            PropAccess *propId = stmt->getPropAccess();
            currVars.addVariable(propId->getIdentifier1(), READ);
            currVars.addVariable(propId->getIdentifier2(), WRITE);

            getVariablesReductionCall(stmt->getReducCall());
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
            getVariablesReductionCall(stmt->getReducCall());

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
            currVars.merge(getVariablesExpr(stmt->getRightSide()));
        }
        else
        {
            PropAccess *propId = stmt->getPropAccess();
            currVars.addVariable(propId->getIdentifier1(), READ);
            currVars.addVariable(propId->getIdentifier2(), READ_WRITE);

            currVars.merge(getVariablesExpr(stmt->getRightSide()));
        }
    }
    return currVars;
}

usedVariables getVariablesUnary(unary_stmt *stmt)
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
// usedVariables getVariablesBFS(iterateBFS *stmt)
// {
//   usedVariables currVars = getVariablesStatement(stmt->getBody());
//   if (stmt->getRBFS() != nullptr)
//   {
//     iterateReverseBFS *RBFSstmt = stmt->getRBFS();
//     if (RBFSstmt->getBFSFilter() != nullptr)
//       currVars.merge(RBFSstmt->getBFSFilter());
//     currVars.merge(RBFSstmt->getBody());
//   }

//   return currVars;
// }*/

usedVariables getVariablesForAll(forallStmt *stmt)
{
    usedVariables currVars = getVariablesStatement(stmt->getBody());
    currVars.removeVariable(stmt->getIterator(), READ_WRITE);

    if (stmt->isSourceProcCall())
    {
        proc_callExpr *expr = stmt->getExtractElementFunc();
        for (argument *arg : expr->getArgList())
        {
            if (arg->getExpr() != nullptr)
                currVars.merge(getVariablesExpr(arg->getExpr()));
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
        currVars.merge(getVariablesExpr(stmt->getfilterExpr()));

    return currVars;
}

usedVariables getVariablesBlock(blockStatement *blockStmt)
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
                usedVariables exprVars = getVariablesExpr(decl->getExpressionAssigned());
                for (Identifier *dVars : declVars)
                    exprVars.removeVariable(dVars, READ_WRITE);

                currVars.merge(exprVars);
            }
        }
        else
        {
            usedVariables stmtVars = getVariablesStatement(stmt);
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
usedVariables getDeclaredPropertyVariablesOfBlock(blockStatement *blockStmt)
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

usedVariables getVariablesStatement(statement *stmt)
{
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
        return getVariablesBlock((blockStatement *)stmt);

    case NODE_ASSIGN:
        return getVariablesAssignment((assignment *)stmt);

    case NODE_UNARYSTMT:
        return getVariablesUnary((unary_stmt *)stmt);

    case NODE_IFSTMT:
        return getVariablesIf((ifStmt *)stmt);

    case NODE_WHILESTMT:
        return getVariablesWhile((whileStmt *)stmt);

    case NODE_DOWHILESTMT:
        return getVariablesDoWhile((dowhileStmt *)stmt);

    case NODE_FORALLSTMT:
        return getVariablesForAll((forallStmt *)stmt);

    case NODE_REDUCTIONCALLSTMT:
        return getVariablesReduction((reductionCallStmt *)stmt);

        /*case NODE_ITRBFS:
          return getVariablesBFS((iterateBFS *)stmt);

        case NODE_FIXEDPTSTMT:
          return getVariablesFixedPoint((fixedPointStmt *)stmt);*/
    default:; // added to fix warning!
    }

    return usedVariables();
}