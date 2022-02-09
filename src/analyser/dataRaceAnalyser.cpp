#include "dataRaceAnalyser.h"
#include "../ast/ASTHelper.cpp"

enum STRUCTURE_TYPE
{
    INVALID_STUCTURE,
    MIN_PROP_UPDATE,
    PROP_UPDATE
};

bool checkIdEqual(Identifier *id1, Identifier *id2)
{
    return (strcmp(id1->getIdentifier(), id2->getIdentifier()) == 0);
}

bool checkPropIdEqual(PropAccess* prop1, PropAccess* prop2)
{
    return checkIdEqual(prop1->getIdentifier1(), prop2->getIdentifier1())
            && checkIdEqual(prop2->getIdentifier2(), prop2->getIdentifier2());
}

bool checkIdNameEqual(Identifier *id1, char *c)
{
    return (strcmp(id1->getIdentifier(), c) == 0);
}

bool checkExprEqual(Expression *expr1, Expression *expr2)
{
    if(expr1->getTypeofExpr() == expr2->getTypeofExpr())
    {
        switch (expr1->getTypeofExpr())
        {
        case /* constant-expression */:
            /* code */
            break;
        
        default:
            break;
        }
    }
    return false;
}

/*
class structure
{
    public:
    STRUCTURE_TYPE type;
    structure(STRUCTURE_TYPE type){
        this->type = type;
    }
};

class forAllStruct : structure
{
    public:
    forallStmt* forAllStmt;
    Identifier* iterator;

    forAllStruct(forallStmt* stmt)
    {
        this->forAllStmt = stmt;
        this->iterator = stmt->getIterator();
    }

    forAllStruct(forAllStruct* struct1)
    {
        this->forAllStmt = struct1->forAllStmt;
        this->iterator = struct1->iterator;
        this->type = struct1->type;
    }
};

class ngbrLoopStruct: public forAllStruct
{
    public:
    forallStmt* forStmt;
    Identifier* iterator;

    ngbrLoopStruct(forallStmt* stmt, forAllStruct* parStruct){

    }
};

class relPropUpdateStruct: public neighbourLoop
{
    public:
    ifStmt* stmt;
    PropAccess* propertyAcess;
    Expression* compareExpr;
    OPERATOR type;
    
    list<statement*> otherStmts;
};*/

ASTNode* dataRaceAnalyser::relPropUpdateAnalysis(ifStmt *stmt, Identifier *forIterator)
{
    Expression *cond = stmt->getCondition();
    if ((currStmt->getElseBody() == nullptr) && cond->isRelational())
        if ((cond->getOperatorType() == OPERATOR_GT) || (cond->getOperatorType() == OPERATOR_LT))
        {
            Expression *propExpr = nullptr;
            Expression *otherExpr = nullptr;
            OPERATOR opType = cond->getOperatorType();

            Expression *lExpr = propExpr->getLeft(), *rExpr = propExpr->getRight();

            if (lExpr->isPropIdExpr() && checkIdEqual(lExpr->getPropId()->getIdentifier1(), forIterator))
            {
                propExpr = lExpr;
                otherExpr = rExpr;
            }
            else if (rExpr->isPropIdExpr() && checkIdEqual(rExpr->getPropId()->getIdentifier1(), forIterator))
            {
                propExpr = rExpr;
                otherExpr = lExpr;

                if(opType == OPERATOR_LT) opType = OPERATOR_GT;
                else opType = OPERATOR_LT;
            }

            if (propExpr != nullptr)
            {
                blockStatement *ifBody = (blockStatement *)stmt->getIfBody();

                list<ASTNode *> leftList;
                reductionCall *reductionCallNode = nullptr;
                list<ASTNode *> rightList;

                leftList.push_back(propExpr->getPropId());

                argument *a1 = new argument();
                a1->setExpression(propExpr);
                a1->setExpressionFlag();

                argument *a2 = new argument();
                a2->setExpression(otherExpr);
                a2->setExpressionFlag();

                list<argument *> argList;
                argList.push_back(a1);
                argList.push_back(a2);

                for (statement *stmt : ifBody->returnStatements())
                {
                    if (stmt->getTypeofNode() == NODE_ASSIGN)
                    {
                        assignment *assgn = (assignment *)stmt;
                        if (assgn->lhs_isProp())
                        {
                            PropAccess *propId = assgn->getPropId();
                            PropAccess *ifPropId = propExpr->getPropId();

                            if (checkPropIdEqual(ifPropId, propId))
                            {
                                if ((reductionCallNode == nullptr) && checkExprEqual(otherExpr, assgn->getExpr()))
                                {
                                    if(opType == OPERATOR_GT)
                                        reductionCallNode = Util::createNodeforReductionCall(REDUCE_MIN, argList);
                                    else
                                        reductionCallNode = Util::createNodeforReductionCall(REDUCE_MAX, argList);
                                }
                            }
                            else if (assgn->getExpr()->isLiteral())
                            {
                                leftList.push_back(assgn->getPropId());
                                rightList.push_back(assgn->getExpr());
                            }
                        }
                        else if (assgn->getExpr()->isLiteral())
                        {
                            leftList.push_back(assgn->getId());
                            rightList.push_back(assgn->getExpr());
                        }
                    }
                }

                if (leftList.size() == (ifBody->returnStatements()).size())
                {
                    if(rightList.size() == 0)
                    {
                        reductionCallStmt* reductionCall = Util::createNodeForReductionStmt(propExpr->getPropId(), reductionCallNode);
                        return reductionCall;
                    }
                    else
                    {
                        reductionCallStmt *reductionCall = Util::createNodeForReductionStmtList(leftList, reductionCallNode, rightList);
                        return reductionCall;
                    }
                }
            }
        }
    
    return stmt;
}

ASTNode* dataRaceAnalyser::ngbrForAnalysis(forallStmt *stmt, Identifier *forAllIterator, Identifier *srcGraph)
{
    proc_callExpr *procCall = stmt->getExtractElementFunc();

    if (stmt->isSourceProcCall() && (procCall->getArgList().size() == 1) && checkIdEqual(srcGraph, stmt->getSourceGraph()) && checkIdNameEqual(procCall->getMethodId(), "neighbors"))
    {
        Identifier *ngbrItr = stmt->getIterator();
        argument *firstArg = *(procCall1->getArgList().begin());
        Expression *argVal = firstArg->getExpr();

        if (argVal->isIdentifierExpr() && checkIdEqual(argVal->getId(), forAllIterator))
        {
            statement *body = stmt->getBody();
            if (body->getTypeofNode() == NODE_BLOCKSTMT)
            {
                blockStatement *body = (blockStatement *)body;
                for (statement *stmt : body->returnStatements())
                {
                    if (stmt->getTypeofNode() == NODE_IFSTMT)
                    {
                        ASTNode *newStmt = relPropUpdateAnalysis((ifStmt *)stmt, Identifier * ngbrItr);
                    }
                }
            }
            else if (body->getTypeofNode() == NODE_IFSTMT)
            {
                ASTNode *newStmt = relPropUpdateAnalysis((ifStmt *)stmt, Identifier * ngbrItr);
                stmt->setBody(newStmt);
            }
        }
    }
    return nullptr;
}

ASTNode* dataRaceAnalyser::forAllAnalysis(forallStmt *stmt)
{
    proc_callExpr *procCall = stmt->getExtractElementFunc();
    if (stmt->isForall() && checkIdNameEqual(procCall->getMethodId(), "node"))
    {
        Identifier *itr = stmt->getIterator();
        Identifier *sourceGraph = stmt->getSourceGraph();

        statement *body = stmt->getBody();
        if (body->getTypeofNode() == NODE_BLOCKSTMT)
        {
            blockStatement *body = (blockStatement *)body;
            for (statement *stmt : body->returnStatements())
            {
                if (stmt->getTypeofNode() == NODE_FORALLSTMT)
                    ngbrForAnalysis((forallStmt * stmt), itr, sourceGraph);
            }
        }
        else if (body->getTypeofNode() == NODE_FORALLSTMT)
            ngbrForAnalysis((forallStmt * stmt), itr, sourceGraph);
    }
    return nullptr;
}

/*
void dataRaceAnalyser::analyseForAll(forallStmt *stmt)
{
    Identifier *itr = stmt->getIterator();
    Identifier *srcGraph = stmt->getSourceGraph();
    proc_callExpr *procCall = stmt->getExtractElementFunc();

    if (checkIdEqual(procCall->getMethodId(), "nodes"))
    {
        blockStatement *body = (blockStatement *)stmt->getBody();
        for (statement *cstmt : body->returnStatements())
        {
            if (cstmt->getTypeofNode() == NODE_FORALLSTMT)
            {
                forallStmt *forStmt = (forallStmt *)cstmt;

                Identifier *itr1 = forStmt->getIterator();
                Identifier *srcGraph1 = forStmt->getSourceGraph();
                proc_callExpr *procCall1 = forStmt->getExtractElementFunc();

                if (checkIdEqual(srcGraph, srcGraph1) && checkIdEqual(procCall1->getMethodId(), "neighbors"))
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

                                for (statement *ifBodyStmt : currStmt->getIfBody())
                                {
                                    assignment *assgn = (assignment *)stmt;
                                    PropAccess *propId = assgn->getPropId();

                                    if (assgn->getExpr()->isLiteral())
                                    {
                                        leftList.push_back(propId);
                                        rightList.push_back(assgn->getExpr());
                                    }
                                }

                                reductionCallStmt *modifiedStmt = Util::createNodeForReductionStmtList(leftList, reductionCallNode, rightList);
                            }
                        }
                    }
                }
            }
        }
    }
}
*/

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
            forAllAnalysis(currStmt);
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
        analyseFunc(func);
}