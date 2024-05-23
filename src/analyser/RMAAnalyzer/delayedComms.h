#include "../../ast/ASTNodeTypes.hpp"
class delayedComms {
  
  private:
  unordered_map<int, string> functionMap ;
  std::vector<std::vetor<int> > callGraph ;
  std::vector<statement*> statementIdx ;
  
  public:
  delayedComms ();
  void analyse(list<Function *> funcList);
  void analyseFunc(ASTNode* );
  void analyseStatement(statement* );
  statement* forAllAnalysis(forallStmt* );
  statement* ngbrForAnalysis(forallStmt*, Identifier*, Identifier*);
  statement* relPropUpdateAnalysis(ifStmt*, Identifier*);
  statement* unaryReductionAnalysis(unary_stmt* );
  statement* unaryPropReductionAnalysis(unary_stmt* , Identifier* );
  statement* assignReductionAnalysis(assignment *);
} ;
