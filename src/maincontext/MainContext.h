<<<<<<< HEAD
#include<stdio.h>
#include "../ast/ASTNodeTypes.h"
#include<vector>
=======
#include <stdio.h>
#include "ASTNodeTypes.h"
#include <vector>
>>>>>>> 43c2a08e026f040d5049c3bf784dfbb42fa84e08

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