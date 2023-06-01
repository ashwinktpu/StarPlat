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
  map<string, bool> dynamicLinkFunc;
  static FrontEndContext* instance;
  int currentFuncType;
  int genFuncCount;
  int staticFuncCount;
  int inFuncCount;
  int decFuncCount;
  int dynFuncCount;
  


  public:
  FrontEndContext()
  {
    genFuncCount = 0;
    staticFuncCount = 0;
    inFuncCount = 0;
    decFuncCount = 0;
    dynFuncCount = 0;
  
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
  map<string, bool> getDynamicLinkFuncs();
  void setDynamicLinkFuncs(map<string, bool> sentMap);
  void addFuncToList(Function* func);
  void setCurrentFuncType(int funcType);
  int getCurrentFuncType();
  void incrementCurrentFuncCount();
  int getCurrentFuncCount();

};




#endif