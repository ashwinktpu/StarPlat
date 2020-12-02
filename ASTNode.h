#include<stdio.h>

using namespace std;

class ASTNode
{
  protected:
  ASTNode* parent;

  public:
  ASTNode()
  {
      parent=NULL;
  }
  void setParent(ASTNode* node)
  {
      parent=node;
  }
  ASTNode* getParent()
  {
        return parent;
  }


};