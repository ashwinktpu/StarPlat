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
    return;
}

liveVarsNode* liveVarsAnalyser::initStatement(statement* node, set<liveVarsNode*> predecessor)
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

liveVarsNode* liveVarsAnalyser::initAssignment(assignment* node, set<liveVarsNode*> predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsAssignment(node));
    liveVars->addPredecessors(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initBlockStatement(blockStatement* node, set<liveVarsNode*> predecessor)
{
    list<statement*> stmts = node->returnStatements();

    //handle the case where block is empty??

    auto stmt = stmts.begin();
    liveVarsNode* pred = initStatement((*stmt), predecessor);
    stmt++;

    for(; stmt != stmts.end(); stmt++)
    {
        set<liveVarsNode*> curr;
        curr.insert(pred);
        pred = initStatement((*stmt), curr);
    }
    
    return pred;
}

liveVarsNode* liveVarsAnalyser::initDeclaration(declaration* node, set<liveVarsNode*> predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsDeclaration(node));
    liveVars->addPredecessors(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initDoWhile(dowhileStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initFixedPoint(fixedPointStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initForAll(forallStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initIfStmt(ifStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initIterateBFS(iterateBFS*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initProcCall(proc_callStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initReduction(reductionCallStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initReturn(returnStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initUnary(unary_stmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initWhile(whileStmt*, set<liveVarsNode*>)
{
    return nullptr;
}