#ifndef PR_ANALYSER
#define PR_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"

class BoolProp
{
    bool _bool;
    PropAccess *prop;
    Identifier *sum;
    
    public:
    BoolProp() {}
    BoolProp(bool b, PropAccess *p = NULL, Identifier *s = NULL) : _bool(b), prop(p), sum(s) {}
    bool getBool() { return _bool; }
    PropAccess *getProp() { return prop; }
    Identifier *getSum() { return sum; }
};

class PRAnalyser
{
    char *backend;
    public:
    PRAnalyser() {}

    void analyse(char *);
    void analyseFunc(ASTNode *);
    void analyseBlock(statement *);
    BoolProp analyseForAll(forallStmt *);
    blockStatement *analyseDoWhile(statement *);
    BoolProp analyseFor(forallStmt *);
};

#endif