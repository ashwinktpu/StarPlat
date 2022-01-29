#ifndef AST_ANALYSER
#define AST_ANALYSER

#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
//#include "dslCodePad.h"

class ASTAnalyser
{
  public:
  ASTAnalyser(){
  }

  void analyse();
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  void analyseBlock(blockStatement* blockStmt);
};

#endif
