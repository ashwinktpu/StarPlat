#include<stdio.h>

using namespace std;

class FrontEndContext
{
  private:
  vector<Block*> blockList();

  public:
  FrontEndContext();
  void startBlock();
  void endBlock();
  Block* getCurrentBlock();


};