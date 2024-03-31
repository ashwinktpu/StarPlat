#ifndef LIVE_VARS_ANALYZER_H
#define LIVE_VARS_ANALYZER_H

#include <unordered_map>
#include "../../ast/ASTHelper.cpp"
#include "../analyserUtil.h"

class liveVarsAnalyser
{
    private:
    std::unordered_map<ASTNode*, liveVarsNode*> nodeMap;
    std::list<ASTNode*> nodeList;
    std::list<liveVarsNode*> currReturnNodes;

    public:
    liveVarsAnalyser();

    void analyse(list<Function*>);
    void analysecfg(liveVarsNode*);
    bool analyseNode(liveVarsNode*);
    void printLiveVarsNode(ASTNode*);

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
    liveVarsNode* initIterateRBFS(iterateReverseBFS*, liveVarsNode*);
    liveVarsNode* initProcCall(proc_callStmt*, liveVarsNode*);
    liveVarsNode* initReduction(reductionCallStmt*, liveVarsNode*);
    liveVarsNode* initReturn(returnStmt*, liveVarsNode*);
    liveVarsNode* initUnary(unary_stmt*, liveVarsNode*);
    liveVarsNode* initWhile(whileStmt*, liveVarsNode*);

    //Returns the used variables in each statment
    usedVariables getVarsStatement(statement*);
    usedVariables getVarsBlock(blockStatement*);
    usedVariables getVarsForAll(forallStmt*);
    usedVariables getVarsUnary(unary_stmt*);
    usedVariables getVarsDeclaration(declaration*);
    usedVariables getVarsWhile(whileStmt*);
    usedVariables getVarsDoWhile(dowhileStmt*);
    usedVariables getVarsAssignment(assignment*);
    usedVariables getVarsIf(ifStmt*);
    usedVariables getVarsExpr(Expression*);
    usedVariables getVarsReduction(reductionCallStmt*);
    usedVariables getVarsFixedPoint(fixedPointStmt*);
    usedVariables getVarsBFS(iterateBFS*);
    usedVariables getVarsRBFS(iterateReverseBFS*);
    usedVariables getVarsProcCall(proc_callStmt*);
    usedVariables getVarsExprProcCall(proc_callExpr*);
};

#endif