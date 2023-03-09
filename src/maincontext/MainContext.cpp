#include "MainContext.hpp"
//#include "../symbolutil/SymbolTable.hpp"
#include <vector>

using namespace std;

void FrontEndContext::startBlock(blockStatement* blockStmtSent)
{
   // symbTab->createNewScope();
    // Block* newBlock=new Block();
   blockList.push_back(blockStmtSent);
 

}

 void FrontEndContext::endBlock()
{
   //symbTab->endScope();
    blockList.pop_back();
}

void FrontEndContext::addFuncToList(Function* func)
{
   funcList.push_back(func);
}

blockStatement* FrontEndContext::getCurrentBlock()
{
   blockStatement* b= blockList.back();
   return b;
}

list<Function*> FrontEndContext::getFuncList()
{
   return funcList;
}

void FrontEndContext::setCurrentFuncType(int funcType )
{
   currentFuncType = funcType;
}

int FrontEndContext::getCurrentFuncType()
{
   return currentFuncType;
}

 void FrontEndContext::incrementCurrentFuncCount()
  {
     int funcType = currentFuncType;
     if(funcType==GEN_FUNC)
        genFuncCount++;
     else if(funcType == STATIC_FUNC)
          genFuncCount++;
     else if(funcType==INCREMENTAL_FUNC)
          inFuncCount++;
     else if(funcType==DECREMENTAL_FUNC)
           decFuncCount++;  
     else if(funcType == DYNAMIC_FUNC)
         dynFuncCount++;                      

  }

  void FrontEndContext::setDynamicLinkFuncs(map<string, bool> sentMap){

   dynamicLinkFunc = sentMap;

  }

 

 map<string, bool> FrontEndContext::getDynamicLinkFuncs(){

  return dynamicLinkFunc;

 }

  int FrontEndContext::getCurrentFuncCount()
      {
          int funcType = currentFuncType;
          int count = 0;
          if(funcType == GEN_FUNC)
             count = genFuncCount;
          else if(funcType == STATIC_FUNC)
                count = staticFuncCount;
          else if(funcType == INCREMENTAL_FUNC)
                 count = inFuncCount;         
          else if(funcType == DECREMENTAL_FUNC)
                 count = decFuncCount;
          else if(funcType == DYNAMIC_FUNC)
              count = dynFuncCount;       

        return count;
      }

  