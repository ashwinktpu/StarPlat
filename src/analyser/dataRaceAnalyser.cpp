#include "dataRaceAnalyser.h"
#include "../ast/ASTHelper.cpp"

enum codeStructure
{
    INVALID_STUCTURE,
    MIN_PROP_UPDATE,
    PROP_UPDATE
};

bool checkIdEqual(Identifier *id1, Identifier *id2)
{
    return (strcmp(id1->getIdentifier(), id2->getIdentifier()) == 0);
}

bool checkExprEqual(Expression *expr1, Expression *expr2)
{
    //to check whether two expressions are equal
    return true;
}

int getStructure(statement *stmt, Identifier *itr1, Identifier *itr2)
{
    if (stmt->getTypeofNode() == NODE_IFSTMT)
    {
        ifStmt *currStmt = (ifStmt *)stmt;
        Expression *cond = currStmt->getCondition();
        if ((currStmt->getElseBody() == nullptr) && cond->isRelational() && (cond->getOperatorType() == OPERATOR_GT))
        {
            Expression *lExpr = cond->getLeft();
            if (lExpr->isPropIdExpr() && checkIdEqual(lExpr->getPropId()->getIdentifier1(), id2))
            {
                blockStatement *ifBody = (blockStatement *)currStmt->getIfBody();
                statement *reqStmt = nullptr;
                int cntValid = 0;

                for (statement *stmt : ifBody->returnStatements())
                {
                    if (stmt->getTypeofNode() == NODE_ASSIGN)
                    {
                        assignment *assgn = (assignment *)stmt;
                        if (assgn->lhs_isProp())
                        {
                            PropAccess *propId = assgn->getPropId();
                            PropAccess *ifPropId = lExpr->getPropId();

                            if (checkIdEqual(ifPropId->getIdentifier1(), propId->getIdentifier1()) && checkIdEqual(ifPropId->getIdentifier2(), propId->getIdentifier2()))
                            {
                                if ((reqStmt == nullptr) && checkExprEqual(cond->getRight(), assgn->getExpr()))
                                {
                                    reqStmt = stmt;
                                    cntValid++;
                                }
                            }
                            else if (assgn->getExpr()->isLiteral())
                                cntValid++;
                        }
                    }
                }

                if (cntValid == (ifBody->returnStatements()).size())
                    return MIN_PROP_UPDATE;
            }
        }
    }
    return INVALID_STUCTURE;
}

void dataRaceAnalyser::analyseForAll(forallStmt *stmt)
{
    Identifier *itr = stmt->getIterator();
    Identifier *srcGraph = stmt->getSourceGraph();
    proc_callExpr *procCall = stmt->getExtractElementFunc();

    if (strcmp(procCall->getMethodId()->getIdentifier(), "nodes") == 0)
    {
        blockStatement *body = (blockStatement *)stmt->getBody();
        for (statement *cstmt : body->returnStatements())
        {
            if ((cstmt->getTypeofNode() == NODE_FORALLSTMT) &&
                !((forallStmt *)cstmt)->isForall())
            {
                forallStmt *forStmt = (forallStmt *)cstmt;

                Identifier *itr1 = forStmt->getIterator();
                Identifier *srcGraph1 = forStmt->getSourceGraph();
                proc_callExpr *procCall1 = forStmt->getExtractElementFunc();

                if (checkIdEqual(srcGraph, srcGraph1) && (strcmp(procCall1->getMethodId()->getIdentifier(), "neighbors") == 0))
                {
                    argument *firstArg = *(procCall1->getArgList().begin());
                    Expression *argVal = firstArg->getExpr();

                    if (argVal->isIdentifierExpr() &&
                        checkIdEqual(argVal->getId(), itr))
                    {
                        blockStatement *forBody = (blockStatement *)forStmt->getBody();
                        for (statement *istmt : forBody->returnStatements())
                        {
                            if (getStructure(istmt, itr, itr1) == MIN_PROP_UPDATE)
                            {
                                ifStmt *currStmt = (ifStmt *)istmt;
                                Expression *cond = currStmt->getCondition();

                                list<ASTNode *> leftList;
                                reductionCall *reductionCallNode;
                                list<ASTNode *> rightList;

                                leftList.push_back(cond->getLeft()->getPropId());

                                argument *a1 = new argument();
                                a1->setExpression(cond->getLeft());
                                a1->setExpressionFlag();

                                argument *a2 = new argument();
                                a2->setExpression(cond->getRight());
                                a2->setExpressionFlag();

                                list<argument *> argList;
                                argList.push_back(a1);
                                argList.push_back(a2);

                                reductionCallNode = Util::createNodeforReductionCall(REDUCE_MIN, argList);

                                for(statement* ifBodyStmt: currStmt->getIfBody())
                                {
                                    assignment *assgn = (assignment *)stmt;
                                    PropAccess *propId = assgn->getPropId();

                                    if(assgn->getExpr()->isLiteral())
                                    {
                                        leftList.push_back(propId);
                                        rightList.push_back(assgn->getExpr());
                                    }
                                }
                            
                                reductionCallStmt* modifiedStmt = Util::createNodeForReductionStmtList(leftList, reductionCallNode, rightList);
                            }
                        }
                    }
                }
            }
        }
    }
}

void dataRaceAnalyser::analyseStatement(statement *stmt)
{
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
    {
        blockStatement *blockStmt = (blockStatement *)stmt;
        for (statement *stmt : blockStmt)
            analyseStatement(stmt);
    }
    break;

    case NODE_WHILESTMT:
        analyseStatement(((whileStmt *)stmt)->getBody());
        break;

    case NODE_DOWHILESTMT:
        analyseStatement(((dowhileStmt *)stmt)->getBody());
        break;

    case NODE_FIXEDPTSTMT:
        analyseStatement(((fixedPointStmt *)stmt)->getBody());
        break;

    case NODE_IFSTMT:
    {
        statement *ifBody = ((ifStmt *)stmt)->getIfBody();
        statement *elseBody = ((ifStmt *)stmt)->getElseBody();
        if (ifBody != NULL)
            analyseStatement(ifBody);
        if (elseBody != NULL)
            analyseStatement(elseBody);
        break;
    }

    case NODE_ITRBFS:
    {
        analyseStatement(((iterateBFS *)stmt)->getBody());
        iterateReverseBFS *revBFS = ((iterateBFS *)stmt)->getRBFS();
        if (revBFS != NULL)
            analyseStatement(revBFS->getBody());
        break;
    }

    case NODE_FORALLSTMT:
    {
        forallStmt *currStmt = (forallStmt *)stmt;
        if (currStmt->isForall())
            analyseForAll(currStmt);
        else
            analyseStatement(currStmt->getBody());
    }
    break;
    }
}

void dataRaceAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseStatement(func->getBlockStatement());
    return;
}

void dataRaceAnalyser::analyse()
{
    list<Function *> funcList = frontEndContext.getFuncList();
    for (Function *func : funcList)
    {
        analyseFunc(func);
    }
}