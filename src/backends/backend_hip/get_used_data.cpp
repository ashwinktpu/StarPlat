/**
 * @file
 * @brief
 *
 * @author cs22m056
 */

#include "dsl_cpp_generator.h"

namespace sphip {

    UsedVariables DslCppGenerator::GetUsedVariablesInStatement(statement *statement) {

        switch (statement->getTypeofNode()) {

        case NODE_WHILESTMT:
            return GetUsedVariablesInWhile(static_cast<whileStmt*>(statement));

        case NODE_DOWHILESTMT:
            return GetUsedVariablesInDoWhile(static_cast<dowhileStmt*>(statement));

        case NODE_ASSIGN:
            return GetUsedVariablesInAssignment(static_cast<assignment*>(statement));

        case NODE_IFSTMT:
            return GetUsedVariablesInIf(static_cast<ifStmt*>(statement));

        case NODE_FIXEDPTSTMT:
            return GetUsedVariablesInFixedPoint(static_cast<fixedPointStmt*>(statement));

        case NODE_REDUCTIONCALLSTMT:
            return GetUsedVariablesInReductionCall(static_cast<reductionCallStmt*>(statement));

        case NODE_UNARYSTMT:
            return GetUsedVariablesInUnaryStmt(static_cast<unary_stmt*>(statement));

        case NODE_FORALLSTMT:
            return GetUsedVariablesInForAll(static_cast<forallStmt*>(statement));

        case NODE_BLOCKSTMT:
            return GetUsedVariablesInBlock(static_cast<blockStatement*>(statement));

        default:
            return UsedVariables();
        }
    }

    UsedVariables DslCppGenerator::GetUsedVariablesInExpression(Expression *expr) {

        UsedVariables result;

        if (expr->isIdentifierExpr()) {

            Identifier *iden = expr->getId();
            result.AddVariable(iden, READ);

        } else if (expr->isPropIdExpr()) {

            PropAccess *propExpr = expr->getPropId();
            result.AddVariable(propExpr->getIdentifier1(), READ);
            result.AddVariable(propExpr->getIdentifier2(), READ);

        } else if (expr->isUnary()) 
        {
            if (expr->getOperatorType() == OPERATOR_NOT)
                result = GetUsedVariablesInExpression(expr->getUnaryExpr());

            else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC)) {

                Expression *uExpr = expr->getUnaryExpr();

                if (uExpr->isIdentifierExpr())
                    result.AddVariable(uExpr->getId(), READ_WRITE);

                else if (uExpr->isPropIdExpr()) {

                    PropAccess* propId = uExpr->getPropId();
                    result.AddVariable(propId->getIdentifier1(), READ);
                    result.AddVariable(propId->getIdentifier2(), READ_WRITE);
                }
            }

        } else if (expr->isLogical() || expr->isArithmetic() || expr->isRelational()) {

            result = GetUsedVariablesInExpression(expr->getLeft());
            result.Merge(GetUsedVariablesInExpression(expr->getRight()));
        }

        return result;
    }

    UsedVariables DslCppGenerator::GetUsedVariablesInWhile(whileStmt *whileStmt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInDoWhile(dowhileStmt *dowhileStmt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInAssignment(assignment *stmt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInIf(ifStmt *ifStmt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInFixedPoint(fixedPointStmt *fixedPt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInReductionCall(reductionCallStmt *stmt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInUnaryStmt(unary_stmt *stmt) {

    }

    UsedVariables DslCppGenerator::GetUsedVariablesInForAll(forallStmt *forAllStmt) {

        UsedVariables currVars = GetUsedVariablesInStatement(forAllStmt->getBody());
        currVars.RemoveVariable(forAllStmt->getIterator(), READ_WRITE);

        if(forAllStmt->isSourceProcCall()) {

            proc_callExpr *expr = forAllStmt->getExtractElementFunc();

            for(argument* arg: expr->getArgList()) {
                
                if(arg->getExpr() != nullptr)
                    currVars.Merge(GetUsedVariablesInExpression(arg->getExpr()));
            }
        } else if(!forAllStmt->isSourceField()) {

            Identifier *iden = forAllStmt->getSource();
            currVars.AddVariable(iden, READ);

        } else {

            PropAccess *propId = forAllStmt->getPropSource();
            currVars.AddVariable(propId->getIdentifier1(), READ);
        }

        if (forAllStmt->hasFilterExpr())
            currVars.Merge(GetUsedVariablesInExpression(forAllStmt->getfilterExpr()));

        return currVars;
    }

    UsedVariables DslCppGenerator::GetUsedVariablesInBlock(blockStatement *block) {

    }

    UsedVariables DslCppGenerator::GetDeclaredPropertyVariablesOfBlock(blockStatement *block) {

        list<statement *> stmtList = block->returnStatements();
        list<Identifier *> declVars;

        UsedVariables currVars; 

        for (statement *stmt : stmtList) {

            if (stmt->getTypeofNode() == NODE_DECL) {

                declaration *decl = (declaration *)stmt;

                if(decl->getType()->isPropNodeType()) 
                    currVars.AddVariable(decl->getdeclId(), READ_WRITE);
            }
        }

        return currVars;
    }

};