#include "blockVarsAnalyser.h"

ASTNodeBlock* blockVarsAnalyser::initBlock(blockStatement* blockStmt, ASTNodeBlock* blockNode)
{
    // Reverse iterate over the statements in the block
    list<statement*> stmts = blockStmt->returnStatements();
    for(auto stmt = stmts.rbegin(); stmt != stmts.rend(); stmt++)
    {
        blockNode = initStatement(*stmt, blockNode);
    }

    return blockNode;
}

ASTNodeBlock* blockVarsAnalyser::initStatement(statement* stmt, ASTNodeBlock* blockNode)
{
    switch(stmt->getTypeofNode())
    {
        case NODE_DECL:
            return initDeclaration((declaration*)stmt, blockNode);
        case NODE_ASSIGN:
            return initAssignment((assignment*)stmt, blockNode);
        case NODE_UNARYSTMT:
            return initUnary((unary_stmt*)stmt, blockNode);
        case NODE_FORALLSTMT:
            return initForAll((forallStmt*)stmt, blockNode);
        case NODE_BLOCKSTMT:
            return initBlock((blockStatement*)stmt, blockNode);
        case NODE_IFSTMT:
            return initIfElse((ifStmt*)stmt, blockNode);
        case NODE_WHILESTMT:
            return initWhile((whileStmt*)stmt, blockNode);
        case NODE_DOWHILESTMT:
            return initDoWhile((dowhileStmt*)stmt, blockNode);
        case NODE_PROCCALLSTMT:
            return initProcCall((proc_callStmt*)stmt, blockNode);
        case NODE_FIXEDPTSTMT:
            return initFixedPoint((fixedPointStmt*)stmt, blockNode);
        case NODE_REDUCTIONCALLSTMT:
            return initReduction((reductionCallStmt*)stmt, blockNode);
        case NODE_ITRBFS:
            return initItrBFS((iterateBFS*)stmt, blockNode);
        case NODE_RETURN:
            return initReturn((returnStmt*)stmt, blockNode);
    }
    return blockNode;
}

ASTNodeBlock* blockVarsAnalyser::initDeclaration(declaration* declStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the declaration
    ASTNodeBlock* declNode = new ASTNodeBlock(declStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsStatement(declStmt);
    declNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    declNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(declStmt, declNode);
    return declNode;
}

ASTNodeBlock* blockVarsAnalyser::initAssignment(assignment* assignStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the assignment
    ASTNodeBlock* assignNode = new ASTNodeBlock(assignStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsStatement(assignStmt);
    assignNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    assignNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(assignStmt, assignNode);
    return assignNode;
}

ASTNodeBlock* blockVarsAnalyser::initUnary(unary_stmt* unaryStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the unary statement
    ASTNodeBlock* unaryNode = new ASTNodeBlock(unaryStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsStatement(unaryStmt);
    unaryNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    unaryNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(unaryStmt, unaryNode);
    return unaryNode;
}

ASTNodeBlock* blockVarsAnalyser::initForAll(forallStmt* forAllStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the forall statement start and end block
    ASTNodeBlock* forAllStartNode = new ASTNodeBlock();
    ASTNodeBlock* forAllEndNode = new ASTNodeBlock();

    // Create a new block node for the forall statement
    ASTNodeBlock* forAllCondNode = new ASTNodeBlock(forAllStmt);

    // Add the used and def variables to use set of the new block for cond
    usedVariables_t usedVars;
    if(forAllStmt->isSourceProcCall())
    {   
        proc_callExpr *expr = forAllStmt->getExtractElementFunc();
        usedVars.merge(getVarsExprProcCall(expr));
    }
    else if(!forAllStmt->isSourceField())
    {
        Identifier *iden = forAllStmt->getSource();
        usedVars.addVariable(iden, USED);
    }
    else
    {
        PropAccess *propId = forAllStmt->getPropSource();
        usedVars.addVariable(propId->getIdentifier1(), USED);
    }

    if (forAllStmt->hasFilterExpr())
        usedVars.merge(getVarsExpr(forAllStmt->getfilterExpr()));

    forAllCondNode->addVars(usedVars);

    // Add the end block as succ of the new cond block
    forAllCondNode->addSucc(forAllEndNode);

    // Add the passed block as succ of the end block
    forAllEndNode->addSucc(blockNode);
    blockNodes.push_back(forAllEndNode);

    // Add this node to the list of block nodes
    addBlockNode(forAllStmt->getBody(), forAllCondNode);

    // Add the forall statement as a succ of the new cond block
    ASTNodeBlock* forAllBodyNode = initStatement(forAllStmt->getBody(), forAllEndNode);
    forAllCondNode->addSucc(forAllBodyNode);

    // Add the cond block as a succ for the forall statement start block
    forAllStartNode->addSucc(forAllCondNode);
    blockNodes.push_back(forAllStartNode);

    // Add the forall statement as a succ of the new block
    forAllCondNode->addSucc(initStatement(forAllStmt->getBody(), forAllCondNode));

    // Map the forall statement's start and end block
    addBlockNode(forAllStmt, forAllStartNode, forAllEndNode);
    return forAllCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initIfElse(ifStmt* ifStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the if statement
    ASTNodeBlock* ifCondNode = new ASTNodeBlock(ifStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsExpr(ifStmt->getCondition());
    ifCondNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    ifCondNode->addSucc(blockNode);

    // Add the if statement as a succ of the new block
    ifCondNode->addSucc(initStatement(ifStmt->getIfBody(), blockNode));

    if(ifStmt->getElseBody() != NULL)
    {
        // Add the else statement as a succ of the new block
        ifCondNode->addSucc(initStatement(ifStmt->getElseBody(), blockNode));
    }

    // Add this node to the list of block nodes
    addBlockNode(ifStmt, ifCondNode);
    return ifCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initWhile(whileStmt* whileStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the do-while statement start and end block
    ASTNodeBlock* whileStartNode = new ASTNodeBlock();
    ASTNodeBlock* whileEndNode = new ASTNodeBlock();

    // Create a new block node for the while statement
    ASTNodeBlock* whileCondNode = new ASTNodeBlock(whileStmt->getCondition());

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsExpr(whileStmt->getCondition());
    whileCondNode->addVars(usedVars);

    // Add the end block as succ of the new block
    whileCondNode->addSucc(whileEndNode);

    // Add the passed block as succ of the end block
    whileEndNode->addSucc(blockNode);
    blockNodes.push_back(whileEndNode);

    // Add the while statement to the list of block nodes
    addBlockNode(whileStmt->getCondition(), whileCondNode);

    // Add the while statement as a succ of the new block
    ASTNodeBlock* whileBodyNode = initStatement(whileStmt->getBody(), whileEndNode);
    whileCondNode->addSucc(whileBodyNode);

    // Add the cond block as a succ of the while statement start block
    whileStartNode->addSucc(whileCondNode);
    blockNodes.push_back(whileStartNode);

    // Map the while statement's start and end block 
    addBlockNode(whileStmt, whileStartNode, whileEndNode);
    return whileStartNode;
}

ASTNodeBlock* blockVarsAnalyser::initDoWhile(dowhileStmt* doWhileStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the do-while statement start and end block
    ASTNodeBlock* doWhileStartNode = new ASTNodeBlock();
    ASTNodeBlock* doWhileEndNode = new ASTNodeBlock();

    // Create a new block node for the do-while condition statement
    ASTNodeBlock* doWhileCondNode = new ASTNodeBlock(doWhileStmt->getCondition());

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsExpr(doWhileStmt->getCondition());
    doWhileCondNode->addVars(usedVars);

    // Add the end block as succ of the new block
    doWhileCondNode->addSucc(doWhileEndNode);

    // Add the passed block as succ of the end block
    doWhileEndNode->addSucc(blockNode);
    blockNodes.push_back(doWhileEndNode);

    // Add this node to the list of block nodes
    addBlockNode(doWhileStmt->getCondition(), doWhileCondNode);

    // Add the do-while statement as a succ of the new block
    ASTNodeBlock* doWhileBodyStartNode = initStatement(doWhileStmt->getBody(), doWhileCondNode);
    doWhileCondNode->addSucc(doWhileBodyStartNode);

    // Add the do-while body start as a succ of the start block
    doWhileStartNode->addSucc(doWhileBodyStartNode);
    blockNodes.push_back(doWhileStartNode);

    // Map the do-while statement's start and end block
    addBlockNode(doWhileStmt, doWhileStartNode, doWhileEndNode);    
    return doWhileStartNode;
}

ASTNodeBlock* blockVarsAnalyser::initProcCall(proc_callStmt* procCallStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the procedure call statement
    ASTNodeBlock* procCallNode = new ASTNodeBlock(procCallStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsStatement(procCallStmt);
    procCallNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    procCallNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(procCallStmt, procCallNode);
    return procCallNode;
}

ASTNodeBlock* blockVarsAnalyser::initFixedPoint(fixedPointStmt* fixedPointStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for start and end block of the fixed point statement
    ASTNodeBlock* fixedPointStartNode = new ASTNodeBlock();
    ASTNodeBlock* fixedPointEndNode = new ASTNodeBlock();

    // Create a new block node for the fixed point statement
    ASTNodeBlock* fixedPointCondNode = new ASTNodeBlock(fixedPointStmt->getFixedPointId());

    // Add the used and def variables to use set of the new block
    fixedPointCondNode->addUse(fixedPointStmt->getFixedPointId());

    // Add the passed block as succ of the new block
    fixedPointCondNode->addSucc(fixedPointEndNode);

    // Add the end block to the list of block nodes
    fixedPointEndNode->addSucc(blockNode);
    blockNodes.push_back(fixedPointEndNode);

    // Add the fixed point statement as a succ of the new block
    fixedPointCondNode->addSucc(initStatement(fixedPointStmt->getBody(), fixedPointCondNode));
    addBlockNode(fixedPointStmt->getFixedPointId(), fixedPointCondNode);

    // Add the fixed point statement as a succ of the start block
    fixedPointStartNode->addSucc(fixedPointCondNode);
    blockNodes.push_back(fixedPointStartNode);

    // Map the fixed point statement's start and end block
    addBlockNode(fixedPointStmt, fixedPointStartNode, fixedPointEndNode);

    return fixedPointStartNode;
}

ASTNodeBlock* blockVarsAnalyser::initReduction(reductionCallStmt* reductionCallStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the reduction statement
    ASTNodeBlock* reductionNode = new ASTNodeBlock(reductionCallStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsStatement(reductionCallStmt);
    reductionNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    reductionNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(reductionCallStmt, reductionNode);
    return reductionNode;
}

/* TODO: This function is not properly implemented */
ASTNodeBlock* blockVarsAnalyser::initItrBFS(iterateBFS* iterateBFS, ASTNodeBlock* blockNode)
{
    // Create a new block node for the iterateBFS statement
    ASTNodeBlock* iterateBFSNode = new ASTNodeBlock(iterateBFS);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsStatement(iterateBFS);
    iterateBFSNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    iterateBFSNode->addSucc(blockNode);

    // Add the iterateBFS statement as a succ of the new block
    iterateBFSNode->addSucc(initStatement(iterateBFS->getBody(), blockNode));

    // Add this node to the list of block nodes
    addBlockNode(iterateBFS, iterateBFSNode);
    return iterateBFSNode;
}

ASTNodeBlock* blockVarsAnalyser::initReturn(returnStmt* returnStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the return statement
    ASTNodeBlock* returnNode = new ASTNodeBlock(returnStmt);

    // Add the used and def variables to use set of the new block
    usedVariables_t usedVars = getVarsExpr(returnStmt->getReturnExpression());
    returnNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    returnNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(returnStmt, returnNode);
    return returnNode;
}