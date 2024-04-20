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
    std::cout<<"Start liveness analysis"<<std::endl;
    
    currReturnNodes.clear();

    liveVarsNode* funcStart = new liveVarsNode();
    liveVarsNode* funcEnd = new liveVarsNode();
    for(formalParam* param : func->getParamList())
    {
        funcStart->addDef(param->getIdentifier());

        // sets all input parameters as live out
        // if not required remove this
        funcEnd->addOut(param->getIdentifier());
    }

    liveVarsNode* blockEnd = initStatement(func->getBlockStatement(), funcStart);

    for(liveVarsNode* node : currReturnNodes)
    {
        funcEnd->addPredecessor(node);
    }

    funcEnd->addPredecessor(blockEnd);

    analysecfg(funcEnd);
    // the funcEnd will not contains any live variables and is just a placeholder
    // analysis on individual end block needs to be called as follows
    for(liveVarsNode* node : funcEnd->getPredecessors())
        analysecfg(node);

    std::cout<<"Liveness graph initialized"<<std::endl<<std::endl;

    printLiveVarsNode(func->getBlockStatement());

    std::cout<<"End liveness analysis"<<std::endl;

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
        predecessor = initStatement((*stmt), predecessor);
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

liveVarsNode* liveVarsAnalyser::initDoWhile(dowhileStmt* node, liveVarsNode* predecessor)
{
    Expression* condNode = node->getCondition();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(condNode);
    condLive->addVars(getVarsExpr(condNode));

    liveVarsNode* doWhileBegin = new liveVarsNode();
    doWhileBegin->addPredecessor(predecessor);
    doWhileBegin->addPredecessor(condLive);

    liveVarsNode* bodyEnd = initStatement(bodyNode, doWhileBegin);

    condLive->addPredecessor(bodyEnd);

    return condLive;
}

liveVarsNode* liveVarsAnalyser::initFixedPoint(fixedPointStmt* node, liveVarsNode* predecessor)
{
    Identifier* fixedPointNode = node->getFixedPointId();
    Expression* fixedPointExpr = node->getDependentProp();
    statement* bodyNode = node->getBody();

    liveVarsNode* condLive = new liveVarsNode(fixedPointNode);
    condLive->addUse(fixedPointNode);
    condLive->addVars(getVarsExpr(fixedPointExpr));

    liveVarsNode* fixedPointBegin = new liveVarsNode();
    fixedPointBegin->addPredecessor(predecessor);
    fixedPointBegin->addPredecessor(condLive);

    liveVarsNode* bodyEnd = initStatement(bodyNode, fixedPointBegin);

    condLive->addPredecessor(bodyEnd);

    return condLive;
}

liveVarsNode* liveVarsAnalyser::initForAll(forallStmt* node, liveVarsNode* predecessor)
{
    statement* forAllBody = node->getBody();

    liveVarsNode* condNode = new liveVarsNode(node->getIterator());

    usedVariables condVars;
    if(node->isSourceProcCall())
    {
        condVars.merge(getVarsExprProcCall(node->getExtractElementFunc()));
        condVars.addVariable(node->getSourceGraph(), READ);
    }
    else if(!node->isSourceField())
    {
        condVars.addVariable(node->getSource(), READ);
    }
    else
    {
        condVars.addVariable(node->getPropSource()->getIdentifier1(), READ);
    }

    if(node->hasFilterExpr())
        condVars.merge(getVarsExpr(node->getfilterExpr()));
    
    condNode->addVars(condVars);
    condNode->addDef(node->getIterator());
    condNode->addPredecessor(predecessor);

    liveVarsNode* bodyNode = initStatement(forAllBody, condNode);

    if(!node->isForall())
    {
        condNode->addPredecessor(bodyNode);
    }

    return bodyNode;
}

liveVarsNode* liveVarsAnalyser::initIfStmt(ifStmt* node, liveVarsNode* predecessor)
{
    Expression* condNode = node->getCondition();
    statement* ifBody = node->getIfBody();
    statement* elseBody = node->getElseBody();

    liveVarsNode* condLive = new liveVarsNode(condNode);
    condLive->addVars(getVarsExpr(condNode));
    condLive->addPredecessor(predecessor);

    liveVarsNode* ifLive = initStatement(ifBody, condLive);

    liveVarsNode* ifEnd = new liveVarsNode();
    ifEnd->addPredecessor(ifLive);
    ifEnd->addPredecessor(condLive);

    if(elseBody != NULL)
    {
        liveVarsNode* elseLive = initStatement(elseBody, condLive);
        ifEnd->addPredecessor(elseLive);
    }
    
    return ifEnd;
}

liveVarsNode* liveVarsAnalyser::initIterateBFS(iterateBFS* node, liveVarsNode* predecessor)
{
    statement* itrBody = node->getBody();
    
    liveVarsNode* condLive = new liveVarsNode(node->getIteratorNode());
    condLive->addPredecessor(predecessor);
    condLive->addDef(node->getIteratorNode());
    condLive->addUse(node->getRootNode());
    condLive->addUse(node->getGraphCandidate());

    usedVariables procLive;
    procLive.merge(getVarsExprProcCall(node->getMethodCall()));
    condLive->addVars(procLive);

    predecessor = condLive;

    Expression* expr = node->getBFSFilter();
    if(expr != nullptr)
    {
        liveVarsNode* exprLive  = new liveVarsNode(expr);
        exprLive->addVars(getVarsExpr(expr));
        exprLive->addPredecessor(predecessor);
        predecessor = exprLive;
    }

    liveVarsNode* itrBodyLive = initStatement(itrBody, predecessor);

    if(node->getRBFS() == nullptr)
        return itrBodyLive;
    else
        return initIterateRBFS(node->getRBFS(), itrBodyLive);
}

liveVarsNode* liveVarsAnalyser::initIterateRBFS(iterateReverseBFS* node, liveVarsNode* predecessor)
{
    Expression* expr = node->getBFSFilter();
    if(expr != nullptr)
    {
        liveVarsNode* exprLive  = new liveVarsNode(expr);
        exprLive->addVars(getVarsExpr(expr));
        exprLive->addPredecessor(predecessor);
        predecessor = exprLive;
    }

    statement* itrBody = node->getBody();

    liveVarsNode* itrBodyLive = initStatement(itrBody, predecessor);

    return itrBodyLive;
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

    // needs to be updated if the expression involves assignments
    liveVars->addOut(liveVars->getUse());

    currReturnNodes.push_back(liveVars);

    return nullptr;
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

    liveVarsNode* condLive = new liveVarsNode(condNode);
    condLive->addVars(getVarsExpr(condNode));
    condLive->addPredecessor(predecessor);

    liveVarsNode* bodyEnd = initStatement(bodyNode, condLive);

    liveVarsNode* whileEnd = new liveVarsNode();
    whileEnd->addPredecessor(condLive);
    whileEnd->addPredecessor(bodyEnd);

    return whileEnd;
}

void liveVarsAnalyser::analysecfg(liveVarsNode* endNode)
{
    queue<liveVarsNode*> nodeQueue;
    nodeQueue.push(endNode);

    while(!nodeQueue.empty())
    {
        liveVarsNode* temp = nodeQueue.front();
        nodeQueue.pop();
        if(analyseNode(temp))
            for(liveVarsNode* pred : temp->getPredecessors())
                nodeQueue.push(pred);
    }
    
    return;
}

bool liveVarsAnalyser::analyseNode(liveVarsNode* node)
{
    bool update = false;

    set<TableEntry*> newIn = node->getOut();
    for(TableEntry* id : node->getDef())
    {
        auto itr = newIn.find(id);
        if(itr != newIn.end())
            newIn.erase(itr);
    }
    for(TableEntry* id : node->getUse())
        newIn.insert(id);
    
    if(node->getIn() != newIn)
    {
        update = true;
        node->setIn(newIn);
    }

    if(update)
        for(liveVarsNode* pred : node->getPredecessors())
            pred->addOut(node->getIn());

    return update;
}

void liveVarsAnalyser::printLiveVarsNode(ASTNode* stmt)
{
    if(stmt == nullptr)
    {
        std::cout<<"Error: null ASTNode*"<<std::endl;
    }
    switch(stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:
        {
            list<statement*> stmtlist = ((blockStatement*)stmt)->returnStatements(); 
            for(auto s = stmtlist.begin(); s != stmtlist.end(); s++)
                printLiveVarsNode(*s);
            break;
        }
        case NODE_DOWHILESTMT:
        {
            printLiveVarsNode(((dowhileStmt*)stmt)->getBody());
            printLiveVarsNode(((dowhileStmt*)stmt)->getCondition());
            break;
        }
        case NODE_FIXEDPTSTMT:
        {
            printLiveVarsNode(((fixedPointStmt*)stmt)->getFixedPointId());
            printLiveVarsNode(((fixedPointStmt*)stmt)->getBody());
            break;
        }
        case NODE_FORALLSTMT:
        {
            printLiveVarsNode(((forallStmt*)stmt)->getIterator());
            printLiveVarsNode(((forallStmt*)stmt)->getBody());
            break;
        }
        case NODE_IFSTMT:
        {
            printLiveVarsNode(((ifStmt*)stmt)->getCondition());
            printLiveVarsNode(((ifStmt*)stmt)->getIfBody());
            if(((ifStmt*)stmt)->getElseBody())
                printLiveVarsNode(((ifStmt*)stmt)->getElseBody());
            break;
        }
        case NODE_ITRBFS:
        {
            printLiveVarsNode(((iterateBFS*)stmt)->getIteratorNode());
            Expression* filter = ((iterateBFS*)stmt)->getBFSFilter();
            if(filter != nullptr)
                printLiveVarsNode(filter);
            printLiveVarsNode(((iterateBFS*)stmt)->getBody());
            iterateReverseBFS* reverse = ((iterateBFS*)stmt)->getRBFS();
            if(reverse != nullptr)
                printLiveVarsNode(reverse);
            break;
        }
        case NODE_ITRRBFS:
        {
            Expression* filter = ((iterateReverseBFS*)stmt)->getBFSFilter();
            if(filter != nullptr)
                printLiveVarsNode(filter);
            printLiveVarsNode(((iterateReverseBFS*)stmt)->getBody());
            break;
        }
        case NODE_WHILESTMT:
        {
            printLiveVarsNode(((whileStmt*)stmt)->getCondition());
            printLiveVarsNode(((whileStmt*)stmt)->getBody());
            break;
        }
        default:
        {
            liveVarsNode* node = stmt->getLiveVarsNode();
            if(node == nullptr)
            {
                std::cout<<"Error: LiveVarsNode* null"<<std::endl;
                break;
            }
            std::cout<<"Use : ";
            for(TableEntry* t : node->getUse())
                std::cout<<t->getId()->getIdentifier()<<", ";
            std::cout<<std::endl;
            std::cout<<"Def : ";
            for(TableEntry* t : node->getDef())
                std::cout<<t->getId()->getIdentifier()<<", ";
            std::cout<<std::endl;
            std::cout<<"In  : ";
            for(TableEntry* t : node->getIn())
                std::cout<<t->getId()->getIdentifier()<<", ";
            std::cout<<std::endl;
            std::cout<<"Out : ";
            for(TableEntry* t : node->getOut())
                std::cout<<t->getId()->getIdentifier()<<", ";
            std::cout<<std::endl<<std::endl;
            break;
        }
    }
    return;
}