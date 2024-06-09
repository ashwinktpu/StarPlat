#ifndef AST_HELPER_H
#define AST_HELPER_H

#include "../maincontext/MainContext.hpp"
#include  "ASTNode.hpp"
#include  "../maincontext/enum_def.hpp"
#include<iostream>
#include<assert.h>

using namespace std;


/*TO be implemented. It will contain functions that will be called by action part of Parser  for building the nodes of AST*/


extern FrontEndContext frontEndContext;  


class Util
{

public: 


static void addFuncToList(ASTNode* func)
{
   Function* funcNode=(Function*)func;

    frontEndContext.addFuncToList(funcNode);
    frontEndContext.incrementCurrentFuncCount(); 
}

static void setCurrentFuncType(int funcType)
{
    frontEndContext.setCurrentFuncType(funcType);
}




static void resetTemp(vector<Identifier*>& tempIds)
{
  int currentFuncType =  frontEndContext.getCurrentFuncType();
  printf("currentFuncType check!!%d\n",currentFuncType);
  for(Identifier* id:tempIds)
     {  
         printf("tempID %s funccount %d\n",id->getIdentifier(),frontEndContext.getCurrentFuncCount());
        graphId[currentFuncType][frontEndContext.getCurrentFuncCount()].push_back(id);
     }
  

}

static void storeGraphId(Identifier* id)
{
   int currentFuncType = frontEndContext.getCurrentFuncType();
   if(currentFuncType==GEN_FUNC)
     {
         graphId[0][frontEndContext.getCurrentFuncCount()].push_back(id);
     }
     else if(currentFuncType==STATIC_FUNC)
         {
             graphId[1][frontEndContext.getCurrentFuncCount()].push_back(id);
                 
         }
    else if(currentFuncType==INCREMENTAL_FUNC)
          {
              graphId[2][frontEndContext.getCurrentFuncCount()].push_back(id);
            
          }
    else if(currentFuncType==DECREMENTAL_FUNC)
          {
              graphId[3][frontEndContext.getCurrentFuncCount()].push_back(id);
          }   
    else if(currentFuncType == DYNAMIC_FUNC)
          {
               graphId[4][frontEndContext.getCurrentFuncCount()].push_back(id);
          }      


          //        

}

static int getCurrentFuncType()
{
    return frontEndContext.getCurrentFuncType();
}

static ASTNode* createFuncNode(ASTNode* id,list<formalParam*> formalParamList)
{
  Identifier* funcId=(Identifier*)id;
  //Type* retType = (Type*)retType;
  
  Function* functionNode=Function::createFunctionNode(funcId,formalParamList/**, retType*/);
  return functionNode;

}

static ASTNode* createStaticFuncNode(ASTNode* id,list<formalParam*> formalParamList/*, ASTNode* retType*/)
{ 
  Identifier* staticFuncId=(Identifier*)id;
  //Type* retType = (Type*)retType;
  
  Function* staticFuncNode=Function::createStaticFunctionNode(staticFuncId,formalParamList/*, retType*/);
  return staticFuncNode;
   
}



static ASTNode* createDynamicFuncNode(ASTNode* id,list<formalParam*> formalParamList)
{ 
  Identifier* dynFuncId=(Identifier*)id;
  //Type* retType = (Type*)retType;

  Function* dynFuncNode=Function::createDynamicFunctionNode(dynFuncId,formalParamList/*, retType*/);
  return dynFuncNode;
   
}

static ASTNode* createIncrementalNode(list<formalParam*> formalParamList)
{ 
  //Type* retType = (Type*)retType;
  Function* incrementalNode = Function::createIncrementalNode(formalParamList/*, retType*/);

  return incrementalNode;
   
}
static ASTNode* createDecrementalNode(list<formalParam*> formalParamList)
{ 
 // Type* retType = (Type*)retType;
  Function* decrementalNode = Function::createDecrementalNode(formalParamList/*, retType*/);
  return decrementalNode;
   
}





static void createNewBlock()
{
    blockStatement* blockStatementNode=blockStatement::createnewBlock();
    frontEndContext.startBlock(blockStatementNode);

}


static ASTNode* finishBlock()
{
     blockStatement* blockStatementNode=frontEndContext.getCurrentBlock();

    frontEndContext.endBlock();

    return blockStatementNode;

    }
static void addToBlock(ASTNode* statementNode)
{  // cout<<"Inside ADD BLOCK"<<statementNode<<"\n";
    if(statementNode!=NULL)
    {
        statement* nodeForStatement=(statement*)statementNode;
        blockStatement* currentBlock=frontEndContext.getCurrentBlock();
        currentBlock->addStmtToBlock(nodeForStatement);
    }


}
static ASTNode* createIdentifierNode(const char* idName)
{
    Identifier* idNode;
    idNode=Identifier::createIdNode(idName);
   // cout<<"IDENTIFIER VALUE"<<idNode->getIdentifier()<<"\n";
    return idNode;
}
static paramList* createPList(ASTNode* fParam)
{    paramList* pList=new paramList();
     pList->PList.push_back((formalParam*)fParam);
     return pList;
}

static paramList* addToPList(paramList* pList,ASTNode* fparam)
{
    pList->PList.push_front((formalParam*)fparam);
    return pList;
}

static argList* createAList(argument* arg)
{
    argList* aList=new argList();
    aList->AList.push_back(arg);
    return aList;
}



static argList* addToAList(argList* aList,argument* arg)
{
    aList->AList.push_front(arg);
    return aList;

}



static ASTNodeList* addToNList(ASTNodeList* nodeList,ASTNode* node)
{
    nodeList->ASTNList.push_front(node);

    return nodeList;

}

static ASTNodeList* createNList(ASTNode* node)
{
    ASTNodeList* nodeList=new ASTNodeList();
    nodeList->ASTNList.push_back(node);

    return nodeList;


}

static ASTNode* createParamNode(ASTNode* type,ASTNode* id)
{
  //~ Identifier* paramId=(Identifier*)id;
  // cout<<"PARAMID NODE VALUE "<<paramId->getIdentifier()<<"\n";
  formalParam* formalParamNode=formalParam::createFormalParam((Type*)type,(Identifier*)id);
  return formalParamNode;

}

static ASTNode* createNormalDeclNode(ASTNode* type,ASTNode* id)
{
    declaration* declNode=declaration::normal_Declaration((Type*)type,(Identifier*)id);
    return declNode;
}

static ASTNode* createAssignedDeclNode(ASTNode* type,ASTNode* id,ASTNode* exprAssigned)
{
    declaration* declNode=declaration::assign_Declaration((Type*)type,(Identifier*)id,(Expression*)exprAssigned);
    return declNode;
}
static ASTNode* createPrimitiveTypeNode(int typeId)
{  // cout<<"Inside Func";
    Type* typeNode=Type::createForPrimitive(typeId,1);

    return typeNode;

}
static ASTNode* createGraphTypeNode(int typeId,Identifier* targetGraph)
{
    Type* typeNode=Type::createForGraphType(typeId,2,targetGraph);
    return typeNode;

}
static ASTNode* createCollectionTypeNode(int typeId,ASTNode* targetGraph)
{
    Type* typeNode=Type::createForCollectionType(typeId,3,(Identifier*)targetGraph);
    return typeNode;

}
static ASTNode* createPropertyTypeNode(int typeId,ASTNode* innerTargetType)
{
    Type* typeNode=Type::createForPropertyType(typeId,4,(Type*)innerTargetType);
    return typeNode;

}
static ASTNode* createNodeEdgeTypeNode(int typeId)
{
    Type* typeNode=Type::createForNodeEdgeType(typeId,5);
    return typeNode;

}

static ASTNode * createAddAssignment (ASTNode* indexExpr, int reduction_op, ASTNode* rhs) {
  printf ("called to creation of dummy node\n") ;
  return NULL ;
}

static ASTNode* createAssignmentNode(ASTNode* leftSide, ASTNode* rhs)
{  
     assignment* assignmentNode;
    if(leftSide->getTypeofNode() == NODE_ID)
    {
      assignmentNode=assignment::id_assignExpr((Identifier*)leftSide,(Expression*)rhs);
    }
    if(leftSide->getTypeofNode() == NODE_PROPACCESS)
    {
        assignmentNode=assignment::prop_assignExpr((PropAccess*)leftSide,(Expression*)rhs);
    }
    if(leftSide->getTypeofNode() == NODE_EXPR){
      
       cout<<"INSIDE ASSIGNMENT***********EXPR"<<"\n";
       Expression* expr = (Expression*)leftSide;
       if(expr->getExpressionFamily() == EXPR_MAPGET) {
           
          cout<<"INSIDE MAPGET EXPR"<<"\n"; 
          assignmentNode = assignment::indexAccess_assignExpr((Expression*) leftSide, (Expression*)rhs);
         }
     }
    
    
    return assignmentNode;
}


static ASTNode* createNodeForProcCallStmt(ASTNode* procCall)
{
    statement* procCallStmt;
    procCallStmt=proc_callStmt::nodeForCallStmt((Expression*)procCall);
    bool value=procCallStmt->getTypeofNode()==NODE_PROCCALLSTMT;
    return procCallStmt;
}

static ASTNode* createNodeForUnaryStatements(ASTNode* unaryExpr)
{
  statement* unaryStmt;
  Expression* unaryExprNode=(Expression*)unaryExpr;
  Expression* leftSideExpr=unaryExprNode->getUnaryExpr();
  assert(leftSideExpr->getExpressionFamily()==EXPR_ID||leftSideExpr->getExpressionFamily()==EXPR_PROPID);
  unaryStmt=unary_stmt::nodeForUnaryStmt((Expression*)unaryExpr);

  return unaryStmt;


}

static ASTNode* createNodeForProcCall(ASTNode* proc_callId,list<argument*> argList, ASTNode* indexExprSent)
{    
    proc_callExpr* proc_callExprNode;
   
   if(proc_callId->getTypeofNode()==NODE_ID) {
     
      if(indexExprSent != NULL){
         
        cout<<"ENTERED HERE FOR INDEXEXPR PROC CALL****"<<"\n";
         Expression* indexExpr = (Expression*)indexExprSent;
         proc_callExprNode = proc_callExpr::nodeForProc_Call(NULL,NULL,(Identifier*)proc_callId,argList, indexExpr);
     
      }
      else {
        proc_callExprNode = proc_callExpr::nodeForProc_Call(NULL,NULL,(Identifier*)proc_callId,argList, NULL);
      }
      
    }
    if(proc_callId->getTypeofNode()==NODE_PROPACCESS)
    {
      PropAccess* propAccessId=(PropAccess*)proc_callId;

      if(indexExprSent != NULL) {
       
       Expression* indexExpr = (Expression*)indexExprSent; 
       proc_callExprNode=proc_callExpr::nodeForProc_Call(propAccessId->getIdentifier1(),NULL,propAccessId->getIdentifier2(),argList, indexExpr);

      }
      else {
      proc_callExprNode=proc_callExpr::nodeForProc_Call(propAccessId->getIdentifier1(),NULL,propAccessId->getIdentifier2(),argList, NULL);
      }

    }


    
    return proc_callExprNode;
}

static ASTNode* createContainerTypeNode(int typeId, ASTNode* innerType, list<argument*> argList, ASTNode* innerTypeSize){

Type* containerNode = Type::createForContainerType(typeId, (Type*) innerType, argList, (Type*) innerTypeSize);

return containerNode;

}

static ASTNode* createNodeMapTypeNode(int typeId, ASTNode* elemType){

Type* nodeMapNode = Type::createForNodeMapType(typeId, (Type*) elemType);

return nodeMapNode;
}

static ASTNode* createHashMapTypeNode(int typeId, ASTNode* keyType, list<argument*> argList, ASTNode* valType){

Type* hashmapNode = Type::createForHashMapType(typeId, (Type*) keyType, argList, (Type*) valType);

return hashmapNode;

}

static ASTNode* createHashSetTypeNode(int typeId, ASTNode* keyType, list<argument*> argList, ASTNode* innerTypeSize){

Type* hashsetNode = Type::createForHashSetType(typeId, (Type*) keyType, argList, (Type*) innerTypeSize);

return hashsetNode;

}

static ASTNode* createNodeForArithmeticExpr(ASTNode* expr1,ASTNode* expr2,int operatorType)
{
    Expression* arithmeticExprNode=Expression::nodeForArithmeticExpr((Expression*)expr1,(Expression*)expr2,operatorType);
    return arithmeticExprNode;
}
static ASTNode* createNodeForRelationalExpr(ASTNode* expr1,ASTNode* expr2,int operatorType)
{
    Expression* relationalExprNode=Expression::nodeForRelationalExpr((Expression*)expr1,(Expression*)expr2,operatorType);
    return relationalExprNode;
}
static ASTNode* createNodeForLogicalExpr(ASTNode* expr1,ASTNode* expr2,int operatorType)
{
    Expression* logicalExprNode=Expression::nodeForLogicalExpr((Expression*)expr1,(Expression*)expr2,operatorType);
    return logicalExprNode;
}

static ASTNode* createNodeForUnaryExpr(ASTNode* expr,int operatorType)
{
    Expression* unaryExpr=Expression::nodeForUnaryExpr((Expression*)expr,operatorType);
    return unaryExpr;
}

static ASTNode* createNodeForIndexExpr(ASTNode* expr, ASTNode* indexExpr,int operatorType)
{
    Expression* indexExpression = Expression::nodeForIndexExpr((Expression*)expr, (Expression*) indexExpr,operatorType);
    return indexExpression;
}


static ASTNode* createNodeForIval(long value)
{
    Expression* exprIVal=Expression::nodeForIntegerConstant(value);
    return exprIVal;
}
static ASTNode* createNodeForFval(double value)
{
    Expression* exprFVal=Expression::nodeForDoubleConstant(value);
    return exprFVal;
}
static ASTNode* createNodeForBval(bool value)
{
    Expression* exprBVal=Expression::nodeForBooleanConstant(value);
    bool check=(exprBVal->getExpressionFamily()==EXPR_BOOLCONSTANT);
    return exprBVal;
}

static ASTNode* createNodeForINF(bool infinityFlag)
{
    Expression* exprINFVal = Expression::nodeForInfinity(infinityFlag);
    return exprINFVal;
}

/*static ASTNode* createNodeForChar(char charVal)
   {
     Expression* exprCharVal = Expression::nodeForChar(charVal);
     return exprCharVal;


   }*/


static ASTNode* createReturnStatementNode(ASTNode* returnExpression)
{
  statement* returnStmtNode;
  returnStmtNode = returnStmt::createNodeForReturnStmt((Expression*)returnExpression);

  return returnStmtNode;

}

static ASTNode* createBatchBlockStmt(ASTNode* updatesId, ASTNode* batchSizeExpr, ASTNode* blockStmts)
{
   statement* batchBlockStmtNode;  
   batchBlockStmtNode = batchBlock::createNodeForBatchBlock((Identifier*) updatesId, (Expression*)batchSizeExpr,(statement*)blockStmts);
  return batchBlockStmtNode; 
 
}

static ASTNode* createOnAddBlock(ASTNode* updateIterator, ASTNode* updateSource, ASTNode* updateFunc, ASTNode* blockStmts)
{
   statement* onAddBlockNode;
   onAddBlockNode = onAddBlock::createNodeForOnAddBlock((Identifier*)updateIterator, (Identifier*)updateSource, (proc_callExpr*)updateFunc,(statement*)blockStmts);

   return onAddBlockNode;
}

static ASTNode* createOnDeleteBlock(ASTNode* updateIterator, ASTNode* updateSource, ASTNode* updateFunc, ASTNode* blockStmts)
 {
   statement* onDeleteBlockNode;
   onDeleteBlockNode = onDeleteBlock::createNodeForOnDeleteBlock((Identifier*)updateIterator, (Identifier*)updateSource, (proc_callExpr*)updateFunc,(statement*)blockStmts);

   return onDeleteBlockNode;
}



static ASTNode* createNodeForId(ASTNode* node)
{  Expression* exprForId;
   if(node->getTypeofNode()==NODE_ID)
      {
         exprForId=Expression::nodeForIdentifier((Identifier*)node);

      }
    if(node->getTypeofNode()==NODE_PROPACCESS)
     {
         exprForId=Expression::nodeForPropAccess((PropAccess*)node);
     }
     return exprForId;

}
static ASTNode* createNodeForFixedPointStmt(ASTNode* fixedPointId,ASTNode* dependentProp,ASTNode* body)
{
    statement* fixedPointStmtNode;
    fixedPointStmtNode=fixedPointStmt::createforfixedPointStmt((Identifier*)fixedPointId,(Expression*)dependentProp,(blockStatement*)body);
    return fixedPointStmtNode;
}
static ASTNode* createNodeForWhileStmt(ASTNode* iterCondition,ASTNode* body)
{
    statement* whileStmtNode;
    whileStmtNode=whileStmt::create_whileStmt((Expression*)iterCondition,(blockStatement*)body);
    return whileStmtNode;
}
static ASTNode* createNodeForDoWhileStmt(ASTNode* iterCondition,ASTNode* body)
{
    statement* dowhileStmtNode;
    dowhileStmtNode=dowhileStmt::create_dowhileStmt((Expression*)iterCondition,(blockStatement*)body);
    return dowhileStmtNode;
}
static ASTNode* createNodeForIfStmt(ASTNode* iterCondition,ASTNode* thenBody,ASTNode* elseBody)
{
    statement* ifStmtNode;
    ifStmtNode=ifStmt::create_ifStmt((Expression*)iterCondition,(statement*)thenBody,(statement*)elseBody);
    return ifStmtNode;
}
static ASTNode* createNodeForForAllStmt(ASTNode* iterator,ASTNode* sourceGraph,ASTNode* extractElemFunc,ASTNode* filterExpr,ASTNode* body,bool isforall)
{
    statement* forallStmtNode;
    Identifier* id=(Identifier*)iterator;
    Identifier* id1=(Identifier*)sourceGraph;
     Expression* f=NULL;
    if(filterExpr!=NULL)
    {
     f=(Expression*)filterExpr;

    cout<<"CHECK FILTER TYPE"<<f->isRelational()<<"\n";
    }
    forallStmtNode=forallStmt::createforallStmt(id,id1,(proc_callExpr*)extractElemFunc,(statement*)body,(Expression*)filterExpr,isforall);
    return forallStmtNode;
}
static ASTNode* createNodeForForStmt(ASTNode* iterator,ASTNode* source,ASTNode* body,bool isforall)
{
    statement* forallStmtNode;
    Identifier* id=(Identifier*)iterator;

    if(source->getTypeofNode() == NODE_ID)
    forallStmtNode = forallStmt::id_createforForStmt(id,(Identifier*)source,(statement*)body,isforall);

    if(source->getTypeofNode() == NODE_PROPACCESS)
    forallStmtNode = forallStmt::propId_createforForStmt(id,(PropAccess*)source,(statement*)body,isforall);
   
    if(source->getTypeofNode() == NODE_EXPR){
       Expression* expr = (Expression*)source;
       if(expr->getExpressionFamily() == EXPR_MAPGET){

       forallStmtNode = forallStmt::indexExpr_createforForStmt(id, (Expression*)expr, (statement*)body, isforall);   

       }
    }

    return forallStmtNode;
}
static ASTNode* createNodeforReductionCall(int reductionOperationType,list<argument*> argList)
{
    reductionCall* reductionCallNode=reductionCall::nodeForReductionCall(reductionOperationType,argList);
    return reductionCallNode;
}
static ASTNode* createNodeForReductionStmt(ASTNode* leftSide,ASTNode* reductionCallNode)
{
    reductionCallStmt* reductionStmtNode;
    if(leftSide->getTypeofNode()==NODE_ID)
    {
        reductionStmtNode=reductionCallStmt::id_reducCallStmt((Identifier*)leftSide,(reductionCall*)reductionStmtNode);
    }
    if(leftSide->getTypeofNode()==NODE_PROPACCESS)
     {
         reductionStmtNode=reductionCallStmt::propId_reducCallStmt((PropAccess*)leftSide,(reductionCall*)reductionStmtNode);
     }
     return reductionStmtNode;
}
static ASTNode* createNodeForReductionStmtList(list<ASTNode*> leftList,ASTNode* reductionCallNode,list<ASTNode*>exprList)
{
    reductionCallStmt* reductionStmtNode;
    reductionStmtNode=reductionCallStmt::leftList_reducCallStmt(leftList,(reductionCall*)reductionCallNode,exprList);
    return reductionStmtNode;
}

static ASTNode* createNodeForReductionOpStmt(ASTNode* leftSide,int reduction_op,ASTNode* rightSide)
{
  reductionCallStmt* reductionStmtNode;
    if(leftSide->getTypeofNode()==NODE_ID)
    {
      reductionStmtNode=reductionCallStmt::id_reduc_opStmt((Identifier*)leftSide,reduction_op,(Expression*)rightSide);
    }
    if(leftSide->getTypeofNode()==NODE_PROPACCESS)
    {
      reductionStmtNode=reductionCallStmt::propId_reduc_opStmt((PropAccess*)leftSide,reduction_op,(Expression*)rightSide);
    }
    if(leftSide->getTypeofNode()==NODE_EXPR) 
    {

      reductionStmtNode=reductionCallStmt::container_reduc_opStmt((Expression*)leftSide,reduction_op,(Expression*)rightSide);
    }

    return reductionStmtNode;

}



static ASTNode* createPropIdNode(ASTNode* id1,ASTNode* id2)
{
    PropAccess* propIdNode;

    if(id2->getTypeofNode() == NODE_ID)
       propIdNode = PropAccess::createPropAccessNode((Identifier*)id1,(Identifier*)id2);
    if(id2->getTypeofNode() == NODE_EXPR)
       propIdNode = PropAccess::createPropAccessNode((Identifier*) id1, (Expression*) id2);

    return propIdNode;
}
static ASTNode* createIterateInReverseBFSNode( ASTNode* booleanExpr,/*ASTNode* filterExpr,*/ASTNode* body)
{
    iterateReverseBFS* iterateReverseBFSNode;
    iterateReverseBFSNode=iterateReverseBFS::nodeForRevBFS((Expression*)booleanExpr,/*(Expression*)filterExpr,*/(statement*)body);
    return iterateReverseBFSNode;
}
static ASTNode* createIterateInBFSNode(ASTNode* iterator,ASTNode* graphId,ASTNode* procCall,ASTNode* rootNode,ASTNode* filterExpr,ASTNode* body,ASTNode* revBFS)
{
    iterateBFS* iterateBFSNode;
    Identifier* id1=(Identifier*)iterator;
    Identifier* id2=(Identifier*)graphId;
    Identifier* id3=(Identifier*)rootNode;
    proc_callExpr* nodeCall = (proc_callExpr*)procCall;
    char* methodName = nodeCall->getMethodId()->getIdentifier();
    string methodString(methodName);
    assert(methodString.compare("nodes")==0);
    if(revBFS != NULL)
      iterateBFSNode=iterateBFS::nodeForIterateBFS(id1,id2,nodeCall,id3,(Expression*)filterExpr,(statement*)body,(iterateReverseBFS*)revBFS);
    else
      iterateBFSNode=iterateBFS::nodeForIterateBFS(id1,id2,nodeCall,id3,(Expression*)filterExpr,(statement*)body, NULL); 
    return iterateBFSNode;
}
};


#endif
