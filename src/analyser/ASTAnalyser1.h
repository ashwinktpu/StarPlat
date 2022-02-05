#ifndef AST_ANALYSER1
#define AST_ANALYSER1

#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"

class ASTAnalyser1
{
  public:
  ASTAnalyser1(){
  }

  void analyse();
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  void analyseBlock(blockStatement* blockStmt);
  usedVariables getVarsStatement(statement* stmt);
  usedVariables getVarsBlock(blockStatement* stmt);
  usedVariables getVarsWhile(whileStmt* stmt);
  usedVariables getVarsDoWhile(dowhileStmt* stmt);
  usedVariables getVarsAssignment(assignment* stmt);
  usedVariables getVarsIf(ifStmt* stmt);
  usedVariables getVarsFixedPoint(fixedPointStmt* stmt);
  usedVariables getVarsReduction(reductionCallStmt* stmt);
  usedVariables getVarsBFS(iterateBFS* stmt);
  usedVariables getVarsExpr(Expression* stmt);
  //usedVariables getVarsProcCall(proc_callStmt* stmt);
};

#endif
