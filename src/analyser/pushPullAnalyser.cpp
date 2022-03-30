#include "pushPullAnalyser.h"
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

blockStatement *PPAnalyser::checkSSSPUpdate(statement *stmt, Identifier *v, Identifier *nbr, Identifier *modified, Identifier *g, Identifier *finished)
{
    //cout << "checking SSSP" << endl;
    bool valid_edge = false;
    bool change = false;
    blockStatement *newBlockStmt = blockStatement::createnewBlock();
    Identifier *e = new Identifier;
    switch(stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:
        {
            blockStatement *blockStmt = (blockStatement *)stmt;
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
                            if (declExpr->getExpressionFamily() == EXPR_PROCCALL)
                            {
                                proc_callExpr *procExpr = (proc_callExpr *)declExpr;
                                if (checkIdEqual(procExpr->getId1(), g) && procExpr->getArgList().size() == 2)
                                {
                                    //cout << "edge ok" << endl;
                                    e = declId;
                                    valid_edge = 1;
                                    newBlockStmt->addStmtToBlock(bstmt);
                                }
                            }
                        }
                    }
                    case NODE_REDUCTIONCALLSTMT:
                    {
                        if (!valid_edge)
                            break;
                        //cout << "reduction call statement reeeeeeeee" << endl;;
                        reductionCallStmt *redStmt = (reductionCallStmt *)bstmt;
                        if (redStmt->isListInvolved())
                        {
                            list<ASTNode *> leftList = redStmt->getLeftList(), rightList = redStmt->getRightList();
                            if (leftList.size() == 2)
                            {
                                //cout << "2 member list" << endl;
                                ASTNode *first = leftList.front(), *second = leftList.back();
                                if (first->getTypeofNode() == NODE_PROPACCESS && second->getTypeofNode() == NODE_PROPACCESS)
                                {
                                    PropAccess *propRedLNode1 = (PropAccess *)first, *propRedLNode2 = (PropAccess *)second;
                                    if (checkIdEqual(propRedLNode1->getIdentifier1(), nbr) && checkIdEqual(propRedLNode2->getIdentifier1(), nbr)
                                        && checkIdEqual(propRedLNode2->getIdentifier2(), modified))
                                    {
                                        //cout << "LHS is okay" << endl;
                                        Identifier *dist = propRedLNode1->getIdentifier2();
                                        ASTNode *firstExpr = rightList.front();
                                        reductionCall *redCallStmt = (reductionCall *)redStmt->getReducCall();
                                        if (redCallStmt->getargList().size() == 2 && redCallStmt->getReductionType() == REDUCE_MIN)
                                        {
                                            //cout << "reduction shape seems okay" << endl;
                                            argument *arg1 = redCallStmt->getargList().front(), *arg2 = redCallStmt->getargList().back();
                                            //cout << arg1->isExpr() << " " << arg2->isExpr() << endl;
                                            if (arg1->isExpr() && arg1->getExpr()->getExpressionFamily() == EXPR_PROPID && arg2->isExpr() && arg2->getExpr()->getExpressionFamily() == EXPR_ARITHMETIC)
                                            {
                                                //cout << "kinda okay reduction" << endl;
                                                PropAccess *prop1 = arg1->getExpr()->getPropId();
                                                if (checkIdEqual(prop1->getIdentifier1(), nbr) && checkIdEqual(prop1->getIdentifier2(), dist))
                                                {
                                                    //cout << "reduction call is okay" << endl;
                                                    //cout << arg2->getExpr()->getExpressionFamily() << endl;
                                                    Expression *expr2L = arg2->getExpr()->getLeft(), *expr2R = arg2->getExpr()->getRight();
                                                    if (expr2L->getExpressionFamily() == EXPR_PROPID && expr2R->getExpressionFamily() == EXPR_PROPID)
                                                    {
                                                        //cout << "yeah shape really okay" << endl;
                                                        PropAccess *prop2L = expr2L->getPropId(), *prop2R = expr2R->getPropId();
                                                        if (checkIdEqual(prop2L->getIdentifier1(), v) && checkIdEqual(prop2L->getIdentifier2(), dist) && checkIdEqual(prop2R->getIdentifier1(), e) && checkIdNameEqual(prop2R->getIdentifier2(), "weight"))
                                                        {
                                                            if (firstExpr->getTypeofNode() == NODE_EXPR && ((Expression *)firstExpr)->getExpressionFamily() == EXPR_BOOLCONSTANT)
                                                            {
                                                                //cout << "this is a valid sssp loop" << endl;
                                                                Expression *ifCondn = Expression::nodeForRelationalExpr(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(v, dist)), Expression::nodeForArithmeticExpr(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(nbr, dist)), Expression::nodeForPropAccess(PropAccess::createPropAccessNode(e, Identifier::createIdNode("weight"))), OPERATOR_ADD), OPERATOR_GT);
                                                                assignment *assn1 = assignment::prop_assignExpr(PropAccess::createPropAccessNode(v, dist), Expression::nodeForArithmeticExpr(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(nbr, dist)), Expression::nodeForPropAccess(PropAccess::createPropAccessNode(e, Identifier::createIdNode("weight"))), OPERATOR_ADD));
                                                                assignment *assn2 = assignment::prop_assignExpr(PropAccess::createPropAccessNode(v, modified), Expression::nodeForBooleanConstant(((Expression *)firstExpr)->getBooleanConstant()));
                                                                assignment *assn3 = assignment::id_assignExpr(Identifier::createIdNode(finished->getIdentifier()), Expression::nodeForBooleanConstant(false));
                                                                blockStatement *ifBlock = blockStatement::createnewBlock();
                                                                ifBlock->addStmtToBlock(assn1);
                                                                ifBlock->addStmtToBlock(assn2);
                                                                ifBlock->addStmtToBlock(assn3); 
                                                                ifStmt *ifstmt = ifStmt::create_ifStmt(ifCondn, ifBlock, NULL); 
                                                                newBlockStmt->addStmtToBlock(ifstmt);
                                                                /*list <ASTNode *> leftList, rightList;
                                                                leftList.push_back(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(v, dist)));
                                                                leftList.push_back(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(v, modified)));
                                                                list <argument *> reducList;
                                                                argument *arg1 = new argument;
                                                                argument *arg2 = new argument;
                                                                arg1->setExpression(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(v, dist)));
                                                                arg1->setExpressionFlag();
                                                                reducList.push_back(arg1);
                                                                arg2->setExpressionFlag();
                                                                arg2->setExpression(Expression::nodeForArithmeticExpr(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(nbr, dist)), Expression::nodeForPropAccess(PropAccess::createPropAccessNode(e, Identifier::createIdNode("weight"))), OPERATOR_ADD));
                                                                reducList.push_back(arg2);
                                                                rightList.push_back(Expression::nodeForBooleanConstant(((Expression *)firstExpr)->getBooleanConstant()));
                                                                reductionCallStmt *redStmt = reductionCallStmt::leftList_reducCallStmt(leftList, reductionCall::nodeForReductionCall(REDUCE_MIN, reducList), rightList);
                                                                
                                                                newBlockStmt->addStmtToBlock(redStmt);*/
                                                                change = true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (change)
        return newBlockStmt;
    else
        return NULL;
}

forallStmt *PPAnalyser::analyseForallIn(statement *stmt, Identifier *srcGraph, Identifier *v, Identifier *modified, int boolConstant, Identifier *finished)
{
    //cout << "checking inner forall" << endl;
    forallStmt *inFstmt = (forallStmt *)stmt;
    if (!inFstmt->hasFilterExpr())
    {
        Identifier *nbr = inFstmt->getIterator(), *inSrcGraph = inFstmt->getSourceGraph();
        if (checkIdNameEqual(inFstmt->getExtractElementFunc()->getMethodId(), "neighbors")
            && checkIdEqual(inSrcGraph, srcGraph))
        {
            proc_callExpr *extractElemFunc = inFstmt->getExtractElementFunc();
            if (extractElemFunc->getArgList().size() == 1)
            {
                blockStatement *updatedBlock = checkSSSPUpdate(inFstmt->getBody(), v, nbr, modified, srcGraph, finished);
                if (updatedBlock != NULL)
                {
                    //cout << "SSSP was changed" << endl;
                    list<argument *> argList;
                    argument *arg1 = new argument;
                    arg1->setExpression(Expression::nodeForIdentifier(v));
                    arg1->setExpressionFlag();
                    argList.push_back(arg1);
                    Expression *filterExpr = Expression::nodeForRelationalExpr(Expression::nodeForPropAccess(PropAccess::createPropAccessNode(nbr, modified)), Expression::nodeForBooleanConstant(boolConstant), OPERATOR_EQ);
                    forallStmt *newFstmt = forallStmt::createforallStmt(nbr, srcGraph, proc_callExpr::nodeForProc_Call(NULL, NULL, Identifier::createIdNode("nodes_to"), argList), updatedBlock, filterExpr, inFstmt->isForall());
                    return newFstmt;
                }
            }
        }
    }
    return NULL;
}

forallStmt *PPAnalyser::analyseForallOut(statement *stmt, Identifier *modified, Identifier *finished)
{
    //cout << "checking outer forall" << endl;
    forallStmt *fstmt = (forallStmt *)stmt;
    //cout << "lol" << endl;
    Identifier *v = fstmt->getIterator();
    Identifier *srcGraph = fstmt->getSourceGraph();
    if (fstmt->isForall() && checkIdNameEqual(fstmt->getExtractElementFunc()->getMethodId(), "nodes") && fstmt->hasFilterExpr())
    {
        Expression *filterExpr = fstmt->getfilterExpr();
        if (filterExpr->getOperatorType() == OPERATOR_EQ)
        {
            if (filterExpr->getLeft()->getExpressionFamily() == EXPR_PROPID)
            {
                //cout << "prop equal expression correct" << endl;
                PropAccess *filterLeft = filterExpr->getLeft()->getPropId();
                //cout << "prop is " << filterLeft->getIdentifier1()->getIdentifier() << " " << filterLeft->getIdentifier2()->getIdentifier() << endl;
                if (checkIdEqual(v, filterLeft->getIdentifier1()) && checkIdEqual(filterLeft->getIdentifier2(), modified))
                {
                    //cout << "ids correct" << endl;
                    if (filterExpr->getRight()->getExpressionFamily() == EXPR_BOOLCONSTANT)
                    {
                        statement *body = fstmt->getBody();
                        forallStmt *updatedForall = new forallStmt;
                        switch(body->getTypeofNode())
                        {
                            case NODE_BLOCKSTMT:
                                //cout << "hereeee1" << endl;
                                for (statement *bodyStmt: ((blockStatement *)body)->returnStatements())
                                    switch (bodyStmt->getTypeofNode())
                                    {
                                        case NODE_FORALLSTMT:
                                            //cout << "this caase" << endl;
                                            updatedForall = analyseForallIn(bodyStmt, srcGraph, v, modified, filterExpr->getRight()->getBooleanConstant(), finished);
                                            if (updatedForall != NULL)
                                            {   
                                                //cout << "infor changed 1" << endl;
                                                list<argument *> argList;
                                                blockStatement *newBlock = blockStatement::createnewBlock();
                                                newBlock->addStmtToBlock(updatedForall);
                                                forallStmt *newForallStmt = forallStmt::createforallStmt(v, srcGraph, proc_callExpr::nodeForProc_Call(NULL, NULL, Identifier::createIdNode("nodes"), argList), newBlock, NULL, 1);
                                                return newForallStmt;
                                            }
                                            break;
                                    }
                                break;
                            case NODE_FORALLSTMT:
                                //cout << "hi" << endl;
                                updatedForall = analyseForallIn(body, srcGraph, v, modified, filterExpr->getRight()->getBooleanConstant(), finished);
                                if (updatedForall != NULL)
                                {   
                                    //cout << "infor changed 2" << endl;
                                    list<argument *> argList;
                                    forallStmt *newForallStmt = forallStmt::createforallStmt(v, srcGraph, proc_callExpr::nodeForProc_Call(NULL, NULL, Identifier::createIdNode("nodes"), argList), updatedForall, NULL, 1);
                                    return newForallStmt;    
                                }
                                break;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

fixedPointStmt *PPAnalyser::analyseFPLoop(statement *stmt)
{
    //cout << "checking FP loop" << endl;
    fixedPointStmt *fpStmt = (fixedPointStmt *)stmt;

    statement *body = fpStmt->getBody();
    Expression *expr = fpStmt->getDependentProp();
    Identifier *fpId = fpStmt->getFixedPointId();
    if (expr->isUnary())
    {
        Identifier *modified = expr->getUnaryExpr()->getId();
        //cout << modified->getIdentifier() << endl;
        if (expr->getOperatorType() == OPERATOR_NOT)
        {
            //cout << "success" << endl;
            statement *body = fpStmt->getBody();
            //cout << body->getTypeofNode() << endl;
            switch (body->getTypeofNode())
            {
                case NODE_BLOCKSTMT:
                    for (statement *bodyStmt: ((blockStatement *)body)->returnStatements())
                        switch (bodyStmt->getTypeofNode())
                        {
                            case NODE_FORALLSTMT:
                                //cout << "this caase" << endl;
                                forallStmt *updatedForall = analyseForallOut(bodyStmt, modified, fpId);
                                //cout << (updatedForall != NULL) << "kekek" << endl;
                                if (updatedForall != NULL)
                                {
                                    //cout << "outfor changed" << endl;
                                    blockStatement *newBlock = blockStatement::createnewBlock();
                                    newBlock->addStmtToBlock(updatedForall);
                                    return fixedPointStmt::createforfixedPointStmt(fpId, expr, newBlock);
                                }
                        }
                case NODE_FORALLSTMT:
                    //cout << "this case" << endl;
                    forallStmt *updatedForall = analyseForallOut(stmt, modified, fpId);
                    if (updatedForall != NULL)
                    {
                        //cout << "outfor changed" << endl;
                        return fixedPointStmt::createforfixedPointStmt(fpId, expr, updatedForall);
                    }
            }
        }
    }
    return NULL;
}

void PPAnalyser::analyseBlock(statement* stmt)
{
    //cout << "analysing block" << endl;
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
            case NODE_FIXEDPTSTMT:
            {
                //cout << "FP" << endl;
                fixedPointStmt *fpStmt = analyseFPLoop(stmt);
                if (fpStmt != NULL)
                {
                    //cout << "replacing fp" << endl;
                    blockStmt->replaceStatement(stmt, fpStmt);
                }
                break;
            }
        }
    }
}

void PPAnalyser::analyseFunc(ASTNode *proc)
{
    //cout << "analysing func" << endl;
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