#include "deviceVarsAnalyser.h"
#include <unordered_map>

list<statement*> transferStatements(lattice &inp, lattice &out)
{
    unordered_map<TableEntry*, lattice::PointType> inputMap = inp.getLattice();
    unordered_map<TableEntry*, lattice::PointType> outputMap = out.getLattice();

    list<statement*> transferStatements;
    for(pair<TableEntry*, lattice::PointType> vars: inputMap)
    {
        lattice::PointType inpType, outType;
        inpType = vars.second;
        outType = outputMap.at(vars.first);

        if(inpType == lattice::CPU_ONLY)
        {
            if((outType == lattice::GPU_ONLY) || (outType == lattice::CPU_GPU_SHARED))
            {
                varTransferStmt* transferStmt = new varTransferStmt(vars.first->getId(), 0);
                transferStatements.push_back(transferStmt);
            }
        }
        else if(inpType == lattice::GPU_ONLY)
        {
            if((outType == lattice::CPU_ONLY) || (outType == lattice::CPU_GPU_SHARED))
            {
                varTransferStmt* transferStmt = new varTransferStmt(vars.first->getId(), 1);
                transferStatements.push_back(transferStmt);
            }
        }
    }
    return transferStatements;
}

statement* deviceVarsAnalyser::transferVarsStatement(statement* stmt, blockStatement* parBlock)
{
    printf("Transfering %p %d\n", stmt, stmt->getTypeofNode());
    
    switch (stmt->getTypeofNode())
    {
    case NODE_DECL:
        return transferVarsDeclaration((declaration *)stmt, parBlock);
    case NODE_ASSIGN:
        return transferVarsAssignment((assignment *)stmt, parBlock);
    case NODE_FORALLSTMT:
    {
        forallStmt *forStmt = (forallStmt *)stmt;

        if (forStmt->isForall())
        return transferVarsForAll((forallStmt *)stmt, parBlock);
        else
        return transferVarsFor((forallStmt*)stmt, parBlock);
    }
    case NODE_BLOCKSTMT:
        return transferVarsBlock((blockStatement *)stmt, parBlock);
    case NODE_UNARYSTMT:
        return transferVarsUnary((unary_stmt *)stmt, parBlock);
    case NODE_IFSTMT:
        return transferVarsIfElse((ifStmt *)stmt, parBlock);
    case NODE_WHILESTMT:
        return transferVarsWhile((whileStmt *)stmt, parBlock);
    case NODE_DOWHILESTMT:
        return transferVarsDoWhile((dowhileStmt *)stmt, parBlock);
    case NODE_PROCCALLSTMT:
        return transferVarsProcCall((proc_callStmt *)stmt, parBlock);
    case NODE_FIXEDPTSTMT:
        return transferVarsFixedPoint((fixedPointStmt *)stmt, parBlock);
    case NODE_REDUCTIONCALLSTMT:
        return transferVarsReduction((reductionCallStmt *)stmt, parBlock);
    case NODE_ITRBFS:
        return transferVarsItrBFS((iterateBFS *)stmt, parBlock);
    }

    return stmt;
}

statement* deviceVarsAnalyser::transferVarsForAll(forallStmt* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    for(statement* bstmt: transferStatements(wrapNode->inMap, wrapNode->outMap))
        parBlock->addStmtToBlock(bstmt);

    return stmt;
}
statement* deviceVarsAnalyser::transferVarsFor(forallStmt* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    list<statement*> transferStmts = transferStatements(wrapNode->inMap, wrapNode->outMap);
    for(statement* bstmt: transferStmts)
        parBlock->addStmtToBlock(bstmt);

    blockStatement* newBody = (blockStatement*) transferVarsStatement(stmt->getBody(), parBlock);
    lattice bodyOut = getWrapNode(stmt->getBody())->outMap;
    bodyOut.removeVariable(stmt->getIterator());
    
    transferStmts = transferStatements(bodyOut, wrapNode->outMap);
    for(statement* bstmt: transferStmts)
        newBody->addStmtToBlock(bstmt);
    
    stmt->setBody(newBody);
    return stmt;
}
statement* deviceVarsAnalyser::transferVarsUnary(unary_stmt* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    list<statement*> transferStmts = transferStatements(wrapNode->inMap, wrapNode->outMap);
    for(statement* stmt: transferStmts)
        parBlock->addStmtToBlock(stmt);
    
    return stmt;
}
statement* deviceVarsAnalyser::transferVarsBlock(blockStatement* stmt, blockStatement* parBlock)
{
    blockStatement* newBlock = blockStatement::createnewBlock();
    for(statement* bstmt: stmt->returnStatements())
    {
        statement* newStmt = transferVarsStatement(bstmt, newBlock);
        newBlock->addStmtToBlock(newStmt);
    }
    return newBlock;
}
//Add statement to make a copy in GPU
statement* deviceVarsAnalyser::transferVarsDeclaration(declaration* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    list<statement*> transferStmts = transferStatements(wrapNode->inMap, wrapNode->outMap);
    for(statement* bstmt: transferStmts)
        parBlock->addStmtToBlock(bstmt);
    
    gpuUsedVars.isUsedVar(stmt->getdeclId()) ? stmt->setInGPU(true) : stmt->setInGPU(false);
    return stmt;
}
statement* deviceVarsAnalyser::transferVarsWhile(whileStmt* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    ASTNodeWrap* condNode = getWrapNode(stmt->getCondition());

    //wrapNode->inMap.print();
    //condNode->outMap.print();
    for(statement* bstmt: transferStatements(wrapNode->inMap, condNode->outMap)){
        parBlock->addStmtToBlock(bstmt);
    }

    blockStatement* newBody = (blockStatement*) transferVarsStatement(stmt->getBody(), parBlock);

    //TODO: Check the merge of results
    for(statement* bstmt: transferStatements(wrapNode->outMap, condNode->outMap)){
        newBody->addStmtToBlock(bstmt);
    }

    stmt->setBody(newBody);
    return stmt;
}
statement* deviceVarsAnalyser::transferVarsDoWhile(dowhileStmt* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    ASTNodeWrap* condNode = getWrapNode(stmt->getCondition());

    blockStatement* newBody = (blockStatement*) transferVarsStatement(stmt->getBody(), parBlock);
    {
        Identifier* tempIden = (Identifier*) Util::createIdentifierNode(getTempVar());
        Type* idenType = (Type*) Util::createPrimitiveTypeNode(TYPE_BOOL);

        TableEntry* e = new TableEntry(tempIden->copy(), idenType);
        tempIden->setSymbolInfo(e);

        ASTNode* boolFalse = Util::createNodeForBval(false);
        ASTNode* boolTrue = Util::createNodeForBval(true);

        Identifier* ifAssign = tempIden->copy();
        ifAssign->setSymbolInfo(e);
        assignment* tempAssign = (assignment*) Util::createAssignmentNode(ifAssign, boolTrue);
        newBody->addToFront(tempAssign);

        declaration* tempDecl = (declaration*) Util::createAssignedDeclNode(idenType,tempIden,boolFalse);
        parBlock->addStmtToBlock(tempDecl);

        blockStatement* ifBody = blockStatement::createnewBlock();
        for(statement* bstmt: transferStatements(condNode->outMap, wrapNode->outMap)){
            ifBody->addStmtToBlock(bstmt);
        }

        Identifier* ifCond = tempIden->copy();
        ifCond->setSymbolInfo(e);

        ifStmt* varTransfer = (ifStmt*) Util::createNodeForIfStmt(Util::createNodeForId(ifCond), ifBody, NULL);
        newBody->addToFront(varTransfer);
    }
    
    lattice bodyOut = getWrapNode(stmt->getBody())->outMap;
    for(statement *bstmt: transferStatements(bodyOut, condNode->outMap)){
        newBody->addStmtToBlock(bstmt);
    }
    for(statement* bstmt: transferStatements(wrapNode->inMap, wrapNode->outMap)){
        parBlock->addStmtToBlock(bstmt);
    }

    stmt->setBody(newBody);
    return stmt;
}

statement* deviceVarsAnalyser::transferVarsAssignment(assignment* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    list<statement*> transferStmts = transferStatements(wrapNode->inMap, wrapNode->outMap);
    for(statement* bstmt: transferStmts)
        parBlock->addStmtToBlock(bstmt);
    
    return stmt;
}
statement* deviceVarsAnalyser::transferVarsIfElse(ifStmt* stmt, blockStatement* parBlock)
{
    Expression* expr = stmt->getCondition();
    ASTNodeWrap* condWrap = getWrapNode(expr);
    ASTNodeWrap* wrapNode = getWrapNode(stmt);

    for(statement* bstmt: transferStatements(condWrap->inMap, condWrap->outMap)){
        parBlock->addStmtToBlock(bstmt);
    }

    ifStmt* newStmt;
    if(stmt->getElseBody() != nullptr)
    {
        blockStatement* ifBody = (blockStatement*) transferVarsStatement(stmt->getIfBody(), parBlock);
        blockStatement* elseBody = (blockStatement*) transferVarsStatement(stmt->getElseBody(), parBlock);

        lattice ifOut = getWrapNode(stmt->getIfBody())->outMap;
        lattice elseOut = getWrapNode(stmt->getElseBody())->outMap;

        for(statement* bstmt: transferStatements(ifOut, wrapNode->outMap)){
            ifBody->addStmtToBlock(bstmt);
        }
        for(statement* bstmt: transferStatements(elseOut, wrapNode->outMap)){
            elseBody->addStmtToBlock(bstmt);
        }

        newStmt = ifStmt::create_ifStmt(expr, ifBody, elseBody);
    }
    else
    {
        blockStatement* ifBody = (blockStatement*) transferVarsStatement(stmt->getIfBody(), parBlock);
        lattice ifOut = getWrapNode(stmt->getIfBody())->outMap;

        for(statement* bstmt: transferStatements(ifOut, wrapNode->outMap)){
            ifBody->addStmtToBlock(bstmt);
        }
        
        blockStatement* elseBody = (blockStatement*) blockStatement::createnewBlock();
        for(statement* bstmt: transferStatements(condWrap->outMap, wrapNode->outMap))
            elseBody->addStmtToBlock(bstmt);

        newStmt = ifStmt::create_ifStmt(expr, ifBody, elseBody);
    }
    return newStmt;
}

statement* deviceVarsAnalyser::transferVarsFixedPoint(fixedPointStmt* stmt, blockStatement* parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    ASTNodeWrap* condNode = getWrapNode(stmt->getFixedPointId());

    for(statement* bstmt: transferStatements(wrapNode->inMap, condNode->outMap)){
        parBlock->addStmtToBlock(bstmt);
    }

    blockStatement* newBody = (blockStatement*) transferVarsStatement(stmt->getBody(), parBlock);

    for(statement* bstmt: transferStatements(wrapNode->outMap, condNode->outMap)){
        newBody->addStmtToBlock(bstmt);
    }

    stmt->setBody(newBody);
    return stmt;
}

statement* deviceVarsAnalyser::transferVarsProcCall(proc_callStmt *stmt, blockStatement *parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    list<statement*> transferStmts = transferStatements(wrapNode->inMap, wrapNode->outMap);
    for(statement* bstmt: transferStmts)
        parBlock->addStmtToBlock(bstmt);
    
    return stmt;
}

statement* deviceVarsAnalyser::transferVarsReduction(reductionCallStmt* stmt, blockStatement *parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    list<statement*> transferStmts = transferStatements(wrapNode->inMap, wrapNode->outMap);
    for(statement* bstmt: transferStmts)
        parBlock->addStmtToBlock(bstmt);
    
    return stmt;
}

statement* deviceVarsAnalyser::transferVarsItrBFS(iterateBFS* stmt, blockStatement *parBlock)
{
    ASTNodeWrap* wrapNode = getWrapNode(stmt);
    for(statement* bstmt: transferStatements(wrapNode->inMap, wrapNode->outMap))
        parBlock->addStmtToBlock(bstmt);    

    return stmt;
}