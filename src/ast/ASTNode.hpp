#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include "../maincontext/enum_def.hpp"

using namespace std;

class ASTNode
{
  protected:
  ASTNode* parent;
  //string typeofNode;
  NODETYPE typeofNode;
  public:
  ASTNode()
  {
      parent=NULL;
  }
  void setParent(ASTNode* node)
  {
      parent=node;
  }
  void setTypeofNode(NODETYPE type)
  {
    typeofNode=type;
  }
  NODETYPE getTypeofNode()
  {
    return typeofNode;
  }
  ASTNode* getParent()
  {
        return parent;
  }


};

#endif