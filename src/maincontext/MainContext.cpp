#include "MainContext.hpp"
#include "../symbolutil/SymbolTable.hpp"
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
