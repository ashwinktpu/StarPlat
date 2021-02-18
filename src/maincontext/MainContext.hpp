#ifndef MAINCONTEXT_H
#define MAINCONTEXT_H


//#include<stdio.h>
#include "../ast/ASTNodeTypes.hpp"
#include <vector>

using namespace std;

class FrontEndContext
{
  private:
  vector<blockStatement*> blockList;
  list<Function*> funcList;
  static FrontEndContext* instance;

  public:
  FrontEndContext()
  {
  
  }

 /* static FrontEndContext* getInstance()
  {
    if(instance==NULL)
      instance=new FrontEndContext();
    return instance;  

   }
  */
  
  void startBlock(blockStatement* blockStmtSent);
  void endBlock();
  blockStatement* getCurrentBlock();
  list<Function*> getFuncList();
  void addFuncToList(Function* func);


};




#endif
