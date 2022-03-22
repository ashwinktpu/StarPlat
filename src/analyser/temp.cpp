#include "pushPullAnalyser.h"
#include "../ast/ASTHelper.cpp"
#include "../symbolutil/SymbolTable.h"
#include "analyserUtil.hpp"

void PPAnalyser::analyseBlock(statement* stmt)
{
    //cout << "analysing block" << endl;
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
                cout << "here" << endl;
                forallStmt *f = (forallStmt *)stmt;
                proc_callExpr *p = f->getExtractElementFunc();
                // cout << p->getId1()->getIdentifier() << endl;
                // cout << p->getId2()->getIdentifier() << endl; 
                cout << p->getMethodId()->getIdentifier() << endl;
            }
        }
    }
}

void PPAnalyser::analyseFunc(ASTNode *proc)
{
    //cout << "analysing func" << endl;
    Function *func = (Function *)proc;
    cout << "entering block" << endl;
    analyseBlock(func->getBlockStatement());
    return;
}

void PPAnalyser::analyse()
{
    list<Function *> funcList = frontEndContext.getFuncList();
    for (Function *func : funcList)
        analyseFunc(func);
}