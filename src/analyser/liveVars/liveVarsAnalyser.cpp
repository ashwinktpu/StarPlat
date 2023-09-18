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

liveVarsNode* liveVarsAnalyser::initDoWhile(dowhileStmt* node, set<liveVarsNode*> predecessor)
{
    Expression* condNode = node->getCondition();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(node);
    condLive->addVars(getVarsExpr(condNode));

    predecessor.insert(condLive);
    liveVarsNode* bodyEnd = initStatement(bodyNode, predecessor);

    condLive->addPredecessor(bodyEnd);

    return condLive;
}

liveVarsNode* liveVarsAnalyser::initFixedPoint(fixedPointStmt* node, set<liveVarsNode*> predecessor)
{
    Identifier* fixedPointNode = node->getFixedPointId();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(fixedPointNode);
    condLive->addUse(fixedPointNode);
    condLive->addPredecessors(predecessor);

    set<liveVarsNode*> condSet;
    condSet.insert(condLive);

    liveVarsNode* bodyEnd = initStatement(bodyNode, condSet);

    condLive->addPredecessor(bodyEnd);

    return bodyEnd;
}

liveVarsNode* liveVarsAnalyser::initForAll(forallStmt*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initIfStmt(ifStmt* node, set<liveVarsNode*> predecessor)
{
    Expression* condNode = node->getCondition();
    statement* ifBody = node->getIfBody();
    statement* elseBody = node->getElseBody();

    liveVarsNode* condLive = new liveVarsNode(condNode);
    condLive->addVars(getVarsExpr(condNode));
    condLive->addPredecessors(predecessor);

    liveVarsNode* ifEnd = new liveVarsNode();

    set<liveVarsNode*> condSet;
    condSet.insert(condLive);

    liveVarsNode* ifLive = initStatement(ifBody, condSet);
    ifEnd->addPredecessor(ifLive);

    if(elseBody != NULL)
    {
        liveVarsNode* elseLive = initStatement(elseBody, condSet);
        ifEnd->addPredecessor(elseLive);
    }
    
    return ifEnd;
}

liveVarsNode* liveVarsAnalyser::initIterateBFS(iterateBFS*, set<liveVarsNode*>)
{
    return nullptr;
}

liveVarsNode* liveVarsAnalyser::initProcCall(proc_callStmt* node, set<liveVarsNode*> predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsProcCall(node));
    liveVars->addPredecessors(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initReduction(reductionCallStmt* node, set<liveVarsNode*> predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsReduction(node));
    liveVars->addPredecessors(predecessor);
    
    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initReturn(returnStmt* node, set<liveVarsNode*> predecessor)
{
    Expression* returnExpr = node->getReturnExpression();

    liveVarsNode* liveVars = new liveVarsNode(node);
    liveVars->addVars(getVarsExpr(returnExpr));
    liveVars->addPredecessors(predecessor);

    currReturnNodes.push_back(liveVars);

    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initUnary(unary_stmt* node, set<liveVarsNode*> predecessor)
{
    liveVarsNode* liveVars = new liveVarsNode(node);

    liveVars->addVars(getVarsUnary(node));
    liveVars->addPredecessors(predecessor);

    return liveVars;
}

liveVarsNode* liveVarsAnalyser::initWhile(whileStmt* node, set<liveVarsNode*> predecessor)
{
    Expression* condNode = node->getCondition();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(node);
    condLive->addVars(getVarsExpr(condNode));
    condLive->addPredecessors(predecessor);

    set<liveVarsNode*> condSet;
    condSet.insert(condLive);

    liveVarsNode* bodyEnd = initStatement(bodyNode, condSet);

    condLive->addPredecessor(bodyEnd);

    return condLive;
}