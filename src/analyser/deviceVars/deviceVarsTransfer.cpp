#include "deviceVarsAnalyser.h"

list<statement*> transferStatements(lattice &inp, lattice &out)
{
    unordered_map<TableEntry*, lattice::PointType> inputMap = inp.getLattice();
    unordered_map<TableEntry*, lattice::PointType> outputMap = out.getLattice();

    
}

statement* transferVarsStatement(statement* stmt, blockStatement* parBlock)
{
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
    }

    return stmt;
}

statement* transferVarsForAll(forallStmt* stmt, blockStatement* parBlock);
statement* transferVarsUnary(unary_stmt* stmt, blockStatement* parBlock)
{
    
}
statement* transferVarsBlock(blockStatement* stmt, blockStatement* parBlock)
{
    blockStatement* newBlock = blockStatement::createnewBlock();
    for(statement* stmt: stmt->returnStatements())
    {
        statement* newStmt = transferVarsStatement(stmt, newBlock);
        newBlock->addStmtToBlock(newStmt);
    }
    return newBlock;
}
statement* transferVarsDeclaration(declaration* stmt, blockStatement* parBlock);
statement* transferVarsWhile(whileStmt* stmt, blockStatement* parBlock);
statement* transferVarsDoWhile(dowhileStmt* stmt, blockStatement* parBlock);
statement* transferVarsAssignment(assignment* stmt, blockStatement* parBlock);
statement* transferVarsIf(ifStmt* stmt, blockStatement* parBlock);
statement* transferVarsExpr(Expression* stmt, blockStatement* parBlock);