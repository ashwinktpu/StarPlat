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

liveVarsNode* liveVarsAnalyser::initStatement(statement* node, liveVarsNode* predecessor)
{
    switch(node->getTypeofNode())
    {
        case NODE_ASSIGN:
            return initAssignment((assignment*)node, predecessor);
        case NODE_BLOCKSTMT:
            return initBlockStatement((blockStatement*)node, predecessor);
        case NODE_DECL:
            return initDeclaration((declaration*)node, predecessor);
        case NODE_DOWHILESTMT:
            return initDoWhile((dowhileStmt*)node, predecessor);
        case NODE_FIXEDPTSTMT:
            return initFixedPoint((fixedPointStmt*)node, predecessor);
        case NODE_FORALLSTMT:
            return initForAll((forallStmt*)node, predecessor);
        case NODE_IFSTMT:
            return initIfStmt((ifStmt*)node, predecessor);
        case NODE_ITRBFS:
            return initIterateBFS((iterateBFS*)node, predecessor);
        case NODE_PROCCALLSTMT:
            return initProcCall((proc_callStmt*)node, predecessor);
        case NODE_REDUCTIONCALLSTMT:
            return initReduction((reductionCallStmt*)node, predecessor);
        case NODE_RETURN:
            return initReturn((returnStmt*)node, predecessor);
        case NODE_UNARYSTMT:
            return initUnary((unary_stmt*)node, predecessor);
        case NODE_WHILESTMT:
            return initWhile((whileStmt*)node, predecessor);
    }

    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initAssignment(assignment* node, liveVarsNode* predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsAssignment(node));
    liveVars->addPredecessor(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initBlockStatement(blockStatement* node, liveVarsNode* predecessor)
{
    list<statement*> stmts = node->returnStatements();
    for(auto stmt = stmts.begin(); stmt != stmts.end(); stmt++)
    {
        liveVarsNode* predecessor = initStatement((*stmt), predecessor);
    }
    
    return predecessor;
}

liveVarsNode* liveVarsAnalyser::initDeclaration(declaration* node, liveVarsNode* predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsDeclaration(node));
    liveVars->addPredecessor(predecessor);
    
    return liveVars;
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