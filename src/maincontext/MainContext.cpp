#include"MainContext.h"
#include"../symbolutil/SymbolTable.h"



FrontEndContext::FrontEndContext()
{
   blockList=new vector<blockStatement*>();
}

void FrontendContext::startBlock(blockStatement* blockStmtSent)
{ 
   symbTab->createNewScope();
  // Block* newBlock=new Block();
   blockList.push_back(blockStmtSent);

}

void FrontendContext::endBlock()
{
   symbTab->endScope();
   blockList.pop_back();

}

blockStatement* FrontendContext::getCurrentBlock()
{
   blockStatement* b=blockList.back();
   return b;
}