#include "PPAnalyser.h"
#include "../ast/ASTHelper.cpp"
#include "../symbolutil/SymbolTable.h"
#include "analyserUtil.hpp"

void PPAnalyser::analyseBlock(statement* stmt)
{
    blockStatement *blockStmt = (blockStatement *)stmt;
    for (statement *stmt: blockStmt->returnStatements())
    {
        switch (stmt->getTypeofNode())
        {
            case NODE_BLOCKSTMT:
            {
                analyseBlock(stmt);
                break;
            }
            case NODE_FORALLSTMT:
            {
            }
            case NODE_DOWHILESTMT:
            {
            }
            case NODE_WHILESTMT:
            {
            }
            case NODE_FIXEDPTSTMT:
            {
            }
        }
    }
}

void PPAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseBlock(func->getBlockStatement());
    return;
}

void PPAnalyser::analyse()
{
    list<Function *> funcList = frontEndContext.getFuncList();
    for (Function *func : funcList)
        analyseFunc(func);
}