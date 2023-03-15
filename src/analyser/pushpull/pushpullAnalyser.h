#ifndef PUSH_PULL_ANALYSER
#define PUSH_PULL_ANALYSER

// #include "../analyserUtil.cpp"
#include "../../ast/ASTNodeTypes.hpp"

class pushpullAnalyser{

    
    void analyseFunc(ASTNode *proc);
    void analyseStatement(statement *stmt);
    void analyseBlock(blockStatement *stmt);
    int analyseForAll(forallStmt *stmt);
    int analyseforinfor(forallStmt *stmt, Identifier *ownvertex);
    int analyseassigninfor(assignment *stmt,Identifier *ownvertex);
    int analysereductioninfor(reductionCallStmt *stmt,Identifier *ownvertex);
    int analyseStatementinForAll(statement* stmt,Identifier* ownvertex);

    public : void
                analyse(list<Function *> funcList);
};
#endif