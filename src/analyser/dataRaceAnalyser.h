#ifndef DR_ANALYSER
#define DR_ANALYSER

#include "../ast/ASTNodeTypes.hpp"

class dataRaceAnalyser
{
  public:
  dataRaceAnalyser(){}

  void analyse();
  void analyseFunc(ASTNode* );
  void analyseStatement(statement* );
  statement* forAllAnalysis(forallStmt* );
  statement* ngbrForAnalysis(forallStmt*, Identifier*, Identifier*);
  statement* relPropUpdateAnalysis(ifStmt*, Identifier*);
  statement* unaryReductionAnalysis(unary_stmt* );
  statement* assignReductionAnalysis(assignment *);
  statement* blockReductionAnalysis(blockStatement* , Identifier* );


};

#endif
