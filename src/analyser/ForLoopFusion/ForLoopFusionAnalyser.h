#ifndef FOR_LOOP_FUSION_ANALYSER
#define FOR_LOOP_FUSION_ANALYSER
#include "../../ast/ASTNodeTypes.hpp"
class ForLoopFusionAnalyser{
    void analyseFunc(ASTNode* proc);
    void analyseStatement(statement* stmt);
    void analyseBlock(blockStatement* blockStmt);
    public:
    ForLoopFusionAnalyser(){
    }
    void analyse(list<Function*> funclist);


};
#endif