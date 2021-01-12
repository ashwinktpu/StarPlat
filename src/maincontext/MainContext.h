#include<stdio.h>
#include<ASTNodeTypes.h>
#include<vector>



class FrontEndContext
{
  private:
  vector<blockStatement*> *blockList;

  public:
  FrontEndContext();
  void startBlock(blockStatement* blockStmtSent);
  void endBlock();
  blockStatement* getCurrentBlock();


};