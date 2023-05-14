#ifndef READ_EXPRESSION_ANALYSER
#define READ_EXPRESSION_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"


class readExpressionAnalyser{

    
    void analyseFunc(ASTNode *proc);
    void analyseStatement(statement *stmt);
    void analyseBlock(blockStatement *stmt,statement* parent);
    void analyseBlockfirst(blockStatement *stmt, statement *parent);
    void analyseforBlock(forallStmt *stmt, statement *forstmt);
    void analyseassigninBlock(assignment *stmt, statement *forstmt);
    void analysedeclarationinBlock(declaration *stmt, statement *parent);
    void analysereductioninBlock(reductionCallStmt *stmt, statement *forstmt);
    void analyseStatementinBlock(statement *stmt, statement* parent);
    void analyseStatementinBlockfirst(statement *stmt, statement *parent);
    int analyseexprinBlock(Expression *stmt, statement *parent);
    void analyseexprinBlock_second(Expression *stmt, statement *parent, Type *type,statement *fatherparent);
    void analyseifinBlock(ifStmt *stmt, statement *forstmt);
    void analyseBlockinBlock(blockStatement *stmt, statement *forstmt);

public:
    void analyse(list<Function *> funcList);
};
#endif