#ifndef PR_ANALYSER
#define PR_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

class BoolProp
{
    public:
    bool _bool;
    PropAccess *prop;
    BoolProp() {}
    BoolProp(bool b, PropAccess *p) : _bool(b), prop(p) {}
    bool getBool() { return _bool; }
    PropAccess *getProp() { return prop; }
};

class PRAnalyser
{
    public:
    PRAnalyser() {}

    void analyse();
    void analyseFunc(ASTNode *);
    void analyseBlock(statement *);
    void analyseForAll(forallStmt *);
    BoolProp analyseFor(forallStmt *);
};

#endif