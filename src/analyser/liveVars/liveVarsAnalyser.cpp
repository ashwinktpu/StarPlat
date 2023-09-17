#include "liveVarsAnalyser.h"
#include "liveVarsNode.h"

liveVarsAnalyser::liveVarsAnalyser()
{
    nodeList.clear();
}

void liveVarsAnalyser::analyse(list<Function*> funcList)
{
    for(Function* func : funcList)
        initFunc(func);
}

void liveVarsAnalyser::initFunc(Function* func)
{
    // end block
    liveVarsNode* endNode = new liveVarsNode(func);

    // return search

    initStatement(func->getBlockStatement(), endNode);

    return;
}

liveVarsNode* liveVarsAnalyser::initStatement(statement* node, liveVarsNode* successor)
{
    switch(node->getTypeofNode())
    {
        case NODE_ASSIGN:
            return initAssignment((assignment*)node, successor);
        case NODE_BLOCKSTMT:
            return initBlockStatement((blockStatement*)node, successor);
        case NODE_DECL:
            return initDeclaration((declaration*)node, successor);
        case NODE_DOWHILESTMT:
            return initDoWhile((dowhileStmt*)node, successor);
        case NODE_FIXEDPTSTMT:
            return initFixedPoint((fixedPointStmt*)node, successor);
        case NODE_FORALLSTMT:
            return initForAll((forallStmt*)node, successor);
        case NODE_IFSTMT:
            return initIfStmt((ifStmt*)node, successor);
        case NODE_ITRBFS:
            return initIterateBFS((iterateBFS*)node, successor);
        case NODE_PROCCALLSTMT:
            return initProcCall((proc_callStmt*)node, successor);
        case NODE_REDUCTIONCALLSTMT:
            return initReduction((reductionCallStmt*)node, successor);
        case NODE_RETURN:
            return initReturn((returnStmt*)node, successor);
        case NODE_UNARYSTMT:
            return initUnary((unary_stmt*)node, successor);
        case NODE_WHILESTMT:
            return initWhile((whileStmt*)node, successor);
    }

    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initAssignment(assignment* node, liveVarsNode* successor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initBlockStatement(blockStatement* node, liveVarsNode* successor)
{
    return nullptr;
}


liveVarsNode* liveVarsAnalyser::initDeclaration(declaration*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initDoWhile(dowhileStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initFixedPoint(fixedPointStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initForAll(forallStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initIfStmt(ifStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initIterateBFS(iterateBFS*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initProcCall(proc_callStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initReduction(reductionCallStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initReturn(returnStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initUnary(unary_stmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initWhile(whileStmt*, liveVarsNode*)
{
    return nullptr;
}