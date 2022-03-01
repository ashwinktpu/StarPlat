#include "deviceVarsAnalyser.h"

bool deviceVarsAnalyser::initBlock(blockStatement *blockStmt, list<Identifier *> &vars)
{
    ASTNodeWrap *blockNode = initWrapNode(blockStatement, vars);
    blockNode->inMap.setType(lattice::CPU_Preferenced);
    blockNode->outMap.setType(lattice::CPU_Preferenced);

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
        return initWhile((whileStmt *)stmt, inMap);
    case NODE_DOWHILESTMT:
        return initDoWhile((dowhileStmt *)stmt, inMap);

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
    stmtNode->inMap.setType(lattice::CPU_Preferenced);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    return false;
}
bool deviceVarsAnalyser::initIfElse(ifStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtWrap = initWrapNode(stmt, vars);
    stmtWrap->inMap.setType(lattice::CPU_Preferenced);
    stmtWrap->outMap.setType(lattice::GPU_Preferenced);

    ASTNodeWrap* condWrap = initWrapNode(stmt, vars);
    stmtWrap->inMap.setType(lattice::CPU_Preferenced);
    stmtWrap->outMap.setType(lattice::GPU_Preferenced);

    bool hasForAll = false;
    hasForAll |= initStatement(stmt->getIfBody());
    if(stmt->getElseBody() != nullptr)
        hasForAll |= initStatement(stmt->getElseBody());

    return hasForAll;
}
bool deviceVarsAnalyser::initAssignment(assignment *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->inMap.setType(lattice::CPU_Preferenced);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    return false;
}
bool deviceVarsAnalyser::initDeclaration(declaration *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->inMap.setType(lattice::CPU_Preferenced);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    stmtNode->outMap.addVariable(stmt->getdeclId(), lattice::NOT_INITIALIZED);
    return false;
}
bool deviceVarsAnalyser::initForAll(forallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->inMap.setType(lattice::CPU_Preferenced);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    return true;
}
bool deviceVarsAnalyser::initWhile(whileStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->inMap.setType(lattice::CPU_Preferenced);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    ASTNodeWrap* condNode = initWrapNode(stmt->getCondition(), vars);
    bool hasForAll = initStatement(stmt->getBody());

    if(hasForAll)
        condNode->inMap.setType(lattice::GPU_Preferenced);
    else
        condNode->inMap.setType(lattice::CPU_Preferenced);
    condNode->outMap.setType(lattice::CPU_Preferenced);

    return hasForAll;
}
bool deviceVarsAnalyser::initDoWhile(dowhileStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap* stmtNode = initWrapNode(stmt, vars);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    ASTNodeWrap* condNode = initWrapNode(stmt->getCondition(), vars);
    condNode->inMap.setType(lattice::CPU_Preferenced);
    condNode->outMap.setType(lattice::CPU_Preferenced);

    bool hasForAll = initWrapNode(stmt->getBody(), vars);
    if(hasForAll)
        stmtNode->inMap.setType(lattice::GPU_Preferenced);
    else
        stmtNode->inMap.setType(lattice::CPU_Preferenced);

    return hasForAll;
}
bool deviceVarsAnalyser::initFor(forallStmt *stmt, list<Identifier *> &vars)
{
    ASTNodeWrap *stmtNode = initWrapNode(stmt, vars);
    stmtNode->outMap.setType(lattice::CPU_Preferenced);

    if(stmt->hasFilterExpr())
    {
        Expression* filterExpr = stmt->getfilterExpr();
        ASTNodeWrap *condNode = initWrapNode(filterExpr, vars);
        condNode->inMap.setType(lattice::CPU_Preferenced);
        condNode->outMap.setType(lattice::CPU_Preferenced);
    }

    vars.push_back(stmt->getIterator());
    bool hasForAll = initStatement(stmt->getBody());
    vars.pop_back();

    if(hasForAll)
        stmtNode->inMap.setType(lattice::GPU_Preferenced);
    else
        stmtNode->inMap.setType(lattice::CPU_Preferenced);

    return hasForAll;
}