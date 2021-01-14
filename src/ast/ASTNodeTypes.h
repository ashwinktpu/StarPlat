#include "ASTNode.h"
#include <string>
#include <list>


/*class declaration for each node type. Incomplete at the moment.*/


struct argument
{
  Expression* expression=NULL;
  assignment* assignExpr=NULL;
  bool expressionflag=false;
  bool assign=false;
};
struct tempNode
{
   reductionCall* reducCall=NULL;
   Expression* exprVal=NULL;

};

class paramList
{
  public:
  list<formalParam*> PList;
};

class argList
{
  public:
  list<argument> AList;
};

class ASTNodeList
{
  public:
  list<ASTNode*> ASTNList;
};


class Identifier:public ASTNode

{
  private:
  char* identifier;
  int accessType;

  public: 
  static Identifier* createIdNode(char* id)
   {
     Identifier* idNode=new Identifier();
     idNode->identifier=id;
     idNode->accessType=0;
     idNode->setTypeofNode("ID");
     return idNode;

   }

   int getAccessType()
   {
     return accessType;
   }

   char* getIdentifier()
   {
     return identifier;
   }


};

class PropAccess:public ASTNode

{
  private:
  Identifier* identifier1;
  Identifier* identifier2;
  int accessType;

  public: 
  static PropAccess* createPropAccessNode(Identifier* id1, Identifier* id2)
   {
     PropAccess* propAccessNode=new PropAccess();
     propAccessNode->identifier1=id1;
     propAccessNode->identifier2=id2;
     propAccessNode->accessType=1;
     propAccessNode->setTypeofNode("PROPACCESS");
     return propAccessNode;

   }

   int getAccessType()
   {
     return accessType;
   }

   Identifier* getIdentifier1()
   {
     return identifier1;
   }
    Identifier* getIdentifier2()
   {
     return identifier2;
   }


};

class Function:public ASTNode
{  
  private:

  Identifier* functionId;
  list<formalParam*> paramList;
  blockStatement* blockstmt;

  public:

  static Function* createFunctionNode(Identifier* funcId,list<formalParam*> paramList)
  {
      Function* func=new Function();
      func->functionId=funcId;
      func->paramList=paramList;
      return func;

  }
  
   void setBlockStatement(blockStatement* blockStmtSent)
   {
     blockstmt=blockStmtSent;
   }
   
   Identifier* getIdentifier()
   {
      return functionId;
   }
   list<formalParam*> getParamList()
   {
     return paramList;
   }

   blockStatement* getBlockStatement()
   {
     return blockstmt;
   }


};
class formalParam:public ASTNode
{
  private:
  Type* type;
  Identifier* identifier;


  public:
  formalParam()
  {
      type=NULL;
      identifier=NULL;
      
  }

  formalParam(Type* typeSent,Identifier* identifierSent)
  {
      type=typeSent;
      identifier=identifierSent;
      type->setParent(this);
   
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
  int typeId;
  int rootType;
  Identifier* TargetGraph;
  Type* innerTargetType;
  Identifier* sourceGraph;
  

 Type()
 {
     typeId=-1;
     TargetGraph=NULL;
     innerTargetType=NULL;
     sourceGraph=NULL;
 }
   
  static Type* createForPrimitive(int typeIdSent,int rootTypeSent)
  {
     Type* type=new Type();
     type->typeName=typeNameSent;
     type->rootType=rootTypeSent;
     return type;

  }

  static Type* createForGraphType(int typeIdSent,int rootTypeSent, Identifier* TargetGraphSent)
  {
       Type* type=new Type();
       type->typeId=typeIdSent;
       type->rootType=rootTypeSent;
       type->TargetGraph=TargetGraphSent;
       return type;  
  }

  
  static Type* createForCollectionType(int typeIdSent,int rootTypeSent, Identifier* TargetGraphSent)
  {
       Type* type=new Type();
       type->typeId=typeIdSent;
       type->rootType=rootTypeSent;
       type->TargetGraph=TargetGraphSent;
       return type;  
  }
  static Type* createForPropertyType(int typeIdSent,int rootTypeSent, Type* innerTargetTypeSent)
  {
       Type* type=new Type();
       type->typeId=typeIdSent;
       type->rootType=rootTypeSent;
       type->innerTargetType=innerTargetTypeSent;
       return type;  
  }
  static Type* createForNodeEdgeType(int typeIdSent,int rootTypeSent)
  {
    Type* type=new Type()
    type->typeId=typeIdSent;
    type->rootType=rootTypeSent;
    return type;
  }
  int getRootType()
  {
    return rootType;
  }

  int gettypeId()
  {
    return typeId;
  }
  bool isNodeEdgeType()
    {
         if(rootType==5)
           return true;
         else
           return false;
         
    }

  void addSourceGraph(Identifier* id)
  {
    sourceGraph=id;
  }  
  Type* getInnerTargetType()
  {
    return innerTargetType;
  }
  Identifier* getTargetGraph()
  {
    return TargetGraph;
  }
  Identifier* getSourceGraph()
  {
    return sourceGraph;
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
        newBlock->statementType="BlockStaement";
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
    Type* getType()
    {
      return type;
    }
    Identifier* getdeclId()
    {
      return identifier;
    }
    Expression* getExpressionAssigned()
    {
      return exprAssigned;
    }

  };
  class assignment:public statement
  {
     private:
     Identifier* identifier;
     PropAccess* propId;
     Expression* exprAssigned;
     int lhsType;

     public:
     assignment()
    {
        identifier=NULL;
        propId=NULL;
        exprAssigned=NULL;
        statementType="assignment";
    }

     static assignment* id_assignExpr(Identifier* identifierSent,Expression* expressionSent)
     {
            assignment* assign=new assignment();
            assign->identifier=identifierSent;
            assign->exprAssigned=expressionSent;
            assign->lhsType=1;
            return assign;
         

     }
      static assignment* prop_assignExpr(PropAccess* propId,Expression* expressionSent)
     {
            assignment* assign=new assignment();
            assign->propId=propId;
            assign->exprAssigned=expressionSent;
            assign->lhsType=2;
            return assign;
         

     }
     
     Identifier* getId()
     {
       return identifier;
     }
     
     PropAccess* getPropId()
     {
       return propId;
     }
     Expression* getExpr()
     {
       return exprAssigned;
     }
     int getLhsType()
     {
       return lhsType;
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
      new_dowhileStmt->iterCondition=iterConditionSent;
      new_dowhileStmt->body=bodySent;
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
    Identifier* source;
    PropAccess* sourceProp;
    proc_callExpr*  extractElemFunc;
    statement* body;
    Expression* filterExpr;
    bool isSourceId;
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
      isSourceId=false;
    }

    static forallStmt* createforallStmt(Identifier* iterator,Identifier* sourceGraph,proc_callExpr* extractElemFunc,statement* body,Expression* filterExpr,bool isforall)
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
    static forallStmt* createforForStmt(Identifier* iterator,Identifier* source,statement* body,bool isforall)
    {
      forallStmt* new_forallStmt=new forallStmt();
      new_forallStmt->iterator=iterator;
      new_forallStmt->source=source;
      new_forallStmt->body=body;
      new_forallStmt->isforall=isforall;
      return new_forallStmt;
    }
     static forallStmt* id_createforForStmt(Identifier* iterator,Identifier* source,statement* body,bool isforall)
    {
      forallStmt* new_forallStmt=new forallStmt();
      new_forallStmt->iterator=iterator;
      new_forallStmt->source=source;
      new_forallStmt->body=body;
      new_forallStmt->isforall=isforall;
      isSourceId=true;
      return new_forallStmt;
    }
   
    static forallStmt* propId_createforForStmt(Identifier* iterator,PropAccess* source,statement* body,bool isforall)
    {
      forallStmt* new_forallStmt=new forallStmt();
      new_forallStmt->iterator=iterator;
      new_forallStmt->sourceProp=source;
      new_forallStmt->body=body;
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

    static fixedPointStmt* createforfixedPointStmt(Expression* convergeExpr,statement* body)
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
    Expression* booleanExpr;
    Expression* filterExpr;
    statement* body;

    public: 
    iterateReverseBFS()
    {
      filterExpr=NULL;
      body=NULL;
    } 

    static iterateReverseBFS* nodeForRevBFS(zExpression* booleanExpr,Expression* filterExpr,statement* body)
    {
      iterateReverseBFS* new_revBFS=new iterateReverseBFS();
      new_revBFS->booleanExpr=booleanExpr;
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
      statementType="ProcCallStatement";
    }

    static proc_callStmt* nodeForCallStmt(Expression* procCall)
    {
      proc_callStmt* procCallStmtNode=new proc_callStmt();
      procCallStmtNode->procCall=(proc_callExpr*)procCall;

      return procCallStmtNode;
    }

    proc_callExpr* getProcCallExpr()
    {
      return procCall;
    }

};
  class Expression:public ASTNode
  {
    private:
    Expression* left;
    Expression* right;
    long integerConstant;
    double doubleConstant;
    bool booleanConstant;
    bool infinityType;
    int operatorType;
    int typeofExpr;
    Identifier* id;
    PropAccess* propId;
    
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
      logicalExpr->typeofExpr=2;

       return logicalExpr;

    }

    static Expression* nodeForIntegerConstant(long integerValue)
    {
       Expression* integerConstantExpr=new Expression();
       integerConstantExpr->integerConstant=integerValue;
       integerConstantExpr->typeofExpr=3;
       return integerConstantExpr;

    }

    static Expression* nodeForDoubleConstant(long doubleValue)
    {
       Expression* doubleConstantExpr=new Expression();
       doubleConstantExpr->doubleConstant=doubleValue;
       doubleConstantExpr->typeofExpr=3;
       return doubleConstantExpr;

    }
     static Expression* nodeForBooleanConstant(bool boolValue)
    {
       Expression* boolExpr=new Expression();
       boolExpr->booleanConstant=boolValue;
       boolExpr->typeofExpr=3;
       return boolExpr;

    }
    
    
     static Expression* nodeForBooleanConstant(bool boolValue)
    {
       Expression* boolExpr=new Expression();
       boolExpr->booleanConstant=boolValue;
       boolExpr->typeofExpr=3;
       return boolExpr;

    }

     static Expression* nodeForInfinity(bool infinityValue)
    {
       Expression* infinityExpr=new Expression();
       infinityExpr->infinityType=infinityValue;
       infinityExpr->typeofExpr=3;
       return infinityExpr;

    }
    
     static Expression* nodeForIdentifier(Identifier* id)
    {
       Expression* idExpr=new Expression();
       idExpr->id=id;
       idExpr->typeofExpr=4;
       return idExpr;

    }
      static Expression* nodeForPropAccess(PropAccess* propId)
    {
       Expression* propIdExpr=new Expression();
       propIdExpr->propId=propId;
       propIdExpr->typeofExpr=5;
       return propIdExpr;

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
  class reductionCall:public ASTNode
  {
    int reductionType;
     list<argument> argList;

     public:
     reductionCall()
     {
       reductionType=0;
     }
     static reductionCall* nodeForReductionCall(int reduceType,list<argument> argList)
     {
       reductionCall* reduceC=new reductionCall();
       reduceC->reductionType=reduceType;
       reduceC->argList=argList;
       return reduceC;
     }

     int getReductionType()
     {
       return reductionType;
     }

     list<argument> getargList()
     {
       return argList;
     }

};

class reductionCallStmt:public statement
  {
     private:
     Identifier* id;
     PropAccess* propAccessId;
     list<ASTNode*> leftList;
     reductionCall* reducCall;
     Expression* exprVal;
     int lhsType;
     
     public:
     reductionCallStmt()
     {
       id=NULL;
       propAccessId=NULL;
       reducCall=NULL;
       exprVal=NULL;
     }

     static reductionCallStmt* id_reducCallStmt(Identifier* id,reductionCall* reducCall)
     {
       reductionCallStmt* reducCallStmtNode=new reductionCallStmt();
       reducCallStmtNode->id=id;
       reducCallStmtNode->reducCall=reducCall;
       reducCallStmtNode->lhsType=1;
       return reducCallStmtNode;
     }
    
     static reductionCallStmt* propId_reducCallStmt(PropAccess* propId,reductionCall* reducCall)
     {
       reductionCallStmt* reducCallStmtNode=new reductionCallStmt();
       reducCallStmtNode->propAccessId=propId;
       reducCallStmtNode->reducCall=reducCall;
       reducCallStmtNode->lhsType=2;
       return reducCallStmtNode;
     }
      
     static reductionCallStmt* leftList_reducCallStmt(list<ASTNode*> llist,reductionCall* reducCall,Expression* exprVal)
     {
       reductionCallStmt* reducCallStmtNode=new reductionCallStmt();
       reducCallStmtNode->leftList=llist;
       reducCallStmtNode->reducCall=reducCall;
       reducCallStmtNode->lhsType=3;
       reducCallStmtNode->exprVal=exprVal;
       return reducCallStmtNode;
     }
    
    int getLhsType()
      {
        return lhsType;

      }

    reductionCall* getReducCall()
    {
      return reducCall;
    }  

    Identifier* getLeftId()
    {
      return id;
    }

    PropAccess* getPropAccess()
    {
       return propAccessId;
        
    }
    list<ASTNode*> getLeftList()
    {
      return leftList;
    }
    Expression* getExprVal()
    {
      return exprVal;
    }


};