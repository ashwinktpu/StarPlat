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
    void analyseFunc(Function*);

    liveVarsNode* analyseStatement(statement*, liveVarsNode*);
    liveVarsNode* analyseAssignment(assignment*, liveVarsNode*);
    liveVarsNode* analyseBlockStatement(blockStatement*, liveVarsNode*);
    liveVarsNode* analyseDeclaration(declaration*, liveVarsNode*);
    liveVarsNode* analyseDoWhile(dowhileStmt*, liveVarsNode*);
    liveVarsNode* analyseFixedPoint(fixedPointStmt*, liveVarsNode*);
    liveVarsNode* analyseForAll(forallStmt*, liveVarsNode*);
    liveVarsNode* analyseIfStmt(ifStmt*, liveVarsNode*);
    liveVarsNode* analyseIterateBFS(iterateBFS*, liveVarsNode*);
    liveVarsNode* analyseProcCall(proc_callStmt*, liveVarsNode*);
    liveVarsNode* analyseReduction(reductionCallStmt*, liveVarsNode*);
    liveVarsNode* analyseReturn(returnStmt*, liveVarsNode*);
    liveVarsNode* analyseUnary(unary_stmt*, liveVarsNode*);
    liveVarsNode* analyseWhile(whileStmt*, liveVarsNode*);
};

#endif