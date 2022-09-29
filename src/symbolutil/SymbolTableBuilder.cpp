#include "SymbolTableBuilder.h"
#include "../ast/ASTHelper.cpp"

void performUpdatesAssociation(PropAccess* expr, ASTNode* preprocessEnv)
{
    Identifier* updatesId = NULL;
    if(preprocessEnv->getTypeofNode() == NODE_ONADDBLOCK)
        {
            onAddBlock* onAddStmt = (onAddBlock*)preprocessEnv;
            updatesId = onAddStmt->getUpdateId();
                      
        }
     else
        {
            onDeleteBlock* onDeleteStmt = (onDeleteBlock*)preprocessEnv;
            updatesId = onDeleteStmt->getUpdateId();

        }
    Identifier* id1 = expr->getIdentifier1();
    string updatesIdName(updatesId->getIdentifier());
    if(updatesIdName == id1->getIdentifier())
       expr->getIdentifier1()->setUpdateAssociation(updatesId);
}

/* if the filter is on a nested for all statement,
   the filter is propagated to the parent parallel-for loop */
void setFilterAssocForForAll(std::vector<ASTNode*> parallelConstruct, Expression* filterExpr)
{
  forallStmt* forStmt = NULL;

  for(size_t i=0 ; i < parallelConstruct.size() ; i++)
     {
         if(parallelConstruct[i]->getTypeofNode() == NODE_FORALLSTMT)
           {
               forStmt = (forallStmt*)parallelConstruct[i];
               break;
           }
     }

   forStmt->setFilterExprAssoc();
   forStmt->setAssocExpr(filterExpr);
}

bool search_and_connect_toId(SymbolTable* sTab,Identifier* id)
 {   // cout<<"ID VALUE IN SEARCH"<<id->getIdentifier()<<"\n";
     assert(id!=NULL);
     assert(id->getIdentifier()!=NULL);
     TableEntry* tableEntry=sTab->findEntryInST(id);
     if(tableEntry==NULL)
     {  return false;
         //to be added.
     }
     // cout<<"FINALLY FOUND IT"<<"\n";
     if(id->getSymbolInfo()!=NULL)
      {
      assert(id->getSymbolInfo()==tableEntry);
      }
     else
        id->setSymbolInfo(tableEntry);

     return true;

 }

 void addPropToSymbolTE(SymbolTable* sTab,Identifier* id,std::list<argument*> argList)
 {
     assert(id!=NULL);
     assert(id->getIdentifier()!=NULL);
     TableEntry* tableEntry=sTab->findEntryInST(id);
     Type* type = tableEntry->getType();
     for(argument* arg:argList)
     {
         assert(arg->isAssignExpr());
         if(arg->isAssignExpr())
         {
             assignment* assign=(assignment*)arg->getAssignExpr();
             type->addToPropList(assign->getId());
             Identifier* TE_id = assign->getId()->getSymbolInfo()->getId();
             TE_id->set_assignedExpr(assign->getExpr()); // added to extract the initialization value.
         }
     }


 }

  declaration*  createPropertyDeclaration(Identifier* &id)
 {

     Type* typeNode=Type::createForPrimitive(TYPE_INT,1);
     Type* propType=Type::createForPropertyType(TYPE_PROPNODE,4,typeNode);
     declaration* declNode=declaration::normal_Declaration(propType,id);
     return declNode;
 }

 argument* createAssignedArg(Identifier* id)
 {
    Expression* exprForId=Expression::nodeForIntegerConstant(-1);
    assignment* assignmentNode=assignment::id_assignExpr(id,exprForId);
    argument* arg=new argument();
    arg->setAssign(assignmentNode);
    arg->setAssignFlag();
    return arg;

 }

 /*void SymbolTableBuilder::setLockDeclInFunc(ASTNode* node)
  {

     ASTNode* parent = node->getParent();
     if(parent!=NULL)
       { 
         if(parent->getTypeofNode()!=NODE_FUNC)
            setLockDeclInFunc(parent);
         else
           {   
               
               Function* func = (Function*) node;
               func->setInitialLockDecl();
           }     
       }

  }*/

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

 bool checkInsideBFSIter(std::vector<ASTNode*> parallelRegions)
  {
      int size = parallelRegions.size() - 1;
      while(size >= 0)
        { 
           ASTNode* parallelRegion = parallelRegions[size];
           if(parallelRegion->getTypeofNode() == NODE_ITRBFS)
              return true;

           size = size - 1;   

        }

        return false;


  }



 void SymbolTableBuilder::buildForProc(Function* func)
 {  
     currentFunc = func;
     init_curr_SymbolTable(func);
     list<formalParam*> paramList=func->getParamList();
     list<formalParam*>::iterator itr;
     for(itr=paramList.begin();itr!=paramList.end();itr++)
     {
         formalParam* fp=(*itr);
         Type* type=fp->getType();
         Identifier* id=fp->getIdentifier(); 
         SymbolTable* symbTab=type->isPropType()?currPropSymbT:currVarSymbT;
         bool creationFine=create_Symbol(symbTab,id,type);
         id->getSymbolInfo()->setArgument(true);

         

    }

    buildForStatements(func->getBlockStatement());
    delete_curr_SymbolTable();



 }

 void SymbolTableBuilder::buildForStatements(statement* stmt)
 {
    bool searchSuccess=false;
    string backend(backendTarget);

    switch(stmt->getTypeofNode())
    {
       case NODE_DECL:
       {
           declaration* declStmt=(declaration*)stmt;
           Type* type=declStmt->getType();
           SymbolTable* symbTab=type->isPropType()?currPropSymbT:currVarSymbT;
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
                  //~ std::cout<< "NORMAL ASST:" <<id->getIdentifier()  <<'\n';
             }
             else if(assign->lhs_isProp())
             {
                 PropAccess* propId=assign->getPropId();
                 searchSuccess=findSymbolPropId(propId);
                  //~ std::cout<< "PROP ASST" << propId->getIdentifier1()->getIdentifier() << '\n';
             }

             if(backend.compare("cuda")== 0  && assign->lhs_isProp()){  // This flags device assingments OUTSIDE for
              //~ std::cout<< "varName1: " << assign->getPropId()->getIdentifier1()->getIdentifier() << '\n';
              //~ std::cout<< "varName2: " << assign->getPropId()->getIdentifier2()->getIdentifier() << '\n';
              std::cout<<"ANAME1:"<< assign->getParent()->getTypeofNode() << '\n';
              std::cout<< "ANAME2:"<< assign->getParent()->getParent()->getTypeofNode() << '\n';
              //~ std::cout<< "ANAME3:"<< assign->getParent()->getParent()->getParent()->getTypeofNode() << '\n';
              if(assign->getParent()->getParent()->getTypeofNode()==NODE_FUNC || (assign->getParent()->getParent()->getTypeofNode()!=NODE_FUNC && assign->getParent()->getParent()->getParent()->getTypeofNode()==NODE_FUNC)){
                std::cout<< "\t\tSetting device bool" << '\n';
                assign->flagAsDeviceVariable();
              }
             }

            Expression* exprAssigned = assign->getExpr();
            checkForExpressions(exprAssigned);
             
             if(assign->lhs_isIdentifier())
             {
                  Identifier* id=assign->getId();
                  if(id->getSymbolInfo()->getType()->isPropNodeType())
                    {
                        if(exprAssigned->isIdentifierExpr())
                          {
                              Identifier* rhsPropId = exprAssigned->getId();
                              if(rhsPropId->getSymbolInfo()->getType()->isPropNodeType())
                                 assign->setPropCopy() ; 
                          }
                    }
                   
              }

             break;
       }
       case NODE_FIXEDPTSTMT:
       {
           fixedPointStmt* fpStmt=(fixedPointStmt*)stmt;
           Identifier* fixedPointId=fpStmt->getFixedPointId();
           searchSuccess=findSymbolId(fixedPointId);
           checkForExpressions(fpStmt->getDependentProp());
           /* This else portion needs to be generalized to handle expressions of all kinds.
              Currently handling dependent expressions that is based on a single node property and is Unary*/
           
           if(fpStmt->getDependentProp()->isUnary()||fpStmt->getDependentProp()->isIdentifierExpr())
           {  
               
               Identifier* depId ;
               if(fpStmt->getDependentProp()->isUnary())
               {
                  depId = fpStmt->getDependentProp()->getUnaryExpr()->getId();
               }
               else
               {  
                   depId = fpStmt->getDependentProp()->getId();
               }
               if(depId->getSymbolInfo()!=NULL)
                 {  
                     
                     Identifier* tableId = depId->getSymbolInfo()->getId();
                     tableId->set_redecl(); //explained in the ASTNodeTypes
                     tableId->set_fpassociation(); //explained in the ASTNodeTypes
                     tableId->set_fpId(fixedPointId->getIdentifier()); //explained in the ASTNodeTypes
                     tableId->set_dependentExpr(fpStmt->getDependentProp());
                 }
           }
           buildForStatements(fpStmt->getBody());

         break;

       }

       case NODE_FORALLSTMT:
       {   
           
           forallStmt* forAll=(forallStmt*)stmt;
           Identifier* source1=forAll->isSourceProcCall()?forAll->getSourceGraph():NULL;
            if(forAll->getSource()!=NULL)
               source1 = forAll->getSource();
            PropAccess* propId=forAll->isSourceField()?forAll->getPropSource():NULL;
            searchSuccess=checkHeaderSymbols(source1,propId,forAll);
            string backend(backendTarget);
            if(forAll->hasFilterExpr())
            {
                checkForExpressions(forAll->getfilterExpr());
            }
            /* Required for omp backend code generation,
               the forall is checked for its existence inside
               another forall which is to be generated with
               omp parallel pragma, and then disable the parallel loop*/

            if( (backend.compare("omp")==0) || (backend.compare("cuda")==0) || (backend.compare("openACC")==0) )
             {  
                 if(parallelConstruct.size()>0)
                  {  
                      forAll->disableForall();
                      if(forAll->hasFilterExpr())
                         setFilterAssocForForAll(parallelConstruct, forAll->getfilterExpr());
                  }

                  if(forAll->isForall())
                    {
                        parallelConstruct.push_back(forAll);
                       
                    }
             }

             if(backend.compare("cuda")== 0 ){ // This flags device assingments INSIDE for
              std::cout<< "FORALL par   NAME1:"<< forAll->getParent()->getTypeofNode() << '\n';
              if(forAll->getParent()->getParent())
                std::cout<< "FORALL ParPar NAME2:"<< forAll->getParent()->getParent()->getTypeofNode() << '\n';
              if(forAll->getParent()->getParent()->getParent())
                std::cout<< "FORALL ParParPar NAME3:"<< forAll->getParent()->getParent()->getParent()->getTypeofNode() << '\n';

              if(forAll->getParent()->getParent()->getTypeofNode()==NODE_FUNC){      // This flags device assingments OUTSIDE for e.g BC
                std::cout<< "\t\tTO Set DEVICE BOOL" << '\n';
                forAll->addDeviceAssignment();
              }
              if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRRBFS ) {
                std::cout<< "\t\tTP SET ACC BC BOOL" << '\n';
                //~ forAll->addDeviceAssignment();
              }
            }


            if(checkInsideBFSIter(parallelConstruct)) // this assumes all the assignment stmts inside for of ITER BFS are atomic. Possibile bug if more statements!
               {
                 /* the assignment statements(arithmetic & logical) within the block of a for statement that
                    is itself within a IterateInBFS abstraction are signaled to become atomic while code
                    generation. */
                  proc_callExpr* extractElem = forAll->getExtractElementFunc();
                  if(extractElem!=NULL)
                   {
                     string methodString(extractElem->getMethodId()->getIdentifier());
                     list<argument*> argList = extractElem->getArgList();
                     if(methodString == nbrCall)
                       {  

                        forAll->addAtomicSignalToStatements();
                       }
                   }
               }

              //~ init_curr_SymbolTable(forAll);
    
              //~ Type* type = (Type*) Util::createNodeEdgeTypeNode(TYPE_NODE);
              //~ bool creatsFine=create_Symbol(currVarSymbT,forAll->getIterator(),type);

              buildForStatements(forAll->getBody());  
              
           //----------------------MERGE CONFLICT OCCURRED HERE (WORKING BRANCH <---- OPENACC)----------------------------------
               //~ if((backend.compare("omp")==0 || backend.compare("openACC")==0 ) && forAll->isForall())
                    //~ {  
                        //~ parallelConstruct.pop_back();
                    //~ } 

              //~ delete_curr_SymbolTable();


               if((backend.compare("omp")==0 || backend.compare("cuda")==0 || backend.compare("openACC")==0 ) &&forAll->isForall())
                    {
                        parallelConstruct.pop_back();
                    }
              break;
          //-----------------------------------------------------------------------------------------------------------------
       }
       case NODE_BLOCKSTMT:
       {  blockStatement* blockStmt=(blockStatement*)stmt;
          init_curr_SymbolTable(blockStmt);
          list<statement*> stmtList=blockStmt->returnStatements();
          list<statement*>::iterator itr;
          int count=0;
          int procPos=-1;
          int bfsPos=-1;
          list<statement*>::iterator itrIBFS;
           list<statement*>::iterator itrProc;
          bool flag=false;
          bool callFlag=false;
          string backend(backendTarget);
          for(itr=stmtList.begin();itr!=stmtList.end();itr++)
          { 
          
             if(!callFlag&&(*itr)->getTypeofNode()==NODE_PROCCALLSTMT)
             {
                 proc_callStmt* proc=(proc_callStmt*)(*itr);
              
              //---------------------MERGE CONFLICT OCCURED HERE (WORKING BRANCH<---OPENACC)----------------------------
                 char* methodId=proc->getProcCallExpr()->getMethodId()->getIdentifier();
                 string IDCoded(attachNodeCall);
                 int x=IDCoded.compare(IDCoded);
                 if(x==0)
                 {
                  itrProc=itr;
                  callFlag=true;
                  procPos=count;
                 }
             }
             if((*itr)->getTypeofNode()==NODE_ITRBFS)
             {

                itrIBFS=itr;

                flag=true;
                bfsPos=count;
             }
             buildForStatements((*itr));
             count++;
          }

          if(flag&&(backend.compare("omp")==0 || backend.compare("openACC")==0 ))
          { 
            iterateBFS* itrbFS=(iterateBFS*)(*itrIBFS);  
            Identifier* id=Identifier::createIdNode("bfsDist");
            declaration* decl=createPropertyDeclaration(id);
            buildForStatements(decl);
            blockStmt->insertToBlock(decl,procPos);
            argument* arg=createAssignedArg(id);
            proc_callStmt* proc=(proc_callStmt*)*itrProc;
            proc_callExpr* pExpr=proc->getProcCallExpr();
            pExpr->addToArgList(arg);
            PropAccess* propIdNode=PropAccess::createPropAccessNode(itrbFS->getRootNode(),id);
            Expression* expr=Expression::nodeForIntegerConstant(-1);
            assignment* assignmentNode=assignment::prop_assignExpr(propIdNode,expr);
            blockStmt->insertToBlock(assignmentNode,bfsPos+1);

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
           //cout<<"Procedure Call statment"<<endl;
           proc_callStmt* proc_call=(proc_callStmt*)stmt;
           proc_callExpr* pExpr=proc_call->getProcCallExpr();
           checkForExpressions(pExpr);
         break;

       }
       case NODE_REDUCTIONCALLSTMT:
       {   
           reductionCallStmt* reducStmt=(reductionCallStmt*)stmt;
           if(reducStmt->is_reducCall())
           {
           list<ASTNode*> leftList=reducStmt->getLeftList();
           list<ASTNode*> exprList=reducStmt->getRightList();
           list<ASTNode*>::iterator itr;
           list<ASTNode*>::iterator itr1;
           for(itr=leftList.begin();itr!=leftList.end();itr++)
           {
               if((*itr)->getTypeofNode()==NODE_ID)
                   findSymbolId((Identifier*)*itr);
                if((*itr)->getTypeofNode()==NODE_PROPACCESS)
                   {
                    findSymbolPropId((PropAccess*)*itr);  
                   
                   }
           }
            
            if(leftList.size() > 2)
               {
                   string backend(backendTarget);
                   if(backend.compare("omp") == 0 ||  backend.compare("openACC") == 0 )
                     {
                        currentFunc->setInitialLockDecl();
                     }   
               }

           for(itr1=exprList.begin();itr1!=exprList.end();itr1++)
           {
               checkForExpressions((Expression*)*itr1);
           } 
           
           reductionCall* reduceExpr = reducStmt->getReducCall();

           checkReductionExpr(reduceExpr);
           }
           else
           {
               if(reducStmt->isLeftIdentifier())
                 {
                     findSymbolId(reducStmt->getLeftId());
                     ASTNode* nearest_Parallel=parallelConstruct.back();
                     if(nearest_Parallel->getTypeofNode()==NODE_FORALLSTMT)
                       {  
                           forallStmt* forAll=(forallStmt*)nearest_Parallel;
                           forAll->push_reduction(reducStmt->reduction_op(),reducStmt->getLeftId());
                       }
                 }
               else
               {
                   findSymbolPropId(reducStmt->getPropAccess());
               }
                 checkForExpressions(reducStmt->getRightSide());
           }
           break;
       }
       case NODE_ITRBFS:
       {
          iterateBFS* iBFS=(iterateBFS*)stmt;
          string backend(backendTarget);
            if(backend.compare("omp")==0 ||  backend.compare("openACC") == 0 )
             { 
               parallelConstruct.push_back(iBFS);
               
             }     
          
          buildForStatements(iBFS->getBody());
          iterateReverseBFS* iRevBFS = iBFS->getRBFS();
          iRevBFS->addAccumulateAssignment();

          if(backend.compare("omp")==0  ||  backend.compare("openACC") == 0 )
             { 
              parallelConstruct.pop_back();
               
             } 
        
           break;
       }
       case NODE_DOWHILESTMT:
       {   
           dowhileStmt* doStmt=(dowhileStmt*)stmt;
           checkForExpressions(doStmt->getCondition());
           buildForStatements(doStmt->getBody());
           break;
       }


       case NODE_RETURN:
         {
            returnStmt* retStmt = (returnStmt*)stmt;
            checkForExpressions(retStmt->getReturnExpression());
            currentFunc->flagReturn();
            break;  
         }
       case NODE_BATCHBLOCKSTMT:
         {
            batchBlock* batchBlckStmt = (batchBlock*)stmt;
            checkForExpressions(batchBlckStmt->getBatchSizeExpr());
            batchBlockEnv = batchBlckStmt;
            buildForStatements(batchBlckStmt->getStatements());
            batchBlockEnv = NULL;
            break;

         }
        case NODE_ONADDBLOCK:
        {
          onAddBlock* onAddStmt = (onAddBlock*)stmt;
          findSymbolId(onAddStmt->getUpdateId());
          batchBlock* batchBlockBound = (batchBlock*)batchBlockEnv;
          /*if(batchBlockBound->getUpdateId()==NULL)
             batchBlockBound->setUpdateId(onAddStmt->getUpdateId());*/
          preprocessEnv = onAddStmt;   
          buildForStatements(onAddStmt->getStatements());
          preprocessEnv = NULL;  
          break;

        }
       case NODE_ONDELETEBLOCK:
       {
         onDeleteBlock* onDeleteStmt = (onDeleteBlock*)stmt;
         findSymbolId(onDeleteStmt->getUpdateId());
         batchBlock* batchBlockBound = (batchBlock*)batchBlockEnv;
         /* if(batchBlockBound->getUpdateId()==NULL)
             batchBlockBound->setUpdateId(onDeleteStmt->getUpdateId());*/
         preprocessEnv = onDeleteStmt;    
         buildForStatements(onDeleteStmt->getStatements());
         preprocessEnv = NULL;
         break;

       } 
       
      case NODE_UNARYSTMT:
       {
           unary_stmt* unaryStmt = (unary_stmt*) stmt;
           checkForExpressions(unaryStmt->getUnaryExpr());
          break;
       }
     default: // added to supress warnings
          std::cout<< "DEFAULT NODE-ok?" << '\n';

      
   }


 }

 void SymbolTableBuilder::checkReductionExpr(reductionCall* reduce)
 {   
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

void SymbolTableBuilder::checkForArguments(list<argument*> argList)
{
    /* primarly required for situations where we require to create a alias for the
       variable for double buffering purposes*/
     for(argument* arg:argList)
     {  
         
         if(arg->isAssignExpr())
         {
             assignment* assign=(assignment*)arg->getAssignExpr();
             if(assign->lhs_isIdentifier())
               findSymbolId(assign->getId());
             else
               findSymbolPropId(assign->getPropId());
            
            checkForExpressions(assign->getExpr());
         }
         if(arg->isExpr())
         {
            checkForExpressions(arg->getExpr());
         }
          
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
             Identifier* methodId=pExpr->getMethodId();
             string s(methodId->getIdentifier());
             if(id!=NULL)
                ifFine=findSymbolId(id);
              checkForArguments(pExpr->getArgList());  

              if(s.compare(attachNodeCall) == 0) 
                {
                 addPropToSymbolTE(currVarSymbT,id,pExpr->getArgList());
                }
              if(s.compare(currentBatch) == 0)  
                 {
                   
                   batchBlock* currentBlock = (batchBlock*)batchBlockEnv ;
                   Identifier* updatesId = currentBlock->getUpdateId();
                   string updatesString(updatesId->getIdentifier());
                   string idString(id->getIdentifier());
                   assert(idString.compare(updatesString) == 0);
                 }
             break;   
         }  
         case EXPR_UNARY:
         {   
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
         {  
             ifFine=findSymbolId(expr->getId());
             break;
         }
         case EXPR_PROPID:
         {  
             // cout<<expr->getPropId()->getIdentifier1()->getIdentifier()<<"\n";
             ifFine=findSymbolPropId(expr->getPropId());
              if(preprocessEnv!=NULL && expr->getPropId()->getIdentifier1()->getSymbolInfo()==NULL)
               {  

                   performUpdatesAssociation((PropAccess*)expr->getPropId(),preprocessEnv);
                  /* Identifier* updatesId = NULL;
                   if(preprocessEnv->getTypeofNode() == NODE_ONADDBLOCK)
                     {
                         onAddBlock* onAddStmt = (onAddBlock*)preprocessEnv;
                         updatesId = onAddStmt->getUpdateId();
                     }
                     else
                     {

                     }*/
               }

             break;
         }


     }
 }
 bool SymbolTableBuilder::findSymbolPropId(PropAccess* propId)
 {  
     bool isFine = true;
     Identifier* id1=propId->getIdentifier1();
     Identifier* id2=propId->getIdentifier2();
     search_and_connect_toId(currVarSymbT,id1);
     search_and_connect_toId(currPropSymbT,id2); //need to change..ideally this should search in prop.
     
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

    if(src!=NULL)
    {
        if(!findSymbolId(src))
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
{     
    list<Function*>::iterator itr;
    for(itr=funcList.begin();itr!=funcList.end();itr++)
       buildForProc((*itr));
}



