#ifndef ATTACH_PROP_ANALYSER
#define ATTACH_PROP_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"

class attachPropAnalyser
{
  private:
  int backend;  //backend = 1 for cuda, and for other backends set this value when required
  public:
  attachPropAnalyser(){
    backend = -1;
  }
  attachPropAnalyser(int backend){
    this->backend = backend;
  }

  //Analyses all the functions to merge the possible attachNodeProp for-all statements
  void analyse(list<Function *> funcList);
  //Analyses the body of given function
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  //Iterates through all the attachProp statements in the current block and tries to merge them
  void analyseBlock(blockStatement* blockStmt);
};

#endif
