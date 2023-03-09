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
    printf("Initializing %p ", stmt);
    toString(stmt->getTypeofNode());
    printf("\n");

    switch(stmt->getTypeofNode())
    {
        case NODE_DECL:
            return initDeclaration((declaration*)stmt, blockNode);
        case NODE_ASSIGN:
            return initAssignment((assignment*)stmt, blockNode);
        case NODE_UNARYSTMT:
            return initUnary((unary_stmt*)stmt, blockNode);
        case NODE_FORALLSTMT:
        {
            forallStmt* forStmt = (forallStmt*)stmt;
            if(forStmt->isForall())
                return initForAll((forallStmt*)stmt, blockNode);
            else
                return initFor((forallStmt*)stmt, blockNode);
        }
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
    usedVariables usedVars = getVarsStatement(declStmt);
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
    usedVariables usedVars = getVarsStatement(assignStmt);
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
    usedVariables usedVars = getVarsStatement(unaryStmt);
    unaryNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    unaryNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(unaryStmt, unaryNode);
    return unaryNode;
}

ASTNodeBlock* blockVarsAnalyser::initForAll(forallStmt* forAllStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the forall statement
    ASTNodeBlock* forAllCondNode = new ASTNodeBlock(forAllStmt);

    // Add the used and def variables to use set of the new block for cond
    usedVariables usedVars;
    if(forAllStmt->isSourceProcCall())
    {
        proc_callExpr *expr = forAllStmt->getExtractElementFunc();
        usedVars.merge(getVarsExprProcCall(expr));
    }
    else if(!forAllStmt->isSourceField())
    {
        Identifier *iden = forAllStmt->getSource();
        usedVars.addVariable(iden, READ);
    }
    else
    {
        PropAccess *propId = forAllStmt->getPropSource();
        usedVars.addVariable(propId->getIdentifier1(), READ);
    }

    if (forAllStmt->hasFilterExpr())
        usedVars.merge(getVarsExpr(forAllStmt->getfilterExpr()));

    forAllCondNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    forAllCondNode->addSucc(blockNode);

    // Add the forall statement as a succ of the new block
    forAllCondNode->addSucc(initStatement(forAllStmt->getBody(), forAllCondNode));

    // Add this node to the list of block nodes
    addBlockNode(forAllStmt, forAllCondNode);
    return forAllCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initFor(forallStmt* forStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the for statement
    ASTNodeBlock* forCondNode = new ASTNodeBlock(forStmt);

    // Add the used and def variables to use set of the new block for cond
    usedVariables usedVars;
    if(forStmt->isSourceProcCall())
    {
        proc_callExpr *expr = forStmt->getExtractElementFunc();
        usedVars.merge(getVarsExprProcCall(expr));
    }
    else if(!forStmt->isSourceField())
    {
        Identifier *iden = forStmt->getSource();
        usedVars.addVariable(iden, READ);
    }
    else
    {
        PropAccess *propId = forStmt->getPropSource();
        usedVars.addVariable(propId->getIdentifier1(), READ);
    }

    if (forStmt->hasFilterExpr())
        usedVars.merge(getVarsExpr(forStmt->getfilterExpr()));

    forCondNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    forCondNode->addSucc(blockNode);

    // Add the for statement as a succ of the new block
    forCondNode->addSucc(initStatement(forStmt->getBody(), forCondNode));

    // Add this node to the list of block nodes
    addBlockNode(forStmt, forCondNode);
    return forCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initIfElse(ifStmt* ifStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the if statement
    ASTNodeBlock* ifCondNode = new ASTNodeBlock(ifStmt);

    // Add the used and def variables to use set of the new block
    usedVariables usedVars = getVarsExpr(ifStmt->getCondition());
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
    // Create a new block node for the while statement
    ASTNodeBlock* whileCondNode = new ASTNodeBlock(whileStmt);

    // Add the used and def variables to use set of the new block
    usedVariables usedVars = getVarsExpr(whileStmt->getCondition());
    whileCondNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    whileCondNode->addSucc(blockNode);

    // Add the while statement as a succ of the new block
    whileCondNode->addSucc(initStatement(whileStmt->getBody(), whileCondNode));

    // Add this node to the list of block nodes
    addBlockNode(whileStmt, whileCondNode);
    return whileCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initDoWhile(dowhileStmt* doWhileStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the do-while statement
    ASTNodeBlock* doWhileCondNode = new ASTNodeBlock(doWhileStmt);

    // Add the used and def variables to use set of the new block
    usedVariables usedVars = getVarsExpr(doWhileStmt->getCondition());
    doWhileCondNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    doWhileCondNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(doWhileStmt, doWhileCondNode);

    // Add the do-while statement as a succ of the new block
    doWhileCondNode->addSucc(initStatement(doWhileStmt->getBody(), doWhileCondNode));

    return doWhileCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initProcCall(proc_callStmt* procCallStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the procedure call statement
    ASTNodeBlock* procCallNode = new ASTNodeBlock(procCallStmt);

    // Add the used and def variables to use set of the new block
    usedVariables usedVars = getVarsStatement(procCallStmt);
    procCallNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    procCallNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(procCallStmt, procCallNode);
    return procCallNode;
}

ASTNodeBlock* blockVarsAnalyser::initFixedPoint(fixedPointStmt* fixedPointStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the fixed point statement
    ASTNodeBlock* fixedPointCondNode = new ASTNodeBlock(fixedPointStmt);

    // Add the used and def variables to use set of the new block
    fixedPointCondNode->addUse(fixedPointStmt->getFixedPointId());

    // Add the passed block as succ of the new block
    fixedPointCondNode->addSucc(blockNode);

    // Add the fixed point statement as a succ of the new block
    fixedPointCondNode->addSucc(initStatement(fixedPointStmt->getBody(), fixedPointCondNode));

    // Add this node to the list of block nodes
    addBlockNode(fixedPointStmt, fixedPointCondNode);
    return fixedPointCondNode;
}

ASTNodeBlock* blockVarsAnalyser::initReduction(reductionCallStmt* reductionCallStmt, ASTNodeBlock* blockNode)
{
    // Create a new block node for the reduction statement
    ASTNodeBlock* reductionNode = new ASTNodeBlock(reductionCallStmt);

    // Add the used and def variables to use set of the new block
    usedVariables usedVars = getVarsStatement(reductionCallStmt);
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
    usedVariables usedVars = getVarsStatement(iterateBFS);
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
    usedVariables usedVars = getVarsExpr(returnStmt->getReturnExpression());
    returnNode->addVars(usedVars);

    // Add the passed block as succ of the new block
    returnNode->addSucc(blockNode);

    // Add this node to the list of block nodes
    addBlockNode(returnStmt, returnNode);
    return returnNode;
}