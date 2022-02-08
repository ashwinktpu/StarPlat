#ifndef DR_ANALYSER
#define DR_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

class dataRaceAnalyser
{
  public:
  class dataRaceAnalyser(){
  }

  void analyse();
  void analyseFunc(ASTNode* );
  void analyseStatement(statement* );
  ASTNode* forAllAnalysis(forallStmt* );
  ASTNode* ngbrForAnalysis(forallStmt*, Identifier*, Identifier*);
  ASTNode* relPropUpdateAnalysis(ifStmt*, Identifier*);
};

#endif
