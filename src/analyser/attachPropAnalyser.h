#ifndef ATTACH_PROP_ANALYSER
#define ATTACH_PROP_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

class attachPropAnalyser
{
  public:
  attachPropAnalyser(){
  }

  void analyse();
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  void analyseBlock(blockStatement* blockStmt);
};

#endif
