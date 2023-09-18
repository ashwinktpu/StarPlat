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
    void initFunc(Function*);

    liveVarsNode* initStatement(statement*, set<liveVarsNode*>);
    liveVarsNode* initAssignment(assignment*, set<liveVarsNode*>);
    liveVarsNode* initBlockStatement(blockStatement*, set<liveVarsNode*>);
    liveVarsNode* initDeclaration(declaration*, set<liveVarsNode*>);
    liveVarsNode* initDoWhile(dowhileStmt*, set<liveVarsNode*>);
    liveVarsNode* initFixedPoint(fixedPointStmt*, set<liveVarsNode*>);
    liveVarsNode* initForAll(forallStmt*, set<liveVarsNode*>);
    liveVarsNode* initIfStmt(ifStmt*, set<liveVarsNode*>);
    liveVarsNode* initIterateBFS(iterateBFS*, set<liveVarsNode*>);
    liveVarsNode* initProcCall(proc_callStmt*, set<liveVarsNode*>);
    liveVarsNode* initReduction(reductionCallStmt*, set<liveVarsNode*>);
    liveVarsNode* initReturn(returnStmt*, set<liveVarsNode*>);
    liveVarsNode* initUnary(unary_stmt*, set<liveVarsNode*>);
    liveVarsNode* initWhile(whileStmt*, set<liveVarsNode*>);

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