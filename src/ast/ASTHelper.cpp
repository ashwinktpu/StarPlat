#include<MainContext.h>
#include<ASTNode.h>
#include<enum_def.h>

extern FrontEndContext FrontEndC;

/*TO be implemented. It will contain functions that will be called by action part of Parser
                                                      for building the nodes of AST*/
ASTNode* createFuncNode(Identifier* id,list<formalParam*> formalParamList)
{
  Function* functionNode=Function::createFunctionNode(id,formalParamList);
  return functionNode;

}

void createNewBlock()
{
    blockStatement* blockStatementNode=blockStatement::createnewBlock();
    FrontEndC.startBlock(blockStatementNode);
}

ASTNode* finishBlock()
{   
    blockStatement* blockStatementNode=FrontEndC.getCurrentBlock();
    FrontEndC.endBlock();
    retun blockStatementNode;
}
void addToBlock(ASTNode* statementNode)
{
    if(statement!=NULL)
    {
        statement* statementNode=(statement*)statementNode;
        blockStatement* currentBlock=FrontEndC.getCurrentBlock();
        currentBlock->addStmtToBlock(statementNode);
    }
}

paramList* addToPList(paramList* pList,formalParam* fParam)
{
     pList->PList.push_back(fParam);
     return paramList;
} 

argList* addToAList(argList* aList,argument arg)
{
    aList->AList.push_back(arg);
    return aList;
}

ASTNodeList* addToList(ASTNodeList* nodeList,ASTNode* node)
{
    nodeList->ASTNList.push_back(node);
    return nodeList;
}
ASTNode* createParamNode(Type* type,Identifier* id)
{
   formalParam* formalParamNode=formalParam::formalParam(type,id);
   return formalParamNode;

}

ASTNode* createNormalDeclNode(Type* type,Identifier* id)
{
    declaration* declNode=declaration::normal_Declaration(type,id);
    return declNode;
}

ASTNode* createAssignedDeclNode(Type* type,Identifier* id,Expression* exprAssigned)
{
    declaration* declNode=declaration::assign_Declaration(type,id,exprAssigned)
    return declaration;
}
ASTNode* createPrimitiveTypeNode(int typeId)
{
    Type* typeNode=Type::createForPrimitive(typeId,1);
    return typeNode;

}
ASTNode* createGraphTypeNode(int typeId,Identifier* targetGraph)
{
    Type* typeNode=Type::createForGraphType(typeId,2,targetGraph);
    return typeNode;

}
ASTNode* createCollectionTypeNode(int typeId,Identifier* targetGraph)
{
    Type* typeNode=Type::createForCollectionType(typeId,3,targetGraph);
    return typeNode;

}
ASTNode* createPropertyTypeNode(int typeId,Type* innerTargetType)
{
    Type* typeNode=Type::createForPropertyType(typeId,4,innerTargetType);
    return typeNode;

}
ASTNode* createNodeEdgeTypeNode(int typeId)
{   
    Type* typeNode=Type::createForNodeEdgeType(typeId,5);
    return typeNode;

}

ASTNode* createAssignmentNode(ASTNode* leftSide,Expression* rhs)
{   assignment* assignmentNode;
    if(leftSide->getTypeofNode=="ID")
    {
      assignmentNode=assignment::id_assignExpr((Identifier*)leftSide,rhs);
    }
    if(leftSide->getTypeofNode=="PROPACCESS")
    {
        assignmentNode=assignment::prop_assignExpr((PropAccess*)leftSide,rhs));
    }
    return assignmentNode;
}

ASTNode* createNodeForProcCallStmt(ASTNode* procCall)
{
    statement* procCallStmt;
    procCallStmt=proc_callStmt::nodeForCallStmt((Expression*)procCall);
    return procCallStmt;
}

ASTNode* createNodeForProcCall(ASTNode* proc_callId,list<argument> argList)
{    
    proc_callExpr* proc_callExprNode;
    if(proc_callId->getTypeofNode=="ID")
    {
      proc_callExprNode=proc_callExpr::nodeForProc_Call((Identifier*)proc_callId,$,argList);
    }
    if(proc_callId->getTypeofNode=="PROPACCESS")
    {
      PropAccess* propAccessId=(PropAccess*)proc_callId;
      
      proc_callExprNode=proc_callExpr::nodeForProc_Call(propAccessId->getIdentifier1(),propAccessId->getIdentifier2(),argList);
    }
    
    return proc_callExprNode;
}

ASTNode* createNodeForArithmeticExpr(Expression* expr1,Expression* expr2,int operatorType)
{
    Expression* arithmeticExprNode=Expression::nodeForArithmeticExpr(expr1,expr2,operatorType);
    return arithmeticExprNode;
}
ASTNode* createNodeForRelationalExpr(Expression* expr1,Expression* expr2,int operatorType)
{
    Expression* relationalExprNode=Expression::nodeForRelationalExpr(expr1,expr2,operatorType);
    return relationalExprNode;
}
ASTNode* createNodeForLogicalExpr(Expression* expr1,Expression* expr2,int operatorType)
{
    Expression* logicalExprNode=Expression::nodeForLogicalExpr(expr1,expr2,operatorType);
    return logicalExprNode;
}
ASTNode* createNodeForIval(long value)
{
    Expression* exprIVal=Expression::nodeForIntegerConstant(value);
    return exprIVal;
}
ASTNode* createNodeForFval(double value)
{
    Expression* exprIVal=Expression::nodeForDoubleConstant(value);
    return exprIVal;
}
ASTNode* createNodeForBval(bool value)
{
    Expression* exprIVal=Expression::nodeForBooleanConstant(value);
    return exprIVal;
}
ASTNode* createNodeForINF(bool infinityFlag)
{
    Expression* exprINFVal=Expression::nodeForInfinity(infinityFlag);
    return exprIVal;
}
ASTNode* createNodeForId(ASTNode* node)
{  Expression* exprForId;
   if(node->getTypeofNode=="ID")
      {
         exprForId=Expression::nodeForIdentifier((Identifier*)node);

      }
    if(node->getTypeofNode=="PROPACESS")
     {
         exprForId=Expression::nodeForPropAccess((PropAccess*)node);
     }
     return exprForId;

}
ASTNode* createNodeForFixedPointStmt(ASTNode* convergeExpr,ASTNode* body)
{
    statement* fixedPointStmtNode;
    fixedPointNode=fixedPointStmt::createforfixedPointStmt((Expression*)convergeExpr,(blockStatement*)body);
    return fixedPointNode;
}
ASTNode* createNodeForWhileStmt(ASTNode* iterCondition,ASTNode* body)
{
    statement* whileStmtNode;
    fixedPointNode=whileStmt::create_whileStmt((Expression*)iterCondition,(blockStatement*)body);
    return whileStmtNode;
}
ASTNode* createNodeForDoWhileStmt(ASTNode* iterCondition,ASTNode* body)
{
    statement* dowhileStmtNode;
    dowhileStmtNode=dowhileStmt::create_dowhileStmt((Expression*)iterCondition,(blockStatement*)body);
    return dowhileStmtNode;
}
ASTNode* createNodeForIfStmt(ASTNode* iterCondition,ASTNode* thenBody,ASTNode* elseBody)
{
    statement* ifStmtNode;
    ifStmtNode=ifStmt::create_ifStmt((Expression*)iterCondition,(blockStatement*)thenBody,(blockStatement*)elseBody);
    return ifStmtNode;
}
ASTNode* createNodeForForAllStmt(ASTNode* iterator,ASTNode* sourceGraph,ASTNode* extractElemFunc,ASTNode* body,ASTNode* filterExpr,bool isforall)
{
    statement* forallStmtNode;
    forallStmtNode=forallStmt::createforallStmt((Identifier*)iterator,(Identifier*)sourceGraph,(proc_callExpr*)extractElemFunc,(statement*)body,(Expression*)filterExpr,isforall);
    return forallStmtNode;
}
ASTNode* createNodeForForStmt(ASTNode* iterator,ASTNode* source,ASTNode* body,bool isforall)
{
    statement* forallStmtNode;
    if(source->getTypeofNode=="ID")
    forallStmtNode=forallStmt::id_createforForStmt((Identifier* )iterator,(Identifier*)sourceGraph,(statement*)body,isforall);
    if(source->getTypeofNode=="PROPACCESS")
    forallStmtNode=forallStmt::propId_createforForStmt((Identifier* )iterator,(PropAccess*)sourceGraph,(statement*)body,isforall);
    return forallStmtNode;
}
ASTNode* createNodeforReductionCall(int reductionOperationType,list<argument> argList)
{
    reductionCall* reductionCallNode=reductionCall::nodeForReductionCall(reductionOperationType,argList);
    return reductionCallNode;
}
ASTNode* createNodeForReductionStmt(ASTNode* leftSide,ASTNode* reductionCallNode)
{
    reductionCallStmt* reductionStmtNode;
    if(leftSide->getTypeofNode=="ID")
    {
        reductionStmtNode=reductionCallStmt::id_reducCallStmt((Identifier*)leftSide,(reductionCall*)reductionStmtNode);
    }
    if(leftSide->getTypeofNode=="PROPACCESS")
     {
         reductionStmtNode=reductionCallStmt::propId_reducCallStmt((PropAccess*)leftSide,(reductionCall*)reductionStmtNode);
     }
     return reductionStmtNode;
}
ASTNode* createNodeForReductionStmtList(list<ASTNode*> leftList,ASTNode* reductionCallNode,ASTNode* exprVal)
{
    reductionCallStmt* reductionStmtNode;
    reductionStmtNode=reductionCallStmt::leftList_reducCallStmt(leftList,(reductionCall*)reductionStmtNode,(Expression*)exprVal);
    return reductionStmtNode;
}
ASTNode* createIdentifierNode(char* idName)
{
    Identifier* idNode;
    idNode=Identifier::createIdNode(idName);
    return idNode;
}
ASTNode* createPropIdNode(ASTNode* id1,ASTNode* id2)
{
    PropAccess* propIdNode;
    propIdNode=PropAccess::createPropAccessNode((Identifier*)id1,(Identifier*)id2);
    return propIdNode;
}
ASTNode* createIterateInReverseBFSNode( ASTNode* booleanExpr,ASTNode* filterExpr,ASTNode* body)
{
    iterateReverseBFS* iterateReverseBFSNode;
    iterateReverseBFSNode=iterateReverseBFS::nodeForRevBFS((Expression*)booleanExpr,(Expression*)filterExpr,(statement*)body);
    return iterateReverseBFSNode;
}
ASTNode* createIterateInBFSNode(ASTNode* iterator,ASTNode* rootNode,ASTNode* filterExpr,ASTNode* body,ASTNode* revBFS)
{
    iterateBFS* iterateBFSNode;
    iterateBFSNode=iterateBFS::nodeForIterateBFS((Identifier*)iterator,(Identifier*)rootNode,(Expression*)filterExpr,(statement*)body,(iterateReverseBFS*)revBFS);
    return iterateBFSNode;
}