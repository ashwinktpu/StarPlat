#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include "../maincontext/enum_def.hpp"
#include "../symbolutil/SymbolTable.h"
#include<assert.h>


using namespace std;

class NodeBlockData;

class ASTNode
{
  protected:
  ASTNode* parent;
  //string typeofNode;
  NODETYPE typeofNode;
  SymbolTable* var_symbTab;
  SymbolTable* prop_symbTab;
  NodeBlockData* blockData;
  bool inParallel;

  public:
  ASTNode()
  {
      parent=NULL;
      inParallel = false;
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

 void createSymbTab()
  {
      var_symbTab=new SymbolTable(this,0);
      prop_symbTab=new SymbolTable(this,1);
  }

  SymbolTable* getVarSymbT()
  {
    return var_symbTab;
  }
  SymbolTable* getPropSymbT()
  {
    return prop_symbTab;
  }

  void setBlockData(NodeBlockData* blockData)
  {
    this->blockData = blockData;
  }

  NodeBlockData* getBlockData()
  {
    return blockData;
  }

  void setInParallel(bool inParallel = true)
  {
    this->inParallel = inParallel;
  }

  bool isInParallel()
  {
    return inParallel;
  }

};

#endif
