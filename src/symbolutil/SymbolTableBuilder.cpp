#include "SymbolTableBuilder.h"

bool search_and_connect_toId(SymbolTable* sTab,Identifier* id)
 {   cout<<"ID VALUE IN SEARCH"<<id->getIdentifier()<<"\n";
     assert(id!=NULL);
     assert(id->getIdentifier()!=NULL);
     TableEntry* tableEntry=sTab->findEntryInST(id);
     if(tableEntry==NULL)
     {  return false;
         //to be added.
     }
     cout<<"FINALLY FOUND IT"<<"\n";
     if(id->getSymbolInfo()!=NULL)
      assert(id->getSymbolInfo()==tableEntry);
     else
        id->setSymbolInfo(tableEntry);

     return true;  

 }

 bool create_Symbol(SymbolTable* sTab,Identifier* id,Type* type)
 {
     bool checkFine=sTab->check_conflicts_and_add(sTab, id,type);
     if(!checkFine)
     {
         //action to be added.
     }
     search_and_connect_toId(sTab,id);

     return checkFine;
 }

 void SymbolTableBuilder::buildForProc(Function* func)
 {  cout<<"INSIDE PROC OF SYMBOLBUILDER"<<"\n";
     init_curr_SymbolTable(func);
     list<formalParam*> paramList=func->getParamList();
     list<formalParam*>::iterator itr;
     for(itr=paramList.begin();itr!=paramList.end();itr++)
     {   
         formalParam* fp=(*itr);
         Type* type=fp->getType();
         Identifier* id=fp->getIdentifier(); 
         SymbolTable* symbTab=type->isPropNodeType()?currPropSymbT:currVarSymbT;
         bool creationFine=create_Symbol(symbTab,id,type);
         id->getSymbolInfo()->setArgument(true);

    }

    buildForStatements(func->getBlockStatement());


 }

 void SymbolTableBuilder::buildForStatements(statement* stmt)
 {   
     bool searchSuccess=false;
    switch(stmt->getTypeofNode())
    {
       case NODE_DECL:
       {
           declaration* declStmt=(declaration*)stmt;
           Type* type=declStmt->getType();
           SymbolTable* symbTab=type->isPropNodeType()?currPropSymbT:currVarSymbT;
           bool creatsFine=create_Symbol(symbTab,declStmt->getdeclId(),type);
           if(declStmt->isInitialized())
           {
               checkForExpressions(declStmt->getExpressionAssigned());
           }
             break;
       }
     
       case NODE_ASSIGN:
       { 
           assignment* assign=(assignment*)stmt;
           if(assign->lhs_isIdentifier())
             {
                 Identifier* id=assign->getId();
                  searchSuccess=findSymbolId(id);
             }
             else if(assign->lhs_isProp())
             {
                 PropAccess* propId=assign->getPropId();
                 searchSuccess=findSymbolPropId(propId);

             }
             break;
       }
       case NODE_FIXEDPTSTMT:
       { 
          fixedPointStmt* fpStmt=(fixedPointStmt*)stmt;
           Identifier* fixedPointId=fpStmt->getFixedPointId();
           searchSuccess=findSymbolId(fixedPointId);
           checkForExpressions(fpStmt->getDependentProp());
           buildForStatements(fpStmt->getBody());
         
         break;

       }

       case NODE_FORALLSTMT:
       {    cout<<"INSIDE BUILDFORSTATEMENT SYMBOLBUILDER"<<"\n"; 
           forallStmt* forAll=(forallStmt*)stmt;
            Identifier* source1=forAll->isSourceProcCall()?forAll->getSourceGraph():NULL;
            PropAccess* propId=forAll->isSourceField()?forAll->getPropSource():NULL;
            searchSuccess=checkHeaderSymbols(source1,propId,forAll);
            if(forAll->hasFilterExpr())
            {
                checkForExpressions(forAll->getfilterExpr());
            }
            buildForStatements(forAll->getBody());
          //  Identifier* source1=NULL;
            break;
       }
       case NODE_BLOCKSTMT:
       {  blockStatement* blockStmt=(blockStatement*)stmt;
          init_curr_SymbolTable(blockStmt);
          list<statement*> stmtList=blockStmt->returnStatements();
          list<statement*>::iterator itr;
          for(itr=stmtList.begin();itr!=stmtList.end();itr++)
          { 
           // cout<<"INSIDE BUILDFORSTATEMENT SYMBOLBUILDER"<<"\n";
            cout<<"SEE THE TYPE OF NODE"<<(*itr)->getTypeofNode()<<"\n";
             buildForStatements((*itr));
          }
          delete_curr_SymbolTable();
             break;

       }
       case NODE_WHILESTMT:
       {   whileStmt* whilestmt=(whileStmt*)stmt;
           checkForExpressions(whilestmt->getCondition());
           buildForStatements(whilestmt->getBody());
           break;
       }
       case NODE_IFSTMT:
       {
           ifStmt* ifstmt=(ifStmt*)stmt;
           checkForExpressions(ifstmt->getCondition());
           buildForStatements(ifstmt->getIfBody());
           if(ifstmt->getElseBody()!=NULL)
            buildForStatements(ifstmt->getElseBody());
          break;  
       }
       case NODE_PROCCALLSTMT:
       {
           proc_callStmt* proc_call=(proc_callStmt*)stmt;
           proc_callExpr* pExpr=proc_call->getProcCallExpr();
           checkForExpressions(pExpr);
         break;

       }
       case NODE_REDUCTIONCALLSTMT:
       {   cout<<"INSIDE SYMBOLBUILDER REDUCE"<<"\n";
           reductionCallStmt* reducStmt=(reductionCallStmt*)stmt;
           list<ASTNode*> leftList=reducStmt->getLeftList();
           list<ASTNode*> exprList=reducStmt->getRightList();
           list<ASTNode*>::iterator itr;
           list<ASTNode*>::iterator itr1;
           for(itr=leftList.begin();itr!=leftList.end();itr++)
           {
               if((*itr)->getTypeofNode()==NODE_ID)
                   findSymbolId((Identifier*)*itr);
                if((*itr)->getTypeofNode()==NODE_PROPACCESS)
                    findSymbolPropId((PropAccess*)*itr);   
           }
           for(itr1=exprList.begin();itr1!=exprList.end();itr1++)
           {
               checkForExpressions((Expression*)*itr1);
           } 
           
           reductionCall* reduceExpr=reducStmt->getReducCall();
           checkReductionExpr(reduceExpr);
           break;
       }
      
   }

   
 }

 void SymbolTableBuilder::checkReductionExpr(reductionCall* reduce)
 {  cout<<"Inside this for checking vars"<<"\n"; 
    list<argument*> argList=reduce->getargList();
    list<argument*>::iterator itr;
    for(itr=argList.begin();itr!=argList.end();itr++)
    {   
        argument* arg=(*itr);
        if(arg->isAssignExpr())
          buildForStatements(arg->getAssignExpr());
        else
          checkForExpressions(arg->getExpr()); 
    }


 }
 void SymbolTableBuilder::checkForExpressions(Expression* expr)
 {  bool ifFine=false;
     switch(expr->getExpressionFamily())
     {
         case EXPR_ARITHMETIC:
            {
                checkForExpressions(expr->getLeft());
                checkForExpressions(expr->getRight());
                break;
            }
         case EXPR_PROCCALL:
         {
             proc_callExpr* pExpr=(proc_callExpr*)expr;
             Identifier* id=pExpr->getId1();
             if(id!=NULL)
                ifFine=findSymbolId(id);
             break;   
         }  
         case EXPR_UNARY:
         {   cout<<"INSIDE UNARY FOR FIXEDPOINT"<<"\n";
             checkForExpressions(expr->getUnaryExpr());
             break;
         }
         case EXPR_LOGICAL:
         {
             checkForExpressions(expr->getLeft());
             checkForExpressions(expr->getRight());
             break;
         }
         case EXPR_RELATIONAL:
         {
             checkForExpressions(expr->getLeft());
             checkForExpressions(expr->getRight());
             break;
         }
         case EXPR_ID:
         {   cout<<"INSIDE EXPR_ID"<<"\n";
             ifFine=findSymbolId(expr->getId());
             break;
         }
         case EXPR_PROPID:
         {   cout<<"INSIDE EXPR_PROPID"<<"\n";
             cout<<expr->getPropId()->getIdentifier1()->getIdentifier()<<"\n";
             ifFine=findSymbolPropId(expr->getPropId());
             break;
         }
         

     }
 }
 bool SymbolTableBuilder::findSymbolPropId(PropAccess* propId)
 {   cout<<"INSIDE THIS FINSYMBOLPROPID"<<"\n";
     bool isFine = true;
     Identifier* id1=propId->getIdentifier1();
     Identifier* id2=propId->getIdentifier2();
     search_and_connect_toId(currVarSymbT,id1);
     search_and_connect_toId(currPropSymbT,id2);
     cout<<"OVER"<<"\n";
 }

 bool SymbolTableBuilder::findSymbolId(Identifier* id)
 {  
    bool success=search_and_connect_toId(currVarSymbT,id);
    if(!success)
    search_and_connect_toId(currPropSymbT,id); //need to optimize on whether to call this function or not.
    return true; //to be changed!
 }

 bool SymbolTableBuilder::checkHeaderSymbols(Identifier* src,PropAccess* propId,forallStmt* forall)
 {   if(propId!=NULL)
    {
     if(!findSymbolPropId(propId))
       return false;
    }

     return true;   //more checking need to be carried out.  
 }

 void SymbolTableBuilder::init_curr_SymbolTable(ASTNode* node)
 {
   node->getVarSymbT()->setParent(currVarSymbT);
   node->getPropSymbT()->setParent(currPropSymbT);
   variableSymbolTables.push_back(currVarSymbT);
   propSymbolTables.push_back(currPropSymbT);
   currVarSymbT=node->getVarSymbT();
   currPropSymbT=node->getPropSymbT();

 }

 void SymbolTableBuilder::delete_curr_SymbolTable()
 { 
     currVarSymbT=variableSymbolTables.back();
     currPropSymbT=propSymbolTables.back();

     variableSymbolTables.pop_back();
     propSymbolTables.pop_back();
 }

void SymbolTableBuilder::buildST(list<Function*> funcList)
{   cout<<"I WAS HERE! SEE ME!"<<"\n";
    //list<Function*> funcList=frontEndContext.getFuncList();
    list<Function*>::iterator itr;
    for(itr=funcList.begin();itr!=funcList.end();itr++)
       buildForProc((*itr));
} 
  

 

