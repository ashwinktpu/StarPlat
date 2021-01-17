#ifndef MAINCONTEXT_H
#define MAINCONTEXT_H


//#include<stdio.h>
#include "../ast/ASTNodeTypes.hpp"
#include <vector>

using namespace std;

class FrontEndContext
{
  private:
  vector<blockStatement*> *blockList;
  static FrontEndContext* instance;
  FrontEndContext()
  {
    blockList=new vector<blockStatement*>();
    
  }
  public:
  static FrontEndContext* getInstance()
  {
    if(!instance)
      instance=new FrontEndContext();
    return instance;  

   }
  
  void startBlock(blockStatement* blockStmtSent);
  void endBlock();
  blockStatement* getCurrentBlock();


};




#endif