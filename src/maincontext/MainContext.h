#include<stdio.h>
#include "../ast/ASTNodeTypes.h"
#include<vector>

using namespace std;

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