#ifndef AST_ANALYSER
#define AST_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

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
