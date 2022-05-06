#include "PRAnalyser.h"
#include "../../ast/ASTHelper.cpp"
#include "../../symbolutil/SymbolTable.h"
#include "../analyserUtil.cpp"

bool checkIdEqual(Identifier *id1, Identifier *id2);

bool checkIdNameEqual(Identifier *id1, char *c);

/*bool checkIdEqual(Identifier *id1, Identifier *id2)
{
    return id1 != NULL && id2 != NULL && (strcmp(id1->getIdentifier(), id2->getIdentifier()) == 0);
}

bool checkIdNameEqual(Identifier *id1, char *c)
{
    return (strcmp(id1->getIdentifier(), c) == 0);
}*/

bool checkPropEqual(PropAccess *prop1, PropAccess *prop2)
{
    return (strcmp(prop1->getIdentifier2()->getIdentifier(), prop2->getIdentifier2()->getIdentifier()) == 0);
}

ASTNode *IdenReplace(ASTNode *stmt, Identifier *sum, PropAccess *v_sum)
{
    switch(stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:
        {
            blockStatement *bStmt = (blockStatement *)stmt;
            blockStatement *nbStmt = blockStatement::createnewBlock();
            for (statement *stmt: bStmt->returnStatements())
                nbStmt->addStmtToBlock((statement *)IdenReplace(stmt, sum, v_sum));
            return nbStmt;
        }
        case NODE_FORALLSTMT:
        {
            forallStmt *currStmt = (forallStmt *)stmt;
            switch(currStmt->getBody()->getTypeofNode())
            {
                case NODE_BLOCKSTMT:
                    return forallStmt::createforallStmt(currStmt->getIterator(), currStmt->getSourceGraph(), currStmt->getExtractElementFunc(), (statement *)IdenReplace(currStmt->getBody(), sum, v_sum), currStmt->getfilterExpr(), currStmt->isForall());
            }
            return stmt;
        }
        case NODE_ASSIGN:
        {
            assignment *assignStmt = (assignment *)stmt;
            if (assignStmt->lhs_isIdentifier())
            {
                Identifier *id = (Identifier *)(assignStmt->getId());
                if (checkIdEqual(id, sum))
                {
                    return assignment::prop_assignExpr(v_sum, (Expression *)IdenReplace(assignStmt->getExpr(), sum, v_sum));
                }
            }
            return stmt;
        }
        case NODE_EXPR:
        {
            Expression *expr = (Expression *)stmt;
            if (expr->getExpressionFamily() == EXPR_ARITHMETIC)
            {
                Expression *lhs = expr->getLeft(), *rhs = expr->getRight();
                Expression *new_lhs = (Expression *)IdenReplace(lhs, sum, v_sum);
                Expression *new_rhs = (Expression *)IdenReplace(rhs, sum, v_sum);
                Expression *replaced = Expression::nodeForArithmeticExpr(new_lhs, new_rhs, expr->getOperatorType());
                if (expr->hasEnclosedBrackets())
                    replaced->setEnclosedBrackets();
                return replaced;
            }
            else if (expr->getExpressionFamily() == EXPR_ID)
            {
                if (checkIdEqual(expr->getId(), sum))
                {
                    Expression *replaced = Expression::nodeForPropAccess(v_sum);
                    if (expr->hasEnclosedBrackets())
                        replaced->setEnclosedBrackets();
                    return replaced;
                }
                return stmt;
            }
            return stmt;
        }
        case NODE_DECL:
        {
            declaration *decl = (declaration *)stmt;
            return declaration::assign_Declaration(decl->getType(), decl->getdeclId(), (Expression *)(IdenReplace(decl->getExpressionAssigned(), sum, v_sum)));
        }
        default:
            return stmt;
    }
}

BoolProp PRAnalyser::analyseFor(forallStmt *stmt)
{
    Identifier *nbr = stmt->getIterator();
    statement *body = stmt->getBody();
    switch (body->getTypeofNode())
    {
        case NODE_ASSIGN:
        {
            assignment *assignStmt = (assignment *)body;
            usedVariables usedVars = analyserUtils::getVarsExpr(assignStmt->getExpr());
            list <PropAccess *> usedProps = usedVars.getPropAcess();
            if (usedProps.size() > 0)
                return BoolProp(true, usedProps.front(), assignStmt->getId());
            break;
        }
        case NODE_BLOCKSTMT:
        {
            blockStatement *blockStmt = (blockStatement *)body;
            for (statement *stmt: blockStmt->returnStatements())
                if (stmt->getTypeofNode() == NODE_ASSIGN)
                {
                    assignment *assnStmt = (assignment *)stmt;
                    usedVariables usedVars = analyserUtils::getVarsExpr(assnStmt->getExpr());
                    list <PropAccess *> usedProps = usedVars.getPropAcess();
                    if (usedProps.size() > 0)
                        return BoolProp(true, usedProps.front(), assnStmt->getId());
                }
        }
    }
    return BoolProp(false);
}

BoolProp PRAnalyser::analyseForAll(forallStmt *stmt)
{
    Identifier *itr = stmt->getIterator();
    Identifier *sourceGraph = stmt->getSourceGraph();

    statement *body = stmt->getBody();

    if (body->getTypeofNode() == NODE_BLOCKSTMT)
    {
        bool read_flag = false, write_flag = false, omp_split_loop = false;
        PropAccess *read_prop = NULL;
        Identifier *read_sum = NULL;
        blockStatement *blockStmt = (blockStatement *)body;
        int count = 1;
        BoolProp bp;
        for (statement *stmt: blockStmt->returnStatements())
        {
            switch (stmt->getTypeofNode())
            {
                case NODE_FORALLSTMT:
                {
                    forallStmt *currStmt = (forallStmt *)stmt;
                    proc_callExpr *procCall = currStmt->getExtractElementFunc();
                    if (currStmt->isSourceProcCall() && (procCall->getArgList().size() == 1) 
                        && checkIdEqual(sourceGraph, currStmt->getSourceGraph())
                        && (checkIdNameEqual(procCall->getMethodId(), "neighbours")
                            || checkIdNameEqual(procCall->getMethodId(), "nodes_to")))
                    {
                        bp = analyseFor(currStmt);
                        if (bp.getBool())
                        {
                            read_flag = true;
                            read_prop = bp.getProp();
                            read_sum = bp.getSum();
                        }
                    }
                    break;
                }
                case NODE_ASSIGN:
                {
                    assignment *assignStmt = (assignment *)stmt;
                    if (assignStmt->lhs_isProp())
                    {
                        PropAccess *lhs_prop = assignStmt->getPropId();
                        if (checkPropEqual(lhs_prop, read_prop))
                        {
                            // cout << "barrier needed" << endl;
                            if (strcmp(backend, "omp") == 0)
                            {
                                omp_split_loop = 1;
                            }
                            if (strcmp(backend, "cuda") == 0)
                            {
                                barrStmt *barrier = barrStmt::nodeForBarrier();
                                blockStmt->insertToBlock(barrier, count);
                            }
                        }
                    }
                    break;
                }
            }
            ++count;
        }
        if (omp_split_loop)
            return bp;
    }
    return BoolProp(false);
}

blockStatement *PRAnalyser::analyseDoWhile(statement *stmt)
{
    BoolProp bp;
    blockStatement *blockStmt = (blockStatement *)stmt;
    Identifier *v = NULL;
    forallStmt *faStmt = new forallStmt;
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
                forallStmt *currStmt = (forallStmt *)stmt;
                faStmt = currStmt;
                v = currStmt->getIterator();
                if (currStmt->isForall() && checkIdNameEqual(currStmt->getExtractElementFunc()->getMethodId(), "nodes"))
                {
                    bp = analyseForAll((forallStmt *)currStmt);
                }
                else
                    analyseBlock(currStmt->getBody());
                break;
            }
        }
    }
    if (bp.getBool())
    {
        blockStatement *bStmt = (blockStatement *)(faStmt->getBody()); 
        forallStmt *fstmt1 = new forallStmt;
        blockStatement *for2Block = blockStatement::createnewBlock();
        Identifier *reduc_id;
        for (statement *stmt: bStmt->returnStatements())
        {
            switch (stmt->getTypeofNode())
            {
            case NODE_DECL:
            {
                declaration *declStmt = (declaration *)stmt;
                if (checkIdEqual(declStmt->getdeclId(), bp.getSum()))
                {
                    continue;
                }
                for2Block->addStmtToBlock((statement *)IdenReplace(declStmt, bp.getSum(), PropAccess::createPropAccessNode(v, bp.getSum())));
                break;
            }
            case NODE_FORALLSTMT:
            {
                forallStmt *fstmt = (forallStmt *)stmt;
                fstmt1 = (forallStmt *)IdenReplace(fstmt, bp.getSum(), PropAccess::createPropAccessNode(v, bp.getSum()));
                break;
            }

            case NODE_REDUCTIONCALLSTMT:
                for2Block->addStmtToBlock(stmt);
                reduc_id = ((reductionCallStmt *)stmt)->getLeftId();
                break;

            case NODE_ASSIGN:
                for2Block->addStmtToBlock((statement *)IdenReplace(stmt, bp.getSum(), PropAccess::createPropAccessNode(v, bp.getSum())));

            }
        }
        blockStatement *newBody = blockStatement::createnewBlock();
        newBody->addStmtToBlock(declaration::normal_Declaration(Type::createForPropertyType(TYPE_PROPNODE, TYPE_FLOAT, Type::createForPrimitive(TYPE_FLOAT, TYPE_FLOAT)), bp.getSum()));
        list<argument *> argList;
        argument *arg1 = new argument;
        arg1->setAssignFlag();
        arg1->setAssign(assignment::id_assignExpr(bp.getSum(), Expression::nodeForIntegerConstant(0)));
        argList.push_back(arg1);
        newBody->addStmtToBlock(proc_callStmt::nodeForCallStmt(proc_callExpr::nodeForProc_Call(fstmt1->getSourceGraph(), NULL, Identifier::createIdNode("attachNodeProperty"), argList)));
        for (statement *stmt: blockStmt->returnStatements())
        {
            switch (stmt->getTypeofNode())
            {
            case NODE_FORALLSTMT:
            {
                blockStatement *tempBody = blockStatement::createnewBlock(); 
                tempBody->addStmtToBlock(fstmt1);
                newBody->addStmtToBlock(forallStmt::createforallStmt(faStmt->getIterator(), faStmt->getSourceGraph(), faStmt->getExtractElementFunc(), tempBody, faStmt->getfilterExpr(), true));
                forallStmt *fStmt2 = forallStmt::createforallStmt(faStmt->getIterator(), faStmt->getSourceGraph(), faStmt->getExtractElementFunc(), for2Block, faStmt->getfilterExpr(), true);
                fStmt2->push_reduction(OPERATOR_ADD, reduc_id);
                newBody->addStmtToBlock(fStmt2);
                break;
            }
            default:
                newBody->addStmtToBlock(stmt);
            }
        }
        return newBody;
    }
    return NULL;
}

void PRAnalyser::analyseBlock(statement *stmt)
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
            case NODE_DOWHILESTMT:
            {
                dowhileStmt *dowhile = (dowhileStmt *)stmt;
                if (dowhile->getBody()->getTypeofNode() == NODE_BLOCKSTMT)
                {
                    blockStatement *newBody = analyseDoWhile(dowhile->getBody());
                    list<statement *> stmtList = newBody->returnStatements();
                    if (newBody != NULL)
                    {
                        dowhileStmt *newdowhile = dowhileStmt::create_dowhileStmt(dowhile->getCondition(), newBody);
                        blockStmt->replaceStatement(dowhile, newdowhile);
                    }
                }
                break;
            }
        }
    }
}

void PRAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseBlock(func->getBlockStatement());
    return;
}

void PRAnalyser::analyse(char *backend_sent)
{
    backend = backend_sent;
    list<Function *> funcList = frontEndContext.getFuncList();
    for (Function *func : funcList)
        analyseFunc(func);
}