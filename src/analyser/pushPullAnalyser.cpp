#include "PPAnalyser.h"
#include "../ast/ASTHelper.cpp"
#include "../symbolutil/SymbolTable.h"
#include "analyserUtil.hpp"

bool checkIdEqual(Identifier *id1, Identifier *id2)
{
    return (strcmp(id1->getIdentifier(), id2->getIdentifier()) == 0);
}

bool checkIdNameEqual(Identifier *id1, char *c)
{
    return (strcmp(id1->getIdentifier(), c) == 0);
}

void PPAnalyser::checkSSSPUpdate(statement *stmt, Identifier *v, Identifier *nbr, Identifier *modified)
{
    switch(stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:
        {
            for (statement *bstmt: blockStmt->returnStatements())
            {
                switch(bstmt->getTypeofNode())
                {
                    case NODE_DECL:
                    {
                        declaration *declStmt = (declaration *)bstmt;
                        Type *declType = declStmt->getType();
                        if (declStmt->isInitialized() && declType->gettypeId() == TYPE_EDGE)
                        {
                            Identifier *declId = declStmt->getdeclId();
                            Expression *declExpr = declStmt->getExpressionAssigned();
                            if (declExpr->getType() == EXPR_PROCCALL)
                            {
                                proc_callExpr *procExpr = (proc_callExpr *)declExpr;
                                if (checkIdEqual(procExpr->getId1()->getIdentifier(), srcGraph) && procExpr->getArgList().size() == 2)
                                {
                                    cout << "edge ok" << endl;
                                }
                            }
                        }
                    }
                    case NODE_REDUCTIONCALLSTMT:
                    
                }
            }
        }
    }
}

void PPAnalyser::analyseForallIn(statement *stmt, Identifier *srcGraph, Identifier *faId, Identifier *uId)
{
    forallStmt *inFstmt = (forallStmt *)stmt;
    if (!inFstmt->hasFilterExpr())
    {
        Identifier *inFaId = inFstmt->getIterator(), *inSrcGraph = inFstmt->getSourceGraph();
        if (checkIdNameEqual(inFstmt->getExtractElementFunc()->getMethodId(), "neighbors")
            && inFstmt->checkIdEqual(inSrcGraph, srcGraph))
        {
            proc_callExpr *extractElemFunc = inFstmt->getExtractElemFunc();
            if (extractElemFunc->getArgList().size() == 1)
            {
                checkSSSPUpdate(inFstmt->getBody(), faId, inFaId, uId);
            }
        }
    }
}

void PPAnalyser::analyseForallOut(statement *stmt, Identifier *uId)
{
    forallStmt *fstmt = (forallStmt *)stmt;
    Identifier *faId = fstmt->getIterator();
    Identifier *srcGraph = fstmt->getSourceGraph();
    if (fstmt->isForall() && checkIdNameEqual(fstmt->getExtractElemFunc()->getMethodId(), "nodes") && fstmt->hasFilterExpr())
    {
        Expression *filterExpr = fstmt->getFilterExpr();
        if (filterExpr->getOperatorType() == OPERATOR_EQ)
        {
            if (filterExpr->getLeft()->getType() == EXPR_PROPID)
            {
                PropAccess *filterLeft = (PropAccess *)filterExpr->getLeft();
                if (checkIdEqual(faId, filterLeft->getIdentifier1() && checkIdEqual(filterLeft->getIdentifier2(), uId)))
                {
                    if (filterExpr->getRight()->getType() == EXPR_BOOLCONSTANT)
                    {
                        statement *body = fstmt->getBody();
                        switch(body->getTypeofNode())
                        {
                            case NODE_BLOCKSTMT:
                            case NODE_FORALLSTMT:
                                analyseForallIn(body, srcGraph);
                        }
                    }
                }
            }
        }
    }
}

void PPAnalyser::analyseFPLoop(statement *stmt)
{
    fixedPointStmt *fpStmt = (fixedPointStmt *)stmt;

    statement *body = fpStmt->getBody();
    Expression *expr = fpStmt->getDependentProp();
    Identifier *fpId = fpStmt->getFixedPointId();
    if (expr->isUnary())
    {
        Identifier *uId = expr->getUnaryExpr()->getIdentifier();
        if (expr->getOperatorType() == OPERATOR_NOT)
        {
            switch (stmt->getTypeofNode())
            {
                case NODE_BLOCKSTMT:
                case NODE_FORALLSTMT:
                    analyseForallOut(stmt, uId);
            }
        }
    }
}

void PPAnalyser::analyseBlock(statement* stmt)
{
    blockStatement *blockStmt = (blockStatement *)stmt;
    for (statement *stmt: blockStmt->returnStatements())
    {
        switch (stmt->getTypeofNode())
        {
            case NODE_BLOCKSTMT:
            {
                analyseBlock(stmt);
                break;
            }
            case NODE_FORALLSTMT:
            {
            }
            case NODE_DOWHILESTMT:
            {
            }
            case NODE_WHILESTMT:
            {
            }
            case NODE_FIXEDPTSTMT:
            {
                analyseFPLoop(stmt);
                break;
            }
        }
    }
}

void PPAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseBlock(func->getBlockStatement());
    return;
}

void PPAnalyser::analyse()
{
    list<Function *> funcList = frontEndContext.getFuncList();
    for (Function *func : funcList)
        analyseFunc(func);
}