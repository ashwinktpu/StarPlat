#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include "../maincontext/enum_def.hpp"
#include "../symbolutil/SymbolTable.h"
#include<assert.h>


using namespace std;

class ASTNode
{
  protected:
  ASTNode* parent;
  //string typeofNode;
  NODETYPE typeofNode;
  SymbolTable* var_symbTab;
  SymbolTable* prop_symbTab;
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



};

#endif
