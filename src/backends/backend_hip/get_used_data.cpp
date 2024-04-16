/**
 * @file
 * @brief
 *
 * @author cs22m056
 */

#include "dsl_cpp_generator.h"

namespace sphip {

usedVariables DslCppGenerator::GetUsedVariablesInStatement(statement *statement) {
    switch (statement->getTypeofNode()) {
        case NODE_WHILESTMT:
            return GetUsedVariablesInWhile(static_cast<whileStmt *>(statement));

        case NODE_DOWHILESTMT:
            return GetUsedVariablesInDoWhile(static_cast<dowhileStmt *>(statement));

        case NODE_ASSIGN:
            return GetUsedVariablesInAssignment(static_cast<assignment *>(statement));

        case NODE_IFSTMT:
            return GetUsedVariablesInIf(static_cast<ifStmt *>(statement));

        case NODE_FIXEDPTSTMT:
            return GetUsedVariablesInFixedPoint(static_cast<fixedPointStmt *>(statement));

        case NODE_REDUCTIONCALLSTMT:
            return GetUsedVariablesInReductionCall(static_cast<reductionCallStmt *>(statement));

        case NODE_UNARYSTMT:
            return GetUsedVariablesInUnaryStmt(static_cast<unary_stmt *>(statement));

        case NODE_FORALLSTMT:
            return GetUsedVariablesInForAll(static_cast<forallStmt *>(statement));

        case NODE_BLOCKSTMT:
            return GetUsedVariablesInBlock(static_cast<blockStatement *>(statement));

        default:
            return usedVariables();
    }
}

usedVariables DslCppGenerator::GetUsedVariablesInExpression(Expression *expr) {
    usedVariables result;

    if (expr->isIdentifierExpr()) {
        Identifier *iden = expr->getId();
        result.addVariable(iden, READ);

    } else if (expr->isPropIdExpr()) {
        PropAccess *propExpr = expr->getPropId();
        result.addVariable(propExpr->getIdentifier1(), READ);
        result.addVariable(propExpr->getIdentifier2(), READ);

    } else if (expr->isUnary()) {
        if (expr->getOperatorType() == OPERATOR_NOT)
            result = GetUsedVariablesInExpression(expr->getUnaryExpr());

        else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC)) {
            Expression *uExpr = expr->getUnaryExpr();

            if (uExpr->isIdentifierExpr())
                result.addVariable(uExpr->getId(), READ_WRITE);

            else if (uExpr->isPropIdExpr()) {
                PropAccess *propId = uExpr->getPropId();
                result.addVariable(propId->getIdentifier1(), READ);
                result.addVariable(propId->getIdentifier2(), READ_WRITE);
            }
        }

    } else if (expr->isLogical() || expr->isArithmetic() || expr->isRelational()) {
        result = GetUsedVariablesInExpression(expr->getLeft());
        result.merge(GetUsedVariablesInExpression(expr->getRight()));
    }

    return result;
}

usedVariables DslCppGenerator::GetUsedVariablesInWhile(whileStmt *whileStmt) {
    usedVariables currVars = GetUsedVariablesInExpression(whileStmt->getCondition());
    currVars.merge(GetUsedVariablesInStatement(whileStmt->getBody()));

    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInDoWhile(dowhileStmt *dowhileStmt) {
    usedVariables currVars = GetUsedVariablesInExpression(dowhileStmt->getCondition());
    currVars.merge(GetUsedVariablesInStatement(dowhileStmt->getBody()));

    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInAssignment(assignment *stmt) {
    usedVariables currVars;
    if (stmt->lhs_isProp()) {
        PropAccess *propId = stmt->getPropId();
        currVars.addVariable(propId->getIdentifier1(), READ);
        currVars.addVariable(propId->getIdentifier2(), WRITE);
    } else if (stmt->lhs_isIdentifier())
        currVars.addVariable(stmt->getId(), WRITE);

    usedVariables exprVars = GetUsedVariablesInExpression(stmt->getExpr());
    currVars.merge(exprVars);
    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInIf(ifStmt *ifStmt) {
    usedVariables currVars = GetUsedVariablesInExpression(ifStmt->getCondition());
    currVars.merge(GetUsedVariablesInStatement(ifStmt->getIfBody()));
    if (ifStmt->getElseBody() != NULL)
        currVars.merge(GetUsedVariablesInStatement(ifStmt->getElseBody()));

    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInFixedPoint(fixedPointStmt *fixedPt) {
    usedVariables currVars = GetUsedVariablesInExpression(fixedPt->getDependentProp());
    currVars.addVariable(fixedPt->getFixedPointId(), READ);
    currVars.merge(GetUsedVariablesInStatement(fixedPt->getBody()));
    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInReductionCall(reductionCallStmt *stmt) {
    usedVariables currVars;

    auto getVarsReductionCall = [&currVars, this](reductionCall *callExpr) -> void {
        for (argument *arg : callExpr->getargList()) {
            if (arg->isExpr())
                currVars.merge(this->GetUsedVariablesInExpression(arg->getExpr()));
        }
    };

    if (stmt->is_reducCall()) {
        if (stmt->getLhsType() == 1) {
            currVars.addVariable(stmt->getLeftId(), WRITE);
            getVarsReductionCall(stmt->getReducCall());
        } else if (stmt->getLhsType() == 2) {
            PropAccess *propId = stmt->getPropAccess();
            currVars.addVariable(propId->getIdentifier1(), READ);
            currVars.addVariable(propId->getIdentifier2(), WRITE);

            getVarsReductionCall(stmt->getReducCall());
        } else if (stmt->getLhsType() == 3) {
            for (ASTNode *node : stmt->getLeftList()) {
                Identifier *affectedId = NULL;
                if (node->getTypeofNode() == NODE_ID) {
                    Identifier *iden = (Identifier *)node;
                    currVars.addVariable(iden, WRITE);
                    affectedId = iden;
                } else if (node->getTypeofNode() == NODE_PROPACCESS) {
                    PropAccess *propId = (PropAccess *)node;
                    currVars.addVariable(propId->getIdentifier1(), READ);
                    currVars.addVariable(propId->getIdentifier2(), WRITE);
                    affectedId = propId->getIdentifier2();
                }

                if (affectedId->getSymbolInfo()->getId()->get_fp_association()) {
                    Identifier *fpId = affectedId->getSymbolInfo()->getId()->get_fpIdNode();
                    currVars.addVariable(fpId, WRITE);
                }
            }
            getVarsReductionCall(stmt->getReducCall());

            for (ASTNode *node : stmt->getRightList()) {
                if (node->getTypeofNode() == NODE_ID) {
                    Identifier *iden = (Identifier *)node;
                    currVars.addVariable(iden, WRITE);
                } else if (node->getTypeofNode() == NODE_PROPACCESS) {
                    PropAccess *propId = (PropAccess *)node;
                    currVars.addVariable(propId->getIdentifier1(), READ);
                    currVars.addVariable(propId->getIdentifier2(), WRITE);
                }
            }
        }
    } else {
        if (stmt->isLeftIdentifier()) {
            currVars.addVariable(stmt->getLeftId(), READ_WRITE);
            currVars.merge(GetUsedVariablesInExpression(stmt->getRightSide()));
        } else {
            PropAccess *propId = stmt->getPropAccess();
            currVars.addVariable(propId->getIdentifier1(), READ);
            currVars.addVariable(propId->getIdentifier2(), READ_WRITE);

            currVars.merge(GetUsedVariablesInExpression(stmt->getRightSide()));
        }
    }
    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInUnaryStmt(unary_stmt *stmt) {
    Expression *unaryExpr = stmt->getUnaryExpr();
    Expression *varExpr = unaryExpr->getUnaryExpr();

    usedVariables currUsed;
    if (varExpr->isIdentifierExpr())
        currUsed.addVariable(varExpr->getId(), READ_WRITE);
    else if (varExpr->isPropIdExpr()) {
        currUsed.addVariable(varExpr->getPropId()->getIdentifier1(), READ);
        currUsed.addVariable(varExpr->getPropId()->getIdentifier2(), WRITE);
    }

    return currUsed;
}

usedVariables DslCppGenerator::GetUsedVariablesInForAll(forallStmt *forAllStmt) {
    usedVariables currVars = GetUsedVariablesInStatement(forAllStmt->getBody());
    currVars.removeVariable(forAllStmt->getIterator(), READ_WRITE);

    if (forAllStmt->isSourceProcCall()) {
        proc_callExpr *expr = forAllStmt->getExtractElementFunc();

        for (argument *arg : expr->getArgList()) {
            if (arg->getExpr() != nullptr)
                currVars.merge(GetUsedVariablesInExpression(arg->getExpr()));
        }
    } else if (!forAllStmt->isSourceField()) {
        Identifier *iden = forAllStmt->getSource();
        currVars.addVariable(iden, READ);

    } else {
        PropAccess *propId = forAllStmt->getPropSource();
        currVars.addVariable(propId->getIdentifier1(), READ);
    }

    if (forAllStmt->hasFilterExpr())
        currVars.merge(GetUsedVariablesInExpression(forAllStmt->getfilterExpr()));

    return currVars;
}

usedVariables DslCppGenerator::GetUsedVariablesInBlock(blockStatement *block) {
    list<statement *> stmtList = block->returnStatements();
    list<Identifier *> declVars;

    usedVariables currVars;
    for (statement *stmt : stmtList) {
        if (stmt->getTypeofNode() == NODE_DECL) {
            declaration *decl = (declaration *)stmt;
            declVars.push_back(decl->getdeclId());

            if (decl->isInitialized()) {
                usedVariables exprVars = GetUsedVariablesInExpression(decl->getExpressionAssigned());
                for (Identifier *dVars : declVars)
                    exprVars.removeVariable(dVars, READ_WRITE);

                currVars.merge(exprVars);
            }
        } else {
            usedVariables stmtVars = GetUsedVariablesInStatement(stmt);
            for (Identifier *dVars : declVars)
                stmtVars.removeVariable(dVars, READ_WRITE);

            currVars.merge(stmtVars);
        }
    }

    return currVars;
}

usedVariables DslCppGenerator::GetDeclaredPropertyVariablesOfBlock(blockStatement *block) {
    list<statement *> stmtList = block->returnStatements();
    list<Identifier *> declVars;

    usedVariables currVars;

    for (statement *stmt : stmtList) {
        cout << "STMT IN SOMEWHERE " << stmt->getTypeofNode() << "\n";
        if (stmt->getTypeofNode() == NODE_DECL) {
            declaration *decl = (declaration *)stmt;

            if (decl->getType()->isPropNodeType())
                currVars.addVariable(decl->getdeclId(), READ_WRITE);
        }
    }

    return currVars;
}

};  // namespace sphip