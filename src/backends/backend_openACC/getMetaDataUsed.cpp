#ifndef GET_METADATA_USED_H
#define GET_METADATA_USED_H

#include "../../ast/ASTNodeTypes.hpp"

MetaDataUsed getMetaDataUsedStatement(statement* stmt);
MetaDataUsed getMetaDataUsedBlock(blockStatement* blockStmt)
{
    list<statement*> stmtList = blockStmt->returnStatements();
    MetaDataUsed currVars;
    for(statement* stmt : stmtList)
    {
        currVars |= getMetaDataUsedStatement(stmt);
    }

    return currVars;
}

MetaDataUsed getMetaDataUsedIf(ifStmt* ifStmt)
{
    MetaDataUsed currVars;
    currVars |= getMetaDataUsedStatement(ifStmt->getIfBody());
    currVars |= getMetaDataUsedStatement(ifStmt->getElseBody());
    return currVars;
}

MetaDataUsed getMetaDataUsedWhile(whileStmt* whileStmt)
{
    return getMetaDataUsedStatement(whileStmt->getBody());
}

MetaDataUsed getMetaDataUsedDoWhile(dowhileStmt* doWhileStmt)
{
    return getMetaDataUsedStatement(doWhileStmt->getBody());
}

MetaDataUsed getMetaDataUsedFor(forallStmt* forStmt)
{
    return forStmt->getMetaDataUsed();
}

MetaDataUsed getMetaDataUsedFixedPoint(fixedPointStmt* fpStmt)
{
    return getMetaDataUsedStatement(fpStmt->getBody());
}

MetaDataUsed getMetaDataUsedRBFS(iterateReverseBFS* rbfsStmt)
{
    return getMetaDataUsedStatement(rbfsStmt->getBody());
}

MetaDataUsed getMetaDataUsedBFS(iterateBFS* bfsStmt)
{   
    return bfsStmt->getMetaDataUsed();
}

MetaDataUsed getMetaDataUsedStatement(statement* stmt)
{
    switch(stmt->getTypeofNode())
    {
        case NODE_BLOCKSTMT:
            return getMetaDataUsedBlock((blockStatement*)stmt);
        
        case NODE_IFSTMT:
            return getMetaDataUsedIf((ifStmt*)stmt);

        case NODE_WHILESTMT:
            return getMetaDataUsedWhile((whileStmt*)stmt);

        case NODE_DOWHILESTMT:
            return getMetaDataUsedDoWhile((dowhileStmt*)stmt);

        case NODE_FORALLSTMT:
            return getMetaDataUsedFor((forallStmt*)stmt);

        case NODE_FIXEDPTSTMT:
            return getMetaDataUsedFixedPoint((fixedPointStmt*)stmt);

        case NODE_ITRBFS:
            return getMetaDataUsedBFS((iterateBFS*)stmt);

        default:
            ; // added to fix warning!
    }
    return MetaDataUsed();
}

#endif