#ifndef PUSH_PULL_ANALYSER
#define PUSH_PULL_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"


class pushpullAnalyser{

    
    void analyseFunc(ASTNode *proc);
    void analyseStatement(statement *stmt);
    void analyseBlock(blockStatement *stmt);
    int analyseForAll(forallStmt *stmt);
    int analyseitrbfs(iterateBFS *stmt);
    int analyseitrrevbfs(iterateReverseBFS *stmt);
    int analyseforinfor(forallStmt *stmt, Identifier *ownvertex);
    int analyseassigninfor(assignment *stmt,Identifier *ownvertex);
    int analysereductioninfor(reductionCallStmt *stmt,Identifier *ownvertex);
    int analyseStatementinForAll(statement* stmt,Identifier* ownvertex);
    int analyseunaryinfor(unary_stmt *stmt, Identifier *ownvertex);
    int analyseifinfor(ifStmt* stmt,Identifier *ownvertex);
    int analyseBlockinfor(blockStatement *stmt, Identifier *ownvertex);

public:
    void analyse(list<Function *> funcList);
};
#endif