#ifndef DR_ANALYSER
#define DR_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

class dataRaceAnalyser
{
  public:
  class dataRaceAnalyser(){
  }

  void analyse();
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  void analyseForAll(forallStmt* stmt);
};

#endif
