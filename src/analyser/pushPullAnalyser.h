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
    fixedPointStmt *analyseFPLoop(statement *);
    forallStmt *analyseForallOut(statement *, Identifier *);
    forallStmt *analyseForallIn(statement *, Identifier *, Identifier *, Identifier *, int);
    blockStatement *checkSSSPUpdate(statement *, Identifier *, Identifier *, Identifier *, Identifier *);

};

#endif