#ifndef PP_ANALYSER
#define PP_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"

class PPAnalyser
{
    char *backend;
    public:
    PPAnalyser() {}

    void analyse(char *);
    void analyseFunc(ASTNode *);
    void analyseBlock(statement *);
    fixedPointStmt *analyseFPLoop(statement *);
    forallStmt *analyseForallOut(statement *, Identifier *, Identifier *);
    forallStmt *analyseForallIn(statement *, Identifier *, Identifier *, Identifier *, int, Identifier *);
    blockStatement *checkSSSPUpdate(statement *, Identifier *, Identifier *, Identifier *, Identifier *, Identifier *);

};

#endif