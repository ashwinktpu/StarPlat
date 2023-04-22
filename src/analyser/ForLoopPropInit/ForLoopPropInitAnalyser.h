#ifndef FOR_LOOP_PROP_INIT
#define FOR_LOOP_PROP_INIT
#include "../../ast/ASTNodeTypes.hpp"
class ForLoopPropInitAnalyser{
    void analyseFunc(Function* func);
    void analyseBlockStatement(blockStatement* stmt);
    void analyseStatement(statement* stmt);
    public:
    ForLoopPropInitAnalyser(){
    }
    void analyse(list<Function *> funcList);
};
#endif