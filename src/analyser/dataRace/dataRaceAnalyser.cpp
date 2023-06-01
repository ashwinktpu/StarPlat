#include "dataRaceAnalyser.h"
#include "../analyserUtil.cpp"
#include "../../ast/ASTHelper.cpp"
#include "../../symbolutil/SymbolTable.h"
#include <unordered_map>

//Checking whether given two identifiers are equal
bool checkIdEqual(Identifier *id1, Identifier *id2)
{
    // TODO : Update with checking TableEntry*
    return (strcmp(id1->getIdentifier(), id2->getIdentifier()) == 0);
}
//Cheking whether given two property access are equal
bool checkPropIdEqual(PropAccess* prop1, PropAccess* prop2)
{
    return (checkIdEqual(prop1->getIdentifier1(), prop2->getIdentifier1())
            && checkIdEqual(prop1->getIdentifier2(), prop2->getIdentifier2()));
}

bool checkIdNameEqual(Identifier *id1, const char *c)
{
    return (strcmp(id1->getIdentifier(), c) == 0);
}
//Chenking whether two expression are syntactically equal
bool checkExprEqual(Expression *expr1, Expression *expr2)
{
    if(expr1->getExpressionFamily() == expr2->getExpressionFamily())
    {
        switch (expr1->getExpressionFamily())
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
Converting the following case,
if(ngbr.prop OP expr)
{
    ngbr.prop = expr;
    ((ngbr.propx = constVal) OR (var = constVal))*
}

to,
<ngbr.prop, leftList> = <T_MIN(ngbr.prop, expr), rightList>
*/
statement* dataRaceAnalyser::relPropUpdateAnalysis(ifStmt *stmt, Identifier *forIterator)
{
    Expression *cond = stmt->getCondition();

    if ((stmt->getElseBody() == nullptr) && cond->isRelational())
        if ((cond->getOperatorType() == OPERATOR_GT) || (cond->getOperatorType() == OPERATOR_LT))
        {
            Expression *propExpr = nullptr;
            Expression *otherExpr = nullptr;
            int opType = cond->getOperatorType();

            Expression *lExpr = cond->getLeft(), *rExpr = cond->getRight();

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
                ASTNode *reductionCallNode = nullptr;
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

                if ((leftList.size() == (ifBody->returnStatements()).size())
                    && (reductionCallNode != nullptr))
                {
                    if(rightList.size() == 0)
                    {
                        ASTNode* reductionCall = Util::createNodeForReductionStmt(propExpr->getPropId(), reductionCallNode);
                        return (statement*) reductionCall;
                    }
                    else
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionStmtList(leftList, reductionCallNode, rightList);
                        return (statement*) reductionCall;
                    }
                }
            }
        }
    
    return stmt;
}
//Analysing neighbour iteration loop which is inside for-all (parallel) loop 
statement* dataRaceAnalyser::ngbrForAnalysis(forallStmt *stmt, Identifier *forAllIterator, Identifier *srcGraph)
{
    proc_callExpr *procCall = stmt->getExtractElementFunc();

    if (stmt->isSourceProcCall() && (procCall->getArgList().size() == 1) 
        && checkIdEqual(srcGraph, stmt->getSourceGraph()) && checkIdNameEqual(procCall->getMethodId(), "neighbors"))
    {
        Identifier *ngbrItr = stmt->getIterator();
        argument *firstArg = *(procCall->getArgList().begin());
        Expression *argVal = firstArg->getExpr();
        
        if (argVal->isIdentifierExpr() && checkIdEqual(argVal->getId(), forAllIterator))
        {
            statement *stmtbody = stmt->getBody();
            if (stmtbody->getTypeofNode() == NODE_BLOCKSTMT)
            {
                blockStatement *body = (blockStatement *)stmtbody;
                blockStatement *newBody = blockStatement::createnewBlock();

                for (statement *bstmt : body->returnStatements())
                {
                    if (bstmt->getTypeofNode() == NODE_IFSTMT)
                    {
                        statement *newStmt = relPropUpdateAnalysis((ifStmt *)bstmt, (Identifier *) ngbrItr);
                        newBody->addStmtToBlock(newStmt);
                    }
                    else if (bstmt->getTypeofNode() == NODE_UNARYSTMT)
                    {
                        statement *newStmt = unaryPropReductionAnalysis((unary_stmt *)bstmt, (Identifier *) ngbrItr);
                        newBody->addStmtToBlock(newStmt);
                    }
                    else
                    {
                        newBody->addStmtToBlock(bstmt);
                    }
                }
                stmt->setBody(newBody);
            }
            else if (stmtbody->getTypeofNode() == NODE_IFSTMT)
            {
                statement *newStmt = relPropUpdateAnalysis((ifStmt *)stmtbody, (Identifier *) ngbrItr);
                stmt->setBody(newStmt);
            }
            else if (stmtbody->getTypeofNode() == NODE_UNARYSTMT)
            {
                statement *newStmt = unaryPropReductionAnalysis((unary_stmt *)stmtbody, (Identifier *) ngbrItr);
                stmt->setBody(newStmt);
            }
        }
    }
    return nullptr;
}
/*
Converting the following case,
globalVar (INC_OP || DEC_OP);

to,
globalVar OP= 1;
*/
statement* dataRaceAnalyser::unaryReductionAnalysis(unary_stmt* stmt)
{
    Expression* unaryExpr = stmt->getUnaryExpr();
    Expression* varExpr = unaryExpr->getUnaryExpr();

    if(varExpr->isIdentifierExpr())
    {
        ASTNode* VALUE_ONE = Util::createNodeForIval(1ll);
        if(unaryExpr->getOperatorType() == OPERATOR_INC)
        {
            ASTNode *reductionCall = Util::createNodeForReductionOpStmt(varExpr->getId(), OPERATOR_ADDASSIGN, VALUE_ONE);
            return (statement*) reductionCall;
        }
        else if(unaryExpr->getOperatorType() == OPERATOR_DEC)
        {
            ASTNode *reductionCall = Util::createNodeForReductionOpStmt(varExpr->getId(), OPERATOR_SUBASSIGN, VALUE_ONE);
            return (statement*) reductionCall;
        }
    }
    return stmt;
}
/*
Converting the following case,
ngbr.prop (INC_OP || DEC_OP);

to,
ngbr.prop OP= 1;
*/
statement* dataRaceAnalyser::unaryPropReductionAnalysis(unary_stmt* stmt, Identifier* propId)
{
    Expression* unaryExpr = stmt->getUnaryExpr();
    Expression* varExpr = unaryExpr->getUnaryExpr();

    if(varExpr->isPropIdExpr() && checkIdEqual(varExpr->getPropId()->getIdentifier1(), propId))
    {
        ASTNode* VALUE_ONE = Util::createNodeForIval(1ll);
        if(unaryExpr->getOperatorType() == OPERATOR_INC)
        {
            ASTNode *reductionCall = Util::createNodeForReductionOpStmt(varExpr->getPropId(), OPERATOR_ADDASSIGN, VALUE_ONE);
            return (statement*) reductionCall;
        }
        else if(unaryExpr->getOperatorType() == OPERATOR_DEC)
        {
            ASTNode *reductionCall = Util::createNodeForReductionOpStmt(varExpr->getPropId(), OPERATOR_SUBASSIGN, VALUE_ONE);
            return (statement*) reductionCall;
        }
    }
    return stmt;
}
/*
Converting the following case,
globalVar = globalVar ARITHMETIC_OP expr;

to,
globalVar OP= expr;
*/
statement* dataRaceAnalyser::assignReductionAnalysis(assignment *stmt)
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
                switch(rhsExpr->getOperatorType())
                {
                    case OPERATOR_ADD:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ADDASSIGN, rightSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_SUB:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_SUBASSIGN, rightSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_MUL:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_MULASSIGN, rightSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_AND:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ANDASSIGN, rightSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_OR:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ORASSIGN, rightSide);
                        return (statement*) reductionCall;
                    }
                }
            }
            else if (rightSide->isIdentifierExpr() && checkIdEqual(rightSide->getId(), lhsId))
            {
                switch(rhsExpr->getOperatorType())
                {
                    case OPERATOR_ADD:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ADDASSIGN, leftSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_MUL:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_MULASSIGN, leftSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_AND:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ANDASSIGN, leftSide);
                        return (statement*) reductionCall;
                    }
                    case OPERATOR_OR:
                    {
                        ASTNode *reductionCall = Util::createNodeForReductionOpStmt(lhsId, OPERATOR_ORASSIGN, leftSide);
                        return (statement*) reductionCall;
                    }
                }
            }
        }
    }
    return stmt;
}

/*
Requirements:
    Only contains Assignment,
                    Declaration
                    or Unary statement

    var = var op expr
    expr requirements:
        uses local variables
        uses properties of current iterator
        uses constants
        uses global variables which aren't written in current loop

Reduction condition:
    op = +,-,*,&&,||
    var is globalVar
    var shouldn't read or written in any of other statements
*/

statement* dataRaceAnalyser::blockReductionAnalysis(blockStatement* blockStmt, Identifier* forAllItr)
{
    usedVariables declaredVars, reducedVars;

    list<statement*> newStatements;
    list<statement*> oldStatements = blockStmt->returnStatements();

    for(statement* stmt: oldStatements)
    {
        if(stmt->getTypeofNode() == NODE_DECL)
        {
            declaration* cstmt = (declaration*) stmt;
            declaredVars.addVariable(cstmt->getdeclId(), READ_WRITE);
            newStatements.push_back(cstmt);
        }
        else if(stmt->getTypeofNode() == NODE_ASSIGN)
        {
            assignment* currStmt = (assignment*) stmt;
            
            if(currStmt->lhs_isIdentifier())
            {
                if(!declaredVars.isUsedVar(currStmt->getId()))
                {
                    if(reducedVars.isUsedVar(currStmt->getId()))
                        return blockStmt;
                    else
                    {
                        reducedVars.addVariable(currStmt->getId(), WRITE);
                        statement* newStmt = assignReductionAnalysis(currStmt);

                        if(!(newStmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT))
                            return blockStmt;

                        newStatements.push_back(newStmt);
                    }
                }
                else
                    newStatements.push_back(currStmt);
            }
            else if(currStmt->lhs_isProp())
            {
                PropAccess* lhs = currStmt->getPropId();
                if(!checkIdEqual(forAllItr, lhs->getIdentifier1()))
                    return blockStmt;
                else
                    newStatements.push_back(currStmt);
            }
        }
        else if(stmt->getTypeofNode() == NODE_UNARYSTMT)
        {
            unary_stmt* currStmt = (unary_stmt*) stmt;
            Expression* unaryExpr = currStmt->getUnaryExpr()->getUnaryExpr();

            if(unaryExpr->isPropIdExpr())
            {
                PropAccess* lhs = unaryExpr->getPropId();
                if(!checkIdEqual(forAllItr, lhs->getIdentifier1()))
                    return blockStmt;
                else
                    newStatements.push_back(currStmt);
            }
            else if(unaryExpr->isIdentifierExpr())
            {
                Identifier* lhs = unaryExpr->getId();
                if(!declaredVars.isUsedVar(lhs))
                {
                    if(reducedVars.isUsedVar(lhs))
                        return blockStmt;
                    else
                    {
                        reducedVars.addVariable(lhs, WRITE);
                        statement* newStmt = unaryReductionAnalysis(currStmt);

                        if(!(newStmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT))
                            return blockStmt;
                        newStatements.push_back(newStmt);
                    }
                }
                else
                    newStatements.push_back(currStmt);
            }
            else
                return blockStmt;
        }
        else
            return blockStmt;
    }

    declaredVars.clear();
    blockStatement* newBlock = blockStatement::createnewBlock();

    auto checkExprVars = [&declaredVars, &reducedVars, &forAllItr](Expression *expr) -> bool
    {
        usedVariables currUsed = analyserUtils::getVarsExpr(expr);

        //No reduced variable should be used
        for(Identifier* redVars: reducedVars.getVariables(WRITE)){
            if(currUsed.isUsedVar(redVars))
                return false;
        }

        //All writed should be to local variabls
        for(Identifier* wVars: currUsed.getVariables(WRITE)){
            if(!declaredVars.isUsedVar(wVars))
                return false;
        }

        //All used properties should be to current iterator
        for(PropAccess* propId: currUsed.getPropAcess(READ_WRITE)){
            if(!checkIdEqual(propId->getIdentifier1(), forAllItr))
                return false;
        }

        return true;
    };

    for(statement* stmt: newStatements)
    {
        if(stmt->getTypeofNode() == NODE_DECL)
        {
            declaration* cstmt = (declaration*) stmt;
            declaredVars.addVariable(cstmt->getdeclId(), READ_WRITE);

            if(cstmt->isInitialized() && !checkExprVars(cstmt->getExpressionAssigned()))
                return blockStmt;
        }
        else if(stmt->getTypeofNode() == NODE_ASSIGN)
        {
            if(!checkExprVars(((assignment*) stmt)->getExpr()))
                return blockStmt;
        }
        else if(stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
        {
            reductionCallStmt* currStmt = (reductionCallStmt*) stmt;
            Expression* rExpr = currStmt->getRightSide();

            if(!checkExprVars(((assignment*) stmt)->getExpr()))
                return blockStmt;
        }
        newBlock->addStmtToBlock(stmt);
    }
    return newBlock;
}
//Analyse parallel forall loop
statement* dataRaceAnalyser::forAllAnalysis(forallStmt *stmt)
{
    proc_callExpr *procCall = stmt->getExtractElementFunc();
    if (stmt->isForall() && checkIdNameEqual(procCall->getMethodId(), "nodes"))
    {
        Identifier *itr = stmt->getIterator();
        Identifier *sourceGraph = stmt->getSourceGraph();

        statement *body = stmt->getBody();
        if (body->getTypeofNode() == NODE_BLOCKSTMT)
        {
            blockStatement *blockbody = (blockStatement *)body;
            for (statement *bodyStmt : blockbody->returnStatements())
            {
                if (bodyStmt->getTypeofNode() == NODE_FORALLSTMT)
                    ngbrForAnalysis((forallStmt *) bodyStmt, itr, sourceGraph);
            }
            statement* rednAnalysisBlock = blockReductionAnalysis(blockbody, itr);
            stmt->setBody(rednAnalysisBlock);
        }
        else if (body->getTypeofNode() == NODE_FORALLSTMT)
            ngbrForAnalysis((forallStmt *)body, itr, sourceGraph);
        else if (body->getTypeofNode() == NODE_ASSIGN)
        {
            statement *newStmt = assignReductionAnalysis((assignment *)body);
            if(newStmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT)
            {
                reductionCallStmt* currStmt = (reductionCallStmt*) newStmt;
                usedVariables currUsed = analyserUtils::getVarsExpr(currStmt->getRightSide());
                bool flag = true;

                if(currUsed.isUsedVar(currStmt->getLeftId()))   flag = false;
                if(currUsed.hasVariables(WRITE)) flag = false;

                for(PropAccess* propId: currUsed.getPropAcess(READ_WRITE)){
                    if(!checkIdEqual(propId->getIdentifier1(), itr))
                        flag = false;
                }
                
                if(flag) stmt->setBody(newStmt);
            }
        }
        else if (body->getTypeofNode() == NODE_UNARYSTMT)
        {
            statement *newStmt = unaryReductionAnalysis((unary_stmt *)body);
            stmt->setBody(newStmt);
        }
    }
    return nullptr;
}
//Analyse all blocks using recursion
void dataRaceAnalyser::analyseStatement(statement *stmt)
{
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
    {
        blockStatement *blockStmt = (blockStatement *)stmt;
        for (statement *stmt : blockStmt->returnStatements())
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
//Analyse the given function
void dataRaceAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseStatement(func->getBlockStatement());
    return;
}
//Analyse all functions
void dataRaceAnalyser::analyse(list<Function *> funcList)
{
    // = frontEndContext.getFuncList();
    for (Function *func : funcList)
    {
        analyseFunc(func);
        printAST().printFunction(func);
    }
}
