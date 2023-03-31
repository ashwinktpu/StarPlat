#ifndef FOR_LOOP_FUSION_ANALYSER
#define FOR_LOOP_FUSION_ANALYSER
#include "../../ast/ASTNodeTypes.hpp"
class ForLoopFusionAnalyser{
    void analyseFunc(Function* func);
    void analyseBlockStatement(blockStatement* stmt);
    void analyseStatement(statement* stmt);
    public:
    ForLoopFusionAnalyser(){
    }
    void analyse(list<Function *> funcList);
};
#endif