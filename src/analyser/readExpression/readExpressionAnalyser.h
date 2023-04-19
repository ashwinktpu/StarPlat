#ifndef READ_EXPRESSION_ANALYSER
#define READ_EXPRESSION_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"


class readExpressionAnalyser{

    
    void analyseFunc(ASTNode *proc);
    void analyseStatement(statement *stmt);
    void analyseBlock(blockStatement *stmt);
    void analyseForAll(forallStmt *stmt);
    void analyseforinfor(forallStmt *stmt,  forallStmt *forstmt);
    void analyseassigninfor(assignment *stmt,  forallStmt *forstmt);
    void analysedeclarationinfor(declaration *stmt, forallStmt *parent);
    void analysereductioninfor(reductionCallStmt *stmt, forallStmt *forstmt);
    void analyseStatementinForAll(statement *stmt, forallStmt *forstmt);
    int analyseexprinfor(Expression *stmt, forallStmt *forstmt);
    void analyseexprinfor_second(Expression *stmt, forallStmt *forstmt,Type* type);
    void analyseifinfor(ifStmt *stmt, forallStmt *forstmt);
    void analyseBlockinfor(blockStatement *stmt, forallStmt *forstmt);

public:
    void analyse(list<Function *> funcList);
};
#endif