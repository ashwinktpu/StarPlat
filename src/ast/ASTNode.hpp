#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>

using namespace std;

class ASTNode
{
  protected:
  ASTNode* parent;
  string typeofNode;

  public:
  ASTNode()
  {
      parent=NULL;
  }
  void setParent(ASTNode* node)
  {
      parent=node;
  }
  void setTypeofNode(string type)
  {
    typeofNode=type;
  }
  string getTypeofNode()
  {
    return typeofNode;
  }
  ASTNode* getParent()
  {
        return parent;
  }


};

#endif