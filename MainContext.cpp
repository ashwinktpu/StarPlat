#include<stdio.h>
#include"FrontendContext.h"
#include"SymbolTable.h"

using namespace std;


void FrontendContext::startBlock()
{ 
   symbTab->createNewScope();
   Block* newBlock=new Block();
   blockList.push_back(newBlock);

}

void FrontendContext::endBlock()
{
   symbTab->endScope();
   blockList.pop_back();

}

Block* FrontendContext::getCurrentBlock()
{
   Block* b=blockList.back();
   return b;
}