#include "../maincontext/MainContext.hpp"
#include  "ASTNode.hpp"
#include  "../maincontext/enum_def.hpp"
#include<iostream>

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
}

static ASTNode* createFuncNode(ASTNode* id,list<formalParam*> formalParamList)
{ 
  Identifier* funcId=(Identifier*)id;
  
  Function* functionNode=Function::createFunctionNode(funcId,formalParamList);
  return functionNode;
   
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
static ASTNode* createIdentifierNode(char* idName)
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
{   Identifier* paramId=(Identifier*)id;
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

static ASTNode* createAssignmentNode(ASTNode* leftSide,ASTNode* rhs)
{   assignment* assignmentNode;
    if(leftSide->getTypeofNode()==NODE_ID)
    {
      assignmentNode=assignment::id_assignExpr((Identifier*)leftSide,(Expression*)rhs);
    }
    if(leftSide->getTypeofNode()==NODE_PROPACCESS)
    {
        assignmentNode=assignment::prop_assignExpr((PropAccess*)leftSide,(Expression*)rhs);
    }
    return assignmentNode;
}

static ASTNode* createNodeForProcCallStmt(ASTNode* procCall)
{
    statement* procCallStmt;
    procCallStmt=proc_callStmt::nodeForCallStmt((Expression*)procCall);
    bool value=procCallStmt->getTypeofNode()==NODE_PROCCALLSTMT;
    cout<<"TYPE OF NODE OF STATEMENT"<<value;
    return procCallStmt;
}

static ASTNode* createNodeForProcCall(ASTNode* proc_callId,list<argument*> argList)
{    
    proc_callExpr* proc_callExprNode;
    if(proc_callId->getTypeofNode()==NODE_ID)
    {
      proc_callExprNode=proc_callExpr::nodeForProc_Call(NULL,NULL,(Identifier*)proc_callId,argList);
     
      
    }
    if(proc_callId->getTypeofNode()==NODE_PROPACCESS)
    {
      PropAccess* propAccessId=(PropAccess*)proc_callId;
      
      proc_callExprNode=proc_callExpr::nodeForProc_Call(propAccessId->getIdentifier1(),NULL,propAccessId->getIdentifier2(),argList);
    }
    
    return proc_callExprNode;
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
    cout<<"CHECK PASSED "<<check<<"\n";
    return exprBVal;
}
static ASTNode* createNodeForINF(bool infinityFlag)
{
    Expression* exprINFVal=Expression::nodeForInfinity(infinityFlag);
    return exprINFVal;
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
static ASTNode* createNodeForFixedPointStmt(ASTNode* convergeExpr,ASTNode* body)
{
    statement* fixedPointStmtNode;
    fixedPointStmtNode=fixedPointStmt::createforfixedPointStmt((Expression*)convergeExpr,(blockStatement*)body);
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
    ifStmtNode=ifStmt::create_ifStmt((Expression*)iterCondition,(blockStatement*)thenBody,(blockStatement*)elseBody);
    return ifStmtNode;
}
static ASTNode* createNodeForForAllStmt(ASTNode* iterator,ASTNode* sourceGraph,ASTNode* extractElemFunc,ASTNode* body,ASTNode* filterExpr,bool isforall)
{
    statement* forallStmtNode;
    Identifier* id=(Identifier*)iterator;
    Identifier* id1=(Identifier*)sourceGraph;

    forallStmtNode=forallStmt::createforallStmt(id,id1,(proc_callExpr*)extractElemFunc,(statement*)body,(Expression*)filterExpr,isforall);
    return forallStmtNode;
}
static ASTNode* createNodeForForStmt(ASTNode* iterator,ASTNode* source,ASTNode* body,bool isforall)
{
    statement* forallStmtNode;
    Identifier* id=(Identifier*)iterator;
    if(source->getTypeofNode()==NODE_ID)
    forallStmtNode=forallStmt::id_createforForStmt(id,(Identifier*)source,(statement*)body,isforall);
    if(source->getTypeofNode()==NODE_PROPACCESS)
    forallStmtNode=forallStmt::propId_createforForStmt(id,(PropAccess*)source,(statement*)body,isforall);
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
static ASTNode* createNodeForReductionStmtList(list<ASTNode*> leftList,ASTNode* reductionCallNode,ASTNode* exprVal)
{
    reductionCallStmt* reductionStmtNode;
    reductionStmtNode=reductionCallStmt::leftList_reducCallStmt(leftList,(reductionCall*)reductionStmtNode,(Expression*)exprVal);
    return reductionStmtNode;
}



static ASTNode* createPropIdNode(ASTNode* id1,ASTNode* id2)
{
    PropAccess* propIdNode;
    propIdNode=PropAccess::createPropAccessNode((Identifier*)id1,(Identifier*)id2);
    return propIdNode;
}
static ASTNode* createIterateInReverseBFSNode( ASTNode* booleanExpr,ASTNode* filterExpr,ASTNode* body)
{
    iterateReverseBFS* iterateReverseBFSNode;
    iterateReverseBFSNode=iterateReverseBFS::nodeForRevBFS((Expression*)booleanExpr,(Expression*)filterExpr,(statement*)body);
    return iterateReverseBFSNode;
}
static ASTNode* createIterateInBFSNode(ASTNode* iterator,ASTNode* rootNode,ASTNode* filterExpr,ASTNode* body,ASTNode* revBFS)
{
    iterateBFS* iterateBFSNode;
    Identifier* id1=(Identifier*)iterator;
    Identifier* id2=(Identifier*)rootNode;
    cout<<"INSIDE BFS1"<<id2->getIdentifier()<<"\n";
    iterateBFSNode=iterateBFS::nodeForIterateBFS(id1,id2,(Expression*)filterExpr,(statement*)body,(iterateReverseBFS*)revBFS);
    return iterateBFSNode;
}
};
