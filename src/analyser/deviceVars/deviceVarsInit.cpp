#include "deviceVarsAnalyser.h"

bool deviceVarsAnalyser::initBlock(blockStatement *blockStmt, list<Identifier *> &vars)
{
    ASTNodeWrap *blockNode = initWrapNode(blockStmt, vars);

    bool hasForAll = false;
    int localCount = 0;

    for (statement *stmt : blockStmt->returnStatements())
    {
        hasForAll |= initStatement(stmt, vars);
        if (stmt->getTypeofNode() == NODE_DECL)
        {
            declaration *currStmt = (declaration *)stmt;
            vars.push_back(currStmt->getdeclId());
            localCount++;
        }
    }

    while (localCount--)
        vars.pop_back();

    return hasForAll;
}

bool deviceVarsAnalyser::initStatement(statement *stmt, list<Identifier *> &vars)
{
    printf("Initializing %p %d\n", stmt, stmt->getTypeofNode());

    switch (stmt->getTypeofNode())
    {
    case NODE_DECL:
        return initDeclaration((declaration *)stmt, vars);
    case NODE_ASSIGN:
        return initAssignment((assignment *)stmt, vars);
    case NODE_UNARYSTMT:
        return initUnary((unary_stmt *)stmt, vars);
    case NODE_FORALLSTMT:
    {
        forallStmt *forStmt = (forallStmt *)stmt;
        if (forStmt->isForall())
            return initForAll((forallStmt *)stmt, vars);
        else
            return initFor((forallStmt *)stmt, vars);
    }
    case NODE_BLOCKSTMT:
        return initBlock((blockStatement *)stmt, vars);
    case NODE_IFSTMT:
        return initIfElse((ifStmt *)stmt, vars);
    case NODE_WHILESTMT:
        return initWhile((whileStmt *)stmt, vars);
    case NODE_DOWHILESTMT:
        return initDoWhile((dowhileStmt *)stmt, vars);
    case NODE_PROCCALLSTMT:
        return initProcCall((proc_callStmt *)stmt, vars);
    case NODE_FIXEDPTSTMT:
        return initFixedPoint((fixedPointStmt *)stmt, vars);
    case NODE_REDUCTIONCALLSTMT:
        return initReduction((reductionCallStmt *)stmt, vars);
    case NODE_ITRBFS:
        return initItrBFS((iterateBFS *)stmt, vars);
        /*S
            case NODE_REDUCTIONCALLSTMT:
            return analyseReduction((reductionCallStmt *)stmt, inMap);
            case NODE_ITRBFS:
            return analyseItrBFS((iterateBFS *)stmt, inMap);*/
    }
    return false;
}
bool deviceVarsAnalyser::initUnary(unary_stmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    return false;
}
bool deviceVarsAnalyser::initIfElse(ifStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtWrap = initWrapNode(stmt, vars);
    ASTNodeWrap *condWrap = initWrapNode(stmt->getCondition(), vars);
    condWrap->usedVars = getVarsExpr(stmt->getCondition());

    bool hasForAll = false;
    hasForAll |= initStatement(stmt->getIfBody(), vars);
    if (stmt->getElseBody() != nullptr)
        hasForAll |= initStatement(stmt->getElseBody(), vars);

    return hasForAll;
}
bool deviceVarsAnalyser::initAssignment(assignment *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    stmtNode->usedVars = getVarsAssignment(stmt);

    return false;
}
bool deviceVarsAnalyser::initDeclaration(declaration *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    stmtNode->outMap.addVariable(stmt->getdeclId(), lattice::NOT_INITIALIZED);

    if (stmt->isInitialized())
        stmtNode->usedVars = getVarsExpr(stmt->getExpressionAssigned());

    return false;
}
bool deviceVarsAnalyser::initForAll(forallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    stmtNode->usedVars = getVarsForAll(stmt);

    stmt->initUsedVariable(stmtNode->usedVars.getVariables());
    gpuUsedVars.merge(stmtNode->usedVars);
    return true;
}
bool deviceVarsAnalyser::initWhile(whileStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    ASTNodeWrap *condNode = initWrapNode(stmt->getCondition(), vars);
    condNode->usedVars = getVarsExpr(stmt->getCondition());

    bool hasForAll = initStatement(stmt->getBody(), vars);
    condNode->hasForAll = hasForAll;

    return hasForAll;
}
bool deviceVarsAnalyser::initDoWhile(dowhileStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    ASTNodeWrap *condNode = initWrapNode(stmt->getCondition(), vars);
    condNode->usedVars = getVarsExpr(stmt->getCondition());

    bool hasForAll = initStatement(stmt->getBody(), vars);
    stmtNode->hasForAll = hasForAll;

    return hasForAll;
}
bool deviceVarsAnalyser::initFixedPoint(fixedPointStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    ASTNodeWrap *condNode = initWrapNode(stmt->getFixedPointId(), vars);

    condNode->usedVars.addVariable(stmt->getFixedPointId(), READ_WRITE);
    for (Identifier *iden : getVarsExpr(stmt->getDependentProp()).getVariables(READ_WRITE))
        condNode->usedVars.addVariable(iden, READ_WRITE);

    bool hasForAll = initStatement(stmt->getBody(), vars);
    stmtNode->hasForAll = hasForAll;

    return hasForAll;
}
// Assuming initialization is done in GPU
bool deviceVarsAnalyser::initProcCall(proc_callStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    proc_callExpr *stmt_expr = stmt->getProcCallExpr();

    for (argument *arg : stmt_expr->getArgList())
    {
        if (arg->isAssignExpr())
        {
            assignment *asgn = arg->getAssignExpr();
            stmtNode->usedVars.merge(getVarsExpr(asgn->getExpr()));
            gpuUsedVars.addVariable(asgn->getId(), WRITE);
        }
    }
    return false;
}
bool deviceVarsAnalyser::initFor(forallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    if (stmt->hasFilterExpr())
        stmtNode->usedVars = getVarsExpr(stmt->getfilterExpr());

    vars.push_back(stmt->getIterator());
    bool hasForAll = initStatement(stmt->getBody(), vars);
    vars.pop_back();

    stmtNode->hasForAll = hasForAll;
    return hasForAll;
}
bool deviceVarsAnalyser::initReduction(reductionCallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    stmtNode->usedVars = getVarsReduction(stmt);

    return false;
}
// Ignore filter expression for now
//  TODO : itrBFS executed in GPU

bool deviceVarsAnalyser::initItrBFS(iterateBFS *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    stmtNode->usedVars = getVarsStatement(stmt->getBody());
    stmtNode->usedVars.removeVariable(stmt->getIteratorNode(), READ_WRITE);

    stmt->initUsedVariable(stmtNode->usedVars.getVariables());
    gpuUsedVars.merge(stmtNode->usedVars);

    if (stmt->getRBFS() != nullptr)
    {
        iterateReverseBFS *revStmt = stmt->getRBFS();
        ASTNodeWrap *revNode = initWrapNode(revStmt, vars);

        usedVariables bVars = getVarsExpr(revStmt->getBFSFilter());
        for (Identifier *iden : vars)
            bVars.removeVariable(iden, READ_WRITE);

        revNode->usedVars = getVarsStatement(revStmt->getBody());

        if (bVars.getVariables().size() > 0) {
            Identifier *itr = bVars.getVariables(READ_WRITE).front();
            revNode->usedVars.removeVariable(itr, READ_WRITE);
        }

        revStmt->initUsedVariable(revNode->usedVars.getVariables());
        gpuUsedVars.merge(revNode->usedVars);
    }

    return true;
}