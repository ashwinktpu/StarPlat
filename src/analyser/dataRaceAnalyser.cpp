#include "dataRaceAnalyser.h"
#include "analyserUtil.hpp"
#include "../ast/ASTHelper.cpp"
#include "../symbolutil/SymbolTable.h"
#include <unordered_map>

bool checkIdEqual(Identifier *id1, Identifier *id2)
{
    // TODO : Update with checking TableEntry*
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
        case EXPR_RELATIONAL:        
        case EXPR_LOGICAL:
        case EXPR_ARITHMETIC:
            if(expr1->getOperatorType() == expr2->getOperatorType())
                return (checkExprEqual(expr1->getLeft(), expr2->getLeft())
                        && checkExprEqual(expr1->getRight(), expr2->getRight()));
            break;

        case EXPR_UNARY:
            if((expr1->getOperatorType() == expr2->getOperatorType())
                && (expr1->getOperatorType() == OPERATOR_NOT))
                return checkExprEqual(expr1->getUnaryExpr(), expr2->getUnaryExpr());
            break;
            
        case EXPR_ID:
            return checkIdEqual(expr1->getId(), expr2->getId());
            break;

        case EXPR_PROPID:
            return checkPropIdEqual(expr1->getPropId(), expr2->getPropId());
            break;

        case EXPR_PROCCALL:
            // TODO: Add procedure call check here
            break;

        case EXPR_BOOLCONSTANT:
            return (expr1->getBooleanConstant() == expr2->getBooleanConstant());
            break;
        
        case EXPR_INTCONSTANT:
            return (expr1->getIntegerConstant() == expr2->getIntegerConstant());
            break;
        
        case EXPR_FLOATCONSTANT:
            return (expr1->getFloatConstant() == expr2->getFloatConstant());
            break;
        
        case EXPR_INFINITY:
            return (expr1->isPositiveInfinity() == expr2->isPositiveInfinity());
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
                        reductionCallStmt* reductionCall = Util::createNodeForReductionOpStmt(propExpr->getPropId(), reductionCallNode);
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
                        // TODO : Replace the current statment with new statement
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

ASTNode* dataRaceAnalyser::unaryReductionAnalysis(unary_stmt* stmt)
{
    Expression* unaryExpr = stmt->getUnaryExpr();
    if(unaryExpr->isIdentifierExpr())
    {
        Expression* VALUE_ONE = Util::createNodeForIval(1ll);
        if(unaryExpr->getOperatorType() == OPERATOR_INC)
        {
            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(unaryExpr->getId(), OPERATOR_ADDASSIGN, VALUE_ONE);
            return reductionCall;
        }
        else if(unaryExpr->getOperatorType() == OPERATOR_DEC)
        {
            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(unaryExpr->getId(), OPERATOR_SUBASSIGN, VALUE_ONE);
            return reductionCall;
        }
    }
    return stmt;
}

ASTNode* dataRaceAnalyser::assignReductionAnalysis(assignment *stmt)
{
    if (stmt->lhs_isIdentifier())
    {
        Identifier *lhsId = stmt->getId();
        Expression *rhsExpr = stmt->getExpr();
        if (rhsExpr->isArithmetic() || rhsExpr->isLogical())
        {
            Expression *leftSide = rhsExpr->getLeft();
            Expression *rightSide = rhsExpr->getRight();
            if (leftSide->isIdentifierExpr() && checkIdEqual(leftSide->getId(), lhsId))
            {
                usedVariables rightSideVars = getVarsExpr(rightSide);
                if (!rightSideVars.isUsed(lhsId))
                {
                    switch(rhsExpr->getOperatorType())
                    {
                        case OPERATOR_PLUS:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ADDASSIGN, rightSide);
                            return reductionCall;
                        case OPERATOR_MINUS:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_SUBASSIGN, rightSide);
                            return reductionCall;
                        case OPERATOR_MULTIPLY:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_MULASSIGN, rightSide);
                            return reductionCall;
                        case OPERATOR_AND:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ANDASSIGN, rightSide);
                            return reductionCall;
                        case OPERATOR_OR:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ORASSIGN, rightSide);
                            return reductionCall;
                    }
                }
            }
            else if (rightSide->isIdentifierExpr() && checkIdEqual(rightSide->getId(), lhsId))
            {
                usedVariables leftSideVars = getVarsExpr(leftSide);
                if (!leftSideVars.isUsed(lhsId))
                {
                    switch(rhsExpr->getOperatorType())
                    {
                        case OPERATOR_PLUS:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ADDASSIGN, leftSide);
                            return reductionCall;
                        case OPERATOR_MULTIPLY:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_MULASSIGN, leftSide);
                            return reductionCall;
                        case OPERATOR_AND:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ANDASSIGN, leftSide);
                            return reductionCall;
                        case OPERATOR_OR:
                            reductionCallStmt *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ORASSIGN, leftSide);
                            return reductionCall;
                    }
                }
            }
        }
    }
    return stmt;
}

ASTNode* dataRaceAnalyser::blockReductionAnalysis(blockStatement* blockStmt, Identifier* forAllItr)
{
    usedVariables declaredVars, globalVars;

    list<statement*> newStatements;
    list<statement*> oldStatements = blockStmt->returnStatements();

    for(statement* stmt: oldStatements)
    {
        if(stmt->getTypeofNode() == NODE_DECL)
        {
            declaration* stmt = (declaration*) stmt;
            declaredVars.addVariable(stmt->getdeclId(), READ_WRITE);
            newStatements.push_back(stmt);
        }
        else if(stmt->getTypeofNode() == NODE_ASSIGN)
        {
            assignment* currStmt = (assignment*) stmt;
            
            if(currStmt->lhs_isIdentifier())
            {
                if(!declaredVars.isUsed(currStmt->getId()))
                {
                    if(globalVars.isUsed(currStmt->getId()))
                        return blockStmt;
                    else
                    {
                        globalVars.addVariable(currStmt->getId(), WRITE);
                        ASTNode* newStmt = assignReductionAnalysis(currStmt);

                        if(newStmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
                        {
                            Expression* rhsExpr = ((reductionCallStmt*) newStmt)->getRightSide();
                            bool isValid = true;

                            
                        }
                    }
                }
                else
                {

                }
            }
            else if(currStmt->lhs_isProp())
                globalVars.addVariable(currStmt->getPropId()->getIdentifier2(), WRITE);

            if((newStmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
                && !declaredVars.isUsed(((reductionCallStmt*) newStmt)->getLeftId()))
            {
                newStatements.push_back((reductionCallStmt*) newStmt);
                Identifier* rednIden = ((reductionCallStmt*) newStmt)->getLeftId();
                rednFrequency[rednIden->getSymbolInfo()]++;


            }
            else
                newStatements.push_back(stmt);
        }
        else if(stmt->getTypeofNode() == NODE_UNARYSTMT)
        {
            ASTNode* newStmt = unaryReductionAnalysis((unary_stmt*) stmt);
            if((newStmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
                && !declaredVars.isUsed(((reductionCallStmt*) newStmt)->getLeftId()))
            {
                newStatements.push_back((reductionCallStmt*) newStmt);
                Identifier* rednIden = ((reductionCallStmt*) newStmt)->getLeftId();
                rednFrequency[rednIden->getSymbolInfo()]++;

                globalVars.merge(getVarsExpr(((unary_stmt*) stmt)->getUnaryExpr()));
            }
            else
                newStatements.push_back(stmt);
        }
        else
            return blockStmt;
    }

    blockStatement* newBlock = blockStatement::createnewBlock();
    for(int i=0; i<oldStatements.size(); i++)
    {
        if(oldStatements[i] == newStatements[i])
            newBlock->addStmtToBlock(oldStatements[i]);
        
    }
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
                    ngbrForAnalysis((forallStmt *) stmt, itr, sourceGraph);
            }
        }
        else if (body->getTypeofNode() == NODE_FORALLSTMT)
            ngbrForAnalysis((forallStmt *)body, itr, sourceGraph);
        else if (body->getTypeofNode() == NODE_ASSIGN)
        {
            ASTNode *newStmt = assignReductionAnalysis((assignment *)body);
            stmt->setBody(newStmt);
        }
        else if (body->getTypeofNode() == NODE_UNARYSTMT)
        {
            ASTNode *newStmt = unaryReductionAnalysis((unary_stmt *)body);
            stmt->setBody(newStmt);
        }
    }
    return nullptr;
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