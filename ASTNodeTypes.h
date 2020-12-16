#include<ASTNode.h>
#include<string>
#include <list>

using namespace std;
/*class declaration for each node type. Incomplete at the moment.*/


struct argument
{
  Expression* expression;
  assignment* assignExpr;
  bool expression=false;
  bool assign=false;
};

class Function:public ASTNode
{
  Identifier* functionId;
  list<formalParam*> paramList;

  static Function* createFunctionNode()
  {
      functionId=NULL;

  }
  void setFunctionId(Identifier* id)
   {
     functionId=id;
   }

   void addParam(formalParam* param)
   {
     paramList.push_back(param);
   }
   
   Identifier* getIdentifier()
   {
      return functionId;
   }
   list<formalParam*> getParamList()
   {
     return paramList;
   }


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
  protected: 
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
  
  class blockStatement:public statement
  {
     private:
     list<statement*> statements;

     public: 
      static blockStatement* createnewBlock()
      {
        blockStatement* newBlock=new blockStatement();
        return newBlock;
      }

      void addStmtToBlock(statement* stmt)
          {
            statements.push_back(stmt);
          }     
      list<statement*> returnStatements()
      {
        return statements;
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
        statementType="declaration";
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
  class assignment:public statement
  {
     private:
     Identifier* identifier;
     Expression* exprAssigned;

     public:
     assignment()
    {
        identifier=NULL;
        exprAssigned=NULL;
        statementType="assignment";
    }

     static assignment* assign_expr(Identifier* identifierSent,Expression* expressionSent)
     {
            assignment* assign=new assignment();
            assign->identifier=identifierSent;
            assign->exprAssigned=expressionSent;
            return assign;
         

     }

  };
class whileStmt:public statement
{
  private:
  Expression* iterCondition;
  blockStatement* body;

  public:

    whileStmt()
    {
      iterCondition=NULL;
      body=NULL;
      statementType="WhileStmt";
    }

    static whileStmt* create_whileStmt(Expression* iterConditionSent,blockStatement* bodySent)
    {  
      whileStmt* new_whileStmt=new whileStmt();
      new_whileStmt->iterCondition=iterConditionSent;
      new_whileStmt->body=bodySent;
      return new_whileStmt;
    }

    Expression* getCondition()
    {
      return iterCondition;
    }
   
    statement* getBody()
    {
      return body;
    }

  }; 
  class dowhileStmt:public statement
{
  private:
  Expression* iterCondition;
  blockStatement* body;

  public:

    dowhileStmt()
    {
      iterCondition=NULL;
      body=NULL;
      statementType="WhileStmt";
    }

    static dowhileStmt* create_dowhileStmt(Expression* iterConditionSent,blockStatement* bodySent)
    {  
      dowhileStmt* new_dowhileStmt=new dowhileStmt();
      new_whileStmt->iterCondition=iterConditionSent;
      new_whileStmt->body=bodySent;
      return new_dowhileStmt;
    }

    Expression* getCondition()
    {
      return iterCondition;
    }
   
    statement* getBody()
    {
      return body;
    }

  };

  class forallStmt:public statement
  {
     
    private:
    Identifier* iterator;
    Identifier* sourceGraph;
    statement*  extractElemFunc;
    statement* body;
    Expression* filterExpr;
    bool isforall;
    
    public:
    forallStmt()
    { 
      iterator=NULL;
      sourceGraph=NULL;
      extractElemFunc=NULL;
      body=NULL;
      filterExpr=NULL;
      statementType="ForAllStmt";
      isforall=false;
    }

    static forallStmt* createforallStmt(Identifier* iterator,Identifier* sourceGraph,statement* extractElemFunc,statement* body,Expression* filterExpr,bool isforall)
    { 
      forallStmt* new_forallStmt=new forallStmt();
      new_forallStmt->iterator=iterator;
      new_forallStmt->sourceGraph=sourceGraph;
      new_forallStmt->extractElemFunc=extractElemFunc;
      new_forallStmt->body=body;
      new_forallStmt->filterExpr=filterExpr;
      new_forallStmt->isforall=isforall;
      return new_forallStmt;
    }
     


};

class fixedPointStmt:public statement
  {
     
    private:
    Expression* convergeExpr;
    statement* body;
    
    public:
    fixedPointStmt()
    { 
      convergeExpr=NULL;
      body=NULL;
      statementType="FixedPointStmt";
    }

    static fixedPointStmt* createforallStmt(Expression* convergeExpr,statement* body)
    { 
      fixedPointStmt* new_fixedPointStmt=new fixedPointStmt();
      new_fixedPointStmt->convergeExpr=convergeExpr;
      new_fixedPointStmt->body=body;
      return new_fixedPointStmt;
    }
     


};

  class ifStmt:public statement
 {
  private:
  Expression* condition;
  statement*  ifBody;
  statement*  thenBody;

  public:

    ifStmt()
    {
      condition=NULL;
      ifBody=NULL;
      thenBody=NULL;
      statementType="IfStmt";
    }

    static ifStmt* create_ifStmt(Expression* condition,statement* ifBodySent,statement* thenBodySent)
    {  
      ifStmt* new_ifStmt=new ifStmt();
      new_ifStmt->condition=condition;
      new_ifStmt->ifBody=ifBodySent;
      new_ifStmt->thenBody=thenBodySent;

      return new_ifStmt;
    }

    Expression* getCondition()
    {
      return condition;
    }
   
    statement* getIfBody()
    {
      return ifBody;
    }
    statement* getElseBody()
    {
      return thenBody;
    }

  }; 

  class iterateBFS:public statement
  {
      Identifier* iterator;
      Identifier* rootNode;
      Expression* filterExpr;
      statement* body;
      iterateReverseBFS* revBFS;

      iterateBFS()
      {
        iterator=NULL;
        rootNode=NULL;
        filterExpr=NULL;
        body=NULL;
        revBFS=NULL;
        statementType="IterateInBFS";
      }
    
      static iterateBFS* nodeForIterateBFS(Identifier* iterator,Identifier* rootNode,Expression* filterExpr,statement* body,iterateReverseBFS* revBFS)
      {
        iterateBFS* new_iterBFS=new iterateBFS();
        new_iterBFS->iterator=iterator;
        new_iterBFS->rootNode=rootNode;
        new_iterBFS->filterExpr=filterExpr;
        new_iterBFS->body=body;
        new_iterBFS->revBFS=revBFS;
        return new_iterBFS;
      }


  };
  class iterateReverseBFS:public ASTNode
  { 
    private:
    Expression* filterExpr;
    statement* body;

    public: 
    iterateReverseBFS()
    {
      filterExpr=NULL;
      body=NULL;
    } 

    static iterateReverseBFS* nodeForRevBFS(Expression* filterExpr,statement* body)
    {
      iterateReverseBFS* new_revBFS=new iterateReverseBFS();
      new_revBFS->filterExpr=filterExpr;
      new_revBFS->body=body;
      return new_revBFS;
    }



  };

  class proc_callStmt:public statement
  {
    private:
    proc_callExpr* procCall;

    public:
    proc_callStmt()
    {
      procCall=NULL;
    }

    static proc_callStmt* nodeForCallStmt(Expression* procCall)
    {
      proc_callStmt* procCallStmtNode=new proc_callStmt();
      procCallStmtNode->procCall=(proc_callExpr*)procCall;

      return procCallStmtNode;
    }

};
  class Expression:public ASTNode
  {
    private:
    Expression* left;
    Expression* right;
    int operatorType;
    int typeofExpr;
    
    static Expression* nodeForArithmeticExpr(Expression* left,Expression* right,int arithmeticOperator)
    {   
      Expression* arithmeticExpr=new Expression();
      arithmeticExpr->left=left;
      arithmeticExpr->right=right;
      arithmeticExpr->operatorType=arithmeticOperator;
      arithmeticExpr->typeofExpr=0;

       return arithmeticExpr;

    }

    static Expression* nodeForRelationalExpr(Expression* left,Expression* right,int relationalOperator)
    {   
      Expression* relationalExpr=new Expression();
      relationalExpr->left=left;
      relationalExpr->right=right;
      relationalExpr->operatorType=relationalOperator;
      relationalExpr->typeofExpr=1;

       return relationalExpr;

    }

    static Expression* nodeForLogicalExpr(Expression* left,Expression* right,int logicalOperator)
    {   
      Expression* logicalExpr=new Expression();
      logicalExpr->left=left;
      logicalExpr->right=right;
      logicalExpr->operatorType=logicalOperator;
      logicalExpr->typeofExpr=1;

       return logicalExpr;

    }

   


  };

  class proc_callExpr:public Expression
  {
    private:
    Identifier* id1;
    Identifier* id2;
    list<argument> argList;
    
    public:
    proc_callExpr()
    {
      id1=NULL;
      id2=NULL;
    }

    static proc_callExpr* nodeForProc_Call(Identifier* id1,Identifier* id2,list<argument> argList)
    {
          proc_callExpr* procExpr=new proc_callExpr();
          procExpr->id1=id1;
          procExpr->id2=id2;
          procExpr->argList=argList;
          return procExpr;


    }


  };



