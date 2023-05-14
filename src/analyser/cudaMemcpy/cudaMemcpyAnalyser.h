#ifndef CUDA_MEMCPY_ANALYSER
#define CUDA_MEMCPY_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"


class cudaMemcpyAnalyser{
    void analyseFunc(ASTNode *proc);
    void analyseStatement(statement *stmt);
    void analyseBlock(blockStatement *stmt);
    void analyseForAll(forallStmt *stmt);
    void analysedowhile(dowhileStmt *stmt, list<Identifier *> &primvars);
    void analysefixedpoint(fixedPointStmt *stmt, list<Identifier *> &primvars);

public:
    void analyse(list<Function *> funcList);
};
#endif