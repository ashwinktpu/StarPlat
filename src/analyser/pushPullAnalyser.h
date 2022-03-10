#ifndef PP_ANALYSER
#define PP_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

class PPAnalyser
{
    public:
    PPAnalyser() {}

    void analyse();
    void analyseFunc(ASTNode *);
    void analyseBlock(statement *);
};

#endif