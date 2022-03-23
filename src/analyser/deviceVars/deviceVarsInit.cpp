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

        /*    case NODE_PROCCALLSTMT:
            // TODO : Add proc call statment
            return inMap;
            case NODE_REDUCTIONCALLSTMT:
            return analyseReduction((reductionCallStmt *)stmt, inMap);
            case NODE_ITRBFS:
            return analyseItrBFS((iterateBFS *)stmt, inMap);

            case NODE_FIXEDPTSTMT:
            return analyseFixedPoint((fixedPointStmt *)stmt, inMap);*/
    }
    return false;
}
bool deviceVarsAnalyser::initUnary(u4nary_stmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    return false;
}
bool deviceVarsAnalyser::initIfElse(ifStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtWrap = initWrapNode(stmt, vars);
    ASTNodeWrap* condWrap = initWrapNode(stmt->getCondition(), vars);
    condWrap->usedVars = getVarsExpr(stmt->getCondition());

    bool hasForAll = false;
    hasForAll |= initStatement(stmt->getIfBody(), vars);
    if(stmt->getElseBody() != nullptr)
        hasForAll |= initStatement(stmt->getElseBody(), vars);

    return hasForAll;
}
bool deviceVarsAnalyser::initAssignment(assignment *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->usedVars = getVarsAssignment(stmt);

    return false;
}
bool deviceVarsAnalyser::initDeclaration(declaration *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->outMap.addVariable(stmt->getdeclId(), lattice::NOT_INITIALIZED);
    
    if(stmt->isInitialized())
        stmtNode->usedVars = getVarsExpr(stmt->getExpressionAssigned());

    return false;
}
bool deviceVarsAnalyser::initForAll(forallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->usedVars = getVarsForAll(stmt);

    gpuUsedVars.merge(stmtNode->usedVars);
    return true;
}
bool deviceVarsAnalyser::initWhile(whileStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    ASTNodeWrap* condNode = initWrapNode(stmt->getCondition(), vars);
    condNode->usedVars = getVarsExpr(stmt->getCondition());

    bool hasForAll = initStatement(stmt->getBody());
    condNode->hasForAll = hasForAll;

    return hasForAll;
}
bool deviceVarsAnalyser::initDoWhile(dowhileStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    ASTNodeWrap* condNode = initWrapNode(stmt->getCondition(), vars);
    condNode->usedVars = getVarsExpr(stmt->getCondition());

    bool hasForAll = initWrapNode(stmt->getBody(), vars);
    stmtNode->hasForAll = hasForAll;

    return hasForAll;
}
bool deviceVarsAnalyser::initFor(forallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    if(stmt->hasFilterExpr())
        stmtNode->usedVars = getVarsExpr(stmt->getfilterExpr());

    vars.push_back(stmt->getIterator());
    bool hasForAll = initStatement(stmt->getBody());
    vars.pop_back();

    stmtNode->hasForAll = hasForAll;
    return hasForAll;
}