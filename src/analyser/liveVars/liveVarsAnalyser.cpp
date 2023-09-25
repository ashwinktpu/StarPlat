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
    currReturnNodes.clear();
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

    //handle the case where block is empty??

    auto stmt = stmts.begin();
    liveVarsNode* pred = initStatement((*stmt), predecessor);
    stmt++;

    for(; stmt != stmts.end(); stmt++)
    {
        pred = initStatement((*stmt), pred);
    }
    
    return pred;
}

liveVarsNode* liveVarsAnalyser::initDeclaration(declaration* node, liveVarsNode* predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsDeclaration(node));
    liveVars->addPredecessor(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initDoWhile(dowhileStmt* node, liveVarsNode* predecessor)
{
    Expression* condNode = node->getCondition();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(node);
    condLive->addVars(getVarsExpr(condNode));

    liveVarsNode* dummy = new liveVarsNode();
    dummy->addPredecessor(predecessor);
    dummy->addPredecessor(condLive);

    liveVarsNode* bodyEnd = initStatement(bodyNode, dummy);

    condLive->addPredecessor(bodyEnd);

    return condLive;
}

liveVarsNode* liveVarsAnalyser::initFixedPoint(fixedPointStmt* node, liveVarsNode* predecessor)
{
    Identifier* fixedPointNode = node->getFixedPointId();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(fixedPointNode);
    condLive->addUse(fixedPointNode);
    condLive->addPredecessor(predecessor);

    liveVarsNode* bodyEnd = initStatement(bodyNode, condLive);

    condLive->addPredecessor(bodyEnd);

    return bodyEnd;
}

liveVarsNode* liveVarsAnalyser::initForAll(forallStmt*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initIfStmt(ifStmt* node, liveVarsNode* predecessor)
{
    Expression* condNode = node->getCondition();
    statement* ifBody = node->getIfBody();
    statement* elseBody = node->getElseBody();

    liveVarsNode* condLive = new liveVarsNode(condNode);
    condLive->addVars(getVarsExpr(condNode));
    condLive->addPredecessor(predecessor);

    liveVarsNode* ifEnd = new liveVarsNode();

    liveVarsNode* ifLive = initStatement(ifBody, condLive);
    ifEnd->addPredecessor(ifLive);

    if(elseBody != NULL)
    {
        liveVarsNode* elseLive = initStatement(elseBody, condLive);
        ifEnd->addPredecessor(elseLive);
    }
    
    return ifEnd;
}

liveVarsNode* liveVarsAnalyser::initIterateBFS(iterateBFS*, liveVarsNode*)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initProcCall(proc_callStmt* node, liveVarsNode* predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsProcCall(node));
    liveVars->addPredecessor(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initReduction(reductionCallStmt* node, liveVarsNode* predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsReduction(node));
    liveVars->addPredecessor(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initReturn(returnStmt* node, liveVarsNode* predecessor)
{
    Expression* returnExpr = node->getReturnExpression();

    liveVarsNode* liveVars = new liveVarsNode(node);
    liveVars->addVars(getVarsExpr(returnExpr));
    liveVars->addPredecessor(predecessor);

    currReturnNodes.push_back(liveVars);

    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initUnary(unary_stmt* node, liveVarsNode* predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsUnary(node));
    liveVars->addPredecessor(predecessor);

    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initWhile(whileStmt* node, liveVarsNode* predecessor)
{
    Expression* condNode = node->getCondition();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(node);
    condLive->addVars(getVarsExpr(condNode));
    condLive->addPredecessor(predecessor);

    liveVarsNode* bodyEnd = initStatement(bodyNode, condLive);

    condLive->addPredecessor(bodyEnd);

    return condLive;
}