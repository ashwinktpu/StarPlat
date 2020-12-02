#include<ASTNode.h>
#include<string>

using namespace std;

class Function:public ASTNode
{
  


};
class formalParam:public ASTNode
{
  private:
  Type* type;
  Identifier* identifier;

  formalParam()
  {

  }

  formalParam(Type* typeSent,Identifier* identifierSent)
  {
      type=typeSent;
      identifier=identifierSent;
      type->setParent(this);
      identifier->setParent(this);
  }
  
  Type* getType()
  {
      return type;
  }
  
  Identifier* getIdentifier()
  {
      return identifier;
  }

};
class Type:public ASTNode
{
  string typeName;
  int rootType;
  Identifier* TargetGraph;
  Type* innerTargetType;
  

 Type()
 {
     typeName=" ";
     TargetGraph=NULL;
     innerTargetType=NULL;
 }
   
  static Type* createForPrimitive(string typeNameSent,int rootTypeSent)
  {
     Type* type=new Type();
     type->typeName=typeNameSent;
     type->rootType=rootTypeSent;
     return type;

  }

  static Type* createForGraphType(string typeNameSent,int rootTypeSent, Identifier* TargetGraphSent)
  {
       Type* type=new Type();
       type->typeName=typeNameSent;
       type->rootType=rootTypeSent;
       type->TargetGraph=TargetGraphSent;
       return type;  
  }

  
  static Type* createForCollectionType(string typeNameSent,int rootTypeSent, Identifier* TargetGraphSent)
  {
       Type* type=new Type();
       type->typeName=typeNameSent;
       type->rootType=rootTypeSent;
       type->TargetGraph=TargetGraphSent;
       return type;  
  }
  static Type* createForPropertyType(string typeNameSent,int rootTypeSent, Type* innerTargetTypeSent)
  {
       Type* type=new Type();
       type->typeName=typeNameSent;
       type->rootType=rootTypeSent;
       type->innerTargetType=innerTargetTypeSent;
       return type;  
  }
  

};
class statement:public ASTNode
{
  private: 
  string statementType;

  public:
  statement();
  statement(string statementTypeSent)
  {
     statementType=statementTypeSent;

  }

  string getType()
  {
      return statementType;
  }
  
  };

  class declaration:public statement
  {
    private:
    Type* type;
    Identifier* identifier;
    Expression* exprAssigned;

    public:
    declaration()
    {
        type=NULL;
        identifier=NULL;
        exprAssigned=NULL;
    }

    static declaration* normal_Declaration(Type* typeSent,Identifier* identifierSent)
    {
          declaration* decl=new declaration();
          decl->type=typeSent;
          decl->identifier=identifierSent;
          return decl;
    }

    static declaration* assign_Declaration(Type* typeSent,Identifier* identifierSent,Expression* expression)
    {     declaration* decl=new declaration();
          decl->type=typeSent;
          decl->identifier=identifierSent;
          decl->exprAssigned=expression;
          return decl;

    }

  };
  class assignment:public ASTNode
  {
     private:
     Identifier* identifier;
     Expression* exprAssigned;

   

  };

