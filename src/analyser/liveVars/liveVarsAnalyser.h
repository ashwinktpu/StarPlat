#ifndef LIVE_VARS_ANALYZER_H
#define LIVE_VARS_ANALYZER_H

#include <unordered_map>
#include "../../ast/ASTHelper.cpp"

class liveVarsAnalyser
{
    private:
    std::unordered_map<ASTNode*, liveVarsNode*> nodeMap;

    std::list<ASTNode*> nodeList;

    public:
    liveVarsAnalyser();

    void analyse(list<Function*>);
    void initFunc(Function*);

    liveVarsNode* initStatement(statement*, liveVarsNode*);
    liveVarsNode* initAssignment(assignment*, liveVarsNode*);
    liveVarsNode* initBlockStatement(blockStatement*, liveVarsNode*);
    liveVarsNode* initDeclaration(declaration*, liveVarsNode*);
    liveVarsNode* initDoWhile(dowhileStmt*, liveVarsNode*);
    liveVarsNode* initFixedPoint(fixedPointStmt*, liveVarsNode*);
    liveVarsNode* initForAll(forallStmt*, liveVarsNode*);
    liveVarsNode* initIfStmt(ifStmt*, liveVarsNode*);
    liveVarsNode* initIterateBFS(iterateBFS*, liveVarsNode*);
    liveVarsNode* initProcCall(proc_callStmt*, liveVarsNode*);
    liveVarsNode* initReduction(reductionCallStmt*, liveVarsNode*);
    liveVarsNode* initReturn(returnStmt*, liveVarsNode*);
    liveVarsNode* initUnary(unary_stmt*, liveVarsNode*);
    liveVarsNode* initWhile(whileStmt*, liveVarsNode*);
};

#endif