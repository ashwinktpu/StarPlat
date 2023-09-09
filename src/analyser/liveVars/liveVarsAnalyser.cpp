#include "liveVarsAnalyser.h"
#include "liveVarsNode.h"

liveVarsAnalyser::liveVarsAnalyser()
{
    nodeList.clear();
}

void liveVarsAnalyser::analyse(list<Function*> funcList)
{
    for(Function* func : funcList)
        analyseFunc(func);
}

void liveVarsAnalyser::analyseFunc(Function* func)
{
    // end block
    liveVarsNode* endNode = new liveVarsNode(func);

    // return search

    analyseStatement(func->getBlockStatement(), endNode);

    return;
}

liveVarsNode* liveVarsAnalyser::analyseStatement(statement* node, liveVarsNode* successor)
{
    switch(node->getTypeofNode())
    {
        case NODE_ASSIGN:
            return analyseAssignment((assignment*)node, successor);
        case NODE_BLOCKSTMT:
            return analyseBlockStatement((blockStatement*)node, successor);
        case NODE_DECL:
            return analyseDeclaration((declaration*)node, successor);
        case NODE_DOWHILESTMT:
            return analyseDoWhile((dowhileStmt*)node, successor);
        case NODE_FIXEDPTSTMT:
            return analyseFixedPoint((fixedPointStmt*)node, successor);
        case NODE_FORALLSTMT:
            return analyseForAll((forallStmt*)node, successor);
        case NODE_IFSTMT:
            return analyseIfStmt((ifStmt*)node, successor);
        case NODE_ITRBFS:
            return analyseIterateBFS((iterateBFS*)node, successor);
        case NODE_PROCCALLSTMT:
            return analyseProcCall((proc_callStmt*)node, successor);
        case NODE_REDUCTIONCALLSTMT:
            return analyseReduction((reductionCallStmt*)node, successor);
        case NODE_RETURN:
            return analyseReturn((returnStmt*)node, successor);
        case NODE_UNARYSTMT:
            return analyseUnary((unary_stmt*)node, successor);
        case NODE_WHILESTMT:
            return analyseWhile((whileStmt*)node, successor);
    }

    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseAssignment(assignment*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseBlockStatement(blockStatement* node, liveVarsNode* successor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    // set out

    std::list<statement*> stmtList = node->returnStatements();

    for(auto stmt = stmtList.rbegin(); stmt != stmtList.rend(); stmt++)
    {
        successor = analyseStatement(*stmt, successor);
    }

    // set in

    return liveVars;
}


liveVarsNode* liveVarsAnalyser::analyseDeclaration(declaration*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseDoWhile(dowhileStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseFixedPoint(fixedPointStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseForAll(forallStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseIfStmt(ifStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseIterateBFS(iterateBFS*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseProcCall(proc_callStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseReduction(reductionCallStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseReturn(returnStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseUnary(unary_stmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::analyseWhile(whileStmt*, liveVarsNode*)
{
    return nullptr;
}