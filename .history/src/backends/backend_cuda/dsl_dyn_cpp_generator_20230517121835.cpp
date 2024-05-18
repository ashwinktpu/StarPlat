#include "dsl_dyn_cpp_generator.hpp"
#include "../../ast/ASTHelper.cpp"

namespace spdyncuda{

void dsl_dyn_cpp_generator::setPreprocessEnv()
{
  insidePreprocessEnv = true;
}

void dsl_dyn_cpp_generator::resetPreprocessEnv()
{
  insidePreprocessEnv = false;
}

bool dsl_dyn_cpp_generator::getPreprocessEnv()
{
  return insidePreprocessEnv;
}

void dsl_dyn_cpp_generator::setBatchEnvIds(Identifier* updatesIdSent)
{
   updatesId = updatesIdSent;

}

void dsl_dyn_cpp_generator::resetBatchEnv()
{
   updatesId = NULL;

}



Identifier* dsl_dyn_cpp_generator::getUpdatesId()
{
    return updatesId;
}



void dsl_dyn_cpp_generator::generateOnDeleteBlock(onDeleteBlock* onDeleteStmt, bool isMainFile)
{

  dslCodePad& targetFile = isMainFile ? main : header;
   setPreprocessEnv();
   char strBuffer[1024];
   Identifier* updatesId = onDeleteStmt->getUpdateId();
   proc_callExpr* updatesFunc = onDeleteStmt->getUpdateFunc();
   string methodId(updatesFunc->getMethodId()->getIdentifier());

   if(methodId == "currentBatch")
   {
     sprintf(strBuffer,"for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < %s.size() ; batchIndex++){",updatesId->getIdentifier());
     main.pushstr_newL(strBuffer);
     sprintf(strBuffer,"if(%s[batchIndex].type == 'd')",updatesId->getIdentifier());
     main.pushstr_newL(strBuffer);
     main.pushstr_newL("{");
     sprintf(strBuffer, "update %s = %s[batchIndex] ;",onDeleteStmt->getIteratorId()->getIdentifier(), getUpdatesId()->getIdentifier());
     main.pushstr_newL(strBuffer);
     generateBlock(onDeleteStmt->getStatements(),false);
     main.NewLine();
     main.pushstr_newL("}");
     main.pushstr_newL("}");
   }
   
   resetPreprocessEnv();


}

void dsl_dyn_cpp_generator::generateOnAddBlock(onAddBlock* onAddStmt,bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
    setPreprocessEnv();

   char strBuffer[1024];
   sprintf(strBuffer,"for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < %s.size() ; batchIndex++){",getUpdatesId()->getIdentifier());
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer, "if(%s[batchIndex].type == 'a')",getUpdatesId()->getIdentifier());
   main.pushstr_newL(strBuffer);
   main.pushstr_newL("{");
   sprintf(strBuffer, "update %s = %s[batchIndex] ;",onAddStmt->getIteratorId()->getIdentifier(), getUpdatesId()->getIdentifier());
   main.pushstr_newL(strBuffer);
   generateBlock(onAddStmt->getStatements(),false);
   main.NewLine();
   main.pushstr_newL("}");
   main.pushstr_newL("}");

   resetPreprocessEnv();


}
/*
void dsl_dyn_cpp_generator::generateFreeInCurrentBatch()
{
  //dslCodePad& targetFile = isMainFile ? main : header;
  char strBuffer[1024];
  for(int i=0 ; i < freeIdStore.back().size() ; i++)
     { 
       Identifier* id = freeIdStore.back()[i];
       if(id->getSymbolInfo()->getType()->gettypeId() == TYPE_PROPNODE ||
          id->getSymbolInfo()->getType()->gettypeId() == TYPE_PROPEDGE)
         {
          sprintf(strBuffer, "free(%s) ;", id->getIdentifier());
          main.pushstr_newL(strBuffer);
         }
       if(id->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES)
          {
            sprintf(strBuffer, "%s.clear();", id->getIdentifier());
            main.pushstr_newL(strBuffer);
          }   

     }

}
*/

void dsl_dyn_cpp_generator::generateBatchBlock(batchBlock* batchStmt, bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
  
  list<formalParam*> paramList = currentFunc->getParamList();
  list<formalParam*>::iterator itr;
  Identifier* updateId = batchStmt->getUpdateId();
  char strBuffer[1024];
  insideBatchBlock = true;

  /*for(itr=paramList.begin();itr!=paramList.end();itr++)
  { 
      Type* type=(*itr)->getType();
      if(type->gettypeId() == TYPE_UPDATES)  
        {
          updateId = (*itr)->getIdentifier();
          break;
        }
         
  }*/
  
  if(updateId != NULL)
    {
      setBatchEnvIds(updateId); //TODO: need to set batchsize's Id as well.
      main.pushString("int batchSize = ");
      generateExpr(batchStmt->getBatchSizeExpr(),false);
      main.pushstr_newL(";");
      sprintf(strBuffer,"int batchElements = 0;");
      main.pushstr_newL(strBuffer);
     /* sprintf(strBuffer,"int updateSize = %s.size() ;", updateId->getIdentifier());
      main.pushstr_newL(strBuffer);*/
      sprintf(strBuffer,"for( int updateIndex = 0 ; updateIndex < %s.size() ; updateIndex += batchSize){",updateId->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"if((updateIndex + batchSize) > %s.size())",updateId->getIdentifier());
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("{");
      sprintf(strBuffer,"batchElements = %s.size() - updateIndex ;",updateId->getIdentifier());
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("}");
      main.pushstr_newL("else");
      sprintf(strBuffer,"batchElements = batchSize ;");
      main.pushstr_newL(strBuffer);

      generateBlock(batchStmt->getStatements(),false);
      main.NewLine();
     // generateFreeInCurrentBatch();
     //freeIdStore.pop_back();
      main.pushstr_newL("}");

      resetBatchEnv();

    }

  

    insideBatchBlock = false;


}

void dsl_dyn_cpp_generator::generateStatement(statement* stmt, bool isMainFile )
{ 


   if (stmt->getTypeofNode() == NODE_BLOCKSTMT) {
    generateBlock((blockStatement*)stmt, false, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_DECL) {
    generateVariableDecl((declaration*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_ASSIGN) {
    // generateAssignmentStmt((assignment*)stmt);
    assignment* asst = (assignment*)stmt;
    if (asst->isDeviceAssignment())
      generateDeviceAssignmentStmt(asst, isMainFile);
    else  // atomic or normal asmt
      generateAtomicDeviceAssignmentStmt(asst, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_WHILESTMT) {
    generateWhileStmt((whileStmt*) stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_IFSTMT) {
    generateIfStmt((ifStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_DOWHILESTMT) {
    generateDoWhileStmt((dowhileStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
    std::cout << "STMT: For" << '\n';
    printf("isMainFile val %d\n", isMainFile);
    generateForAll((forallStmt*)stmt, isMainFile);
  }

  if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) {
    generateFixedPoint((fixedPointStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) {
    generateReductionStmt((reductionCallStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_ITRBFS) {
    generateBFSAbstraction((iterateBFS*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_PROCCALLSTMT) {
    generateProcCall((proc_callStmt*)stmt, isMainFile);
  }
  if (stmt->getTypeofNode() == NODE_UNARYSTMT) {
    unary_stmt* unaryStmt = (unary_stmt*)stmt;
    generateExpr(unaryStmt->getUnaryExpr(), isMainFile);
    main.pushstr_newL(";");
  }
  if (isOptimized && (stmt->getTypeofNode() == NODE_TRANSFERSTMT)) {
    varTransferStmt* transferStmt = (varTransferStmt*)stmt;
    generateTransferStmt(transferStmt);
  }
   
    if(stmt->getTypeofNode() == NODE_BATCHBLOCKSTMT)
       {

         generateBatchBlock((batchBlock*)stmt, isMainFile);

       }
    if(stmt->getTypeofNode() == NODE_ONADDBLOCK)
      {
        
        generateOnAddBlock((onAddBlock*)stmt, isMainFile);

      }
    if(stmt->getTypeofNode() == NODE_ONDELETEBLOCK)
      {
        
        generateOnDeleteBlock((onDeleteBlock*)stmt, isMainFile);
        
      }  
         

}


void dsl_dyn_cpp_generator::generate_exprProcCall(Expression* expr, bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
   printf("HERE PRESENT %s\n",proc->getMethodId()->getIdentifier());

  if(methodId=="get_edge")
  {
   
   // if(curFuncType == INCREMENTAL_FUNC || curFuncType == DECREMENTAL_FUNC)
        getEdgeTranslation(expr);//uncomment it
    /*else    
     main.pushString("edge");*/ //To be changed..need to check for a neighbour iteration 
                             // and then replace by the iterator.
  }
  else if(methodId=="count_outNbrs")
       {
         char strBuffer[1024];
         list<argument*> argList=proc->getArgList();
         assert(argList.size()==1);
         Identifier* nodeId=argList.front()->getExpr()->getId();
         Identifier* objectId=proc->getId1();
         sprintf(strBuffer,"(%s.%s[%s+1]-%s.%s[%s])",objectId->getIdentifier(),"indexofNodes",nodeId->getIdentifier(),objectId->getIdentifier(),"indexofNodes",nodeId->getIdentifier());
         main.pushString(strBuffer);
       }
  else if(methodId=="is_an_edge")
     {
        char strBuffer[1024];
         list<argument*> argList=proc->getArgList();
         assert(argList.size()==2);
         Identifier* srcId=argList.front()->getExpr()->getId();
         Identifier* destId=argList.back()->getExpr()->getId();
         Identifier* objectId=proc->getId1();
         sprintf(strBuffer,"%s.%s(%s, %s)",objectId->getIdentifier(),"check_if_nbr",srcId->getIdentifier(),destId->getIdentifier());
         main.pushString(strBuffer);
         
     }
   else if(methodId == "updateCSRAdd" || methodId == "updateCSRDel") 
     {
      
       char strBuffer[1024];
       list<argument*> argList = proc->getArgList();
       assert(argList.size() == 1);
       Identifier* updateId = argList.front()->getExpr()->getId();
       Identifier* objectId=proc->getId1();
       assert(updateId->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES);
       if(methodId == "updateCSRAdd")
          sprintf(strBuffer,"%s.%s(%s, %s, %s)",objectId->getIdentifier(),"updateCSRAdd",updateId->getIdentifier(),"updateIndex","batchElements");
       else
          sprintf(strBuffer,"%s.%s(%s, %s, %s)",objectId->getIdentifier(),"updateCSRDel",updateId->getIdentifier(),"updateIndex","batchElements");
       main.pushString(strBuffer);

     }
   else if(methodId == "currentBatch")
      {
          char strBuffer[1024];
          list<argument*> argList = proc->getArgList();
          assert(argList.size() == 1);
          printf("Get expression Family %d\n",argList.front()->getExpr()->getExpressionFamily());
          assert(argList.front()->getExpr()->getExpressionFamily() == EXPR_INTCONSTANT);
          int updateType = argList.front()->getExpr()->getIntegerConstant();

          if(updateType == 0)
             sprintf(strBuffer,"%s.%s(%s, %s, %s)",graphId[curFuncType][curFuncCount()][0]->getIdentifier(),"getDeletesFromBatch","updateIndex","batchSize",updatesId->getIdentifier());
          else
             sprintf(strBuffer,"%s.%s(%s, %s, %s)",graphId[curFuncType][curFuncCount()][0]->getIdentifier(),"getAddsFromBatch","updateIndex","batchSize",updatesId->getIdentifier());
          main.pushString(strBuffer);   

      }   
   else if(methodId == "Incremental" || methodId == "Decremental") 
        {
          /* for dynamic algos */

           char strBuffer[1024];
           if(methodId == "Incremental")
               sprintf(strBuffer,"%s_add",fileName);
           if(methodId == "Decremental")
              sprintf(strBuffer, "%s_del", fileName);
           main.pushString(strBuffer);
           generateArgList(proc->getArgList(), true);   //uncomment it later  

        } 
   else
    {  /* for userdefined function calls */
        char strBuffer[1024];
        list<argument*> argList=proc->getArgList();
        Identifier* objectId = proc->getId1();
        Expression* indexExpr = proc->getIndexExpr();
        if(objectId!=NULL)
          {
             Identifier* id2 = proc->getId2();
             if(id2 != NULL)
                 sprintf(strBuffer,"%s.%s.%s",objectId->getIdentifier(), id2->getIdentifier(), getProcName(proc));
             else
                 sprintf(strBuffer,"%s.%s",objectId->getIdentifier(), getProcName(proc).c_str());    
          }

          else if(indexExpr != NULL)
          {
            cout<<"ENTERED HERE FOR INDEXEXPR GENERATION DYNAMIC"<<"\n";
            Expression* mapExpr = indexExpr->getMapExpr();
            Identifier* mapExprId = mapExpr->getId();

            if(parallelConstruct.size() > 0 && mapExprId->getSymbolInfo()->getId()->isLocalMapReq())
                 generate_exprIndex(indexExpr, true, isMainFile);
            else
                 generate_exprIndex(indexExpr, false, isMainFile);

            sprintf(strBuffer,".%s", getProcName(proc).data());
          } 
        else
          {
            sprintf(strBuffer,"%s", getProcName(proc).data());
          } 

        main.pushString(strBuffer);
          if(methodId == "insert"){
         
         main.pushString("(");

         if(indexExpr != NULL){
         Expression* mapExpr = indexExpr->getMapExpr();
         Identifier* mapExprId = mapExpr->getId();

        if(parallelConstruct.size() > 0 && mapExprId->getSymbolInfo()->getId()->isLocalMapReq())
            generate_exprIndex(indexExpr, true, true);
        else
            generate_exprIndex(indexExpr, false, true);
         }
         else if(objectId != NULL){
          Identifier* id2 = proc->getId2();
          if(id2 != NULL)
             sprintf(strBuffer,"%s.%s",objectId->getIdentifier(), id2->getIdentifier());               
          else
             sprintf(strBuffer,"%s",objectId->getIdentifier()); 

          main.pushString(strBuffer);      
         }

         main.pushString(".end()");
         main.pushString(",");
         generateArgList(argList,false);
         main.pushString(".begin(),");
         generateArgList(argList, false);
         main.pushString(".end())");
                
      }  
      else  
        generateArgList(argList, true);   

    }

    }




void dsl_dyn_cpp_generator::generate_exprPropId(PropAccess* propId, bool isMainFile) //This needs to be made more generic.
{ 
  dslCodePad& targetFile = isMainFile ? main : header;
  char strBuffer[1024];
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  Expression* indexexpr = propId->getPropExpr();
  ASTNode* propParent=propId->getParent();
  //bool relatedToReduction = propParent!=NULL?propParent->getTypeofNode()==NODE_REDUCTIONCALLSTMT:false;
  if(id2 != NULL){
   if(id2->getSymbolInfo()!=NULL&&id2->getSymbolInfo()->getId()->get_fp_association())
    {
       bool relatedToUpdation = propParent!=NULL?((propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT|| propParent->getTypeofNode() == NODE_ASSIGN)):false;
       if(relatedToUpdation)
           {
             sprintf(strBuffer,"%s_nxt[%s]",id2->getIdentifier(),id1->getIdentifier());
          // printf("Inside this checked !\n");
           }
       else
          {
            sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());
          }    
      
    }
    else
    {
      if(id2->getSymbolInfo()==NULL)
        {
          if(id1->getUpdateAssociation()!=NULL)
            {
               sprintf(strBuffer, "%s[%s].%s" , getUpdatesId()->getIdentifier(),"batchIndex",id2->getIdentifier());
            }
          else
            sprintf(strBuffer, "%s.%s",id1->getIdentifier(),id2->getIdentifier());
        }
      else 
       {   
        if(curFuncType == INCREMENTAL_FUNC || curFuncType == DECREMENTAL_FUNC || curFuncType == DYNAMIC_FUNC)
           { 
              if(id2->getSymbolInfo()->getType()->gettypeId()==TYPE_PROPEDGE)
                 sprintf(strBuffer,"%s[%s.id]",id2->getIdentifier(),id1->getIdentifier());
              else
                 sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());  
           }
        else 
          sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());

       }
    }

}

else if(indexexpr != NULL){

     /* semantic checks needs to be added for maps containing property as values */
     generateExpr(indexexpr, isMainFile);

     if(id1->getSymbolInfo() != NULL) 
     {
       int typeId = id1->getSymbolInfo()->getType()->gettypeId();
       if(typeId == TYPE_INT || typeId == TYPE_NODE)

          sprintf(strBuffer,"[%s]",id1->getIdentifier());

       else if(typeId == TYPE_EDGE){

          sprintf(strBuffer,"[%s.id]",id1->getIdentifier());

       }    
     }
     else
        sprintf(strBuffer,"[%s]",id1->getIdentifier());   
 }
     
 main.pushString(strBuffer);
}


void dsl_dyn_cpp_generator::generateForAllSignature(forallStmt* forAll, bool isMainFile)
{
  
  dslCodePad& targetFile = isMainFile ? main : header;

  map<string, bool> dynamicLinkFunc = frontEndContext.getDynamicLinkFuncs();
  if(curFuncType == STATIC_FUNC)
    {
      dsl_cpp_generator::generateForAllSignature(forAll,false );
    } 
  else
   { 

    Identifier* funcId = currentFunc->getIdentifier();  
   string funcIdString = "";
   if(funcId != NULL)
      funcIdString.assign(funcId->getIdentifier());

   if(curFuncType == DECREMENTAL_FUNC || curFuncType == INCREMENTAL_FUNC 
      ||curFuncType == DYNAMIC_FUNC || (dynamicLinkFunc.find(funcIdString) != dynamicLinkFunc.end())) {

   char strBuffer[1024];
   Identifier* iterator=forAll->getIterator();
   if(forAll->isSourceProcCall())
    {
    Identifier* sourceGraph=forAll->getSourceGraph();
    proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
    Identifier* iteratorMethodId=extractElemFunc->getMethodId();
    if(allGraphIteration(iteratorMethodId->getIdentifier()))
    {
      char* graphId=sourceGraph->getIdentifier();
      char* methodId=iteratorMethodId->getIdentifier();
      string s(methodId);
      if(s.compare("nodes")==0)
      {
        cout<<"INSIDE NODES VALUE"<<"\n";
       sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_nodes",iterator->getIdentifier());
      }
      else
      sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_edges",iterator->getIdentifier());

      main.pushstr_newL(strBuffer);

     }
    else if(neighbourIteration(iteratorMethodId->getIdentifier()))
    { 
       
       char* graphId=sourceGraph->getIdentifier();
       char* methodId=iteratorMethodId->getIdentifier();
       string s(methodId);
       if(s.compare("neighbors")==0)
       {
       list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
       sprintf(strBuffer,"for (edge %s_edge : %s.getNeighbors(%s)) ",nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
       main.pushstr_newL(strBuffer);
       main.pushString("{");
       sprintf(strBuffer,"%s %s = %s_edge.destination ;","int",iterator->getIdentifier(),nodeNbr->getIdentifier()); //needs to move the addition of
       main.pushstr_newL(strBuffer);
       }
       if(s.compare("nodes_to")==0)
       {
        list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
       sprintf(strBuffer,"for (edge %s_inedge : %s.getInNeighbors(%s)) ",nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
       main.pushstr_newL(strBuffer);
       main.pushString("{");
       sprintf(strBuffer,"%s %s = %s_inedge.destination ;","int",iterator->getIdentifier(),nodeNbr->getIdentifier()); //needs to move the addition of
       main.pushstr_newL(strBuffer);
       }
        if(s.compare("inOutNbrs")==0)
       {
        list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
       sprintf(strBuffer,"for (edge %s_edges: %s.getInOutNbrs(%s)) ",nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
       main.pushstr_newL(strBuffer);
       main.pushString("{");
       sprintf(strBuffer,"%s %s = %s_edges.destination ;","int",iterator->getIdentifier(),nodeNbr->getIdentifier()); //needs to move the addition of
       main.pushstr_newL(strBuffer);
       }                                                                                                //statement to a different method.                                                                                                    //statement to a different method.

    }
  }
  else if(forAll->isSourceField())
  {
    /*PropAccess* sourceField=forAll->getPropSource();
    Identifier* dsCandidate = sourceField->getIdentifier1();
    Identifier* extractId=sourceField->getIdentifier2();
    
    if(dsCandidate->getSymbolInfo()->getType()->gettypeId()==TYPE_SETN)
    {

      main.pushstr_newL("std::set<int>::iterator itr;");
      sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",dsCandidate->getIdentifier(),dsCandidate->getIdentifier());
      main.pushstr_newL(strBuffer);

    }
        
    /*
    if(elementsIteration(extractId->getIdentifier()))
      {
        Identifier* collectionName=forAll->getPropSource()->getIdentifier1();
        int typeId=collectionName->getSymbolInfo()->getType()->gettypeId();
        if(typeId==TYPE_SETN)
        {
          main.pushstr_newL("std::set<int>::iterator itr;");
          sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",collectionName->getIdentifier(),collectionName->getIdentifier());
          main.pushstr_newL(strBuffer);
        }

      }*/

  }

   else if(forAll->isSourceExpr()){
  
  Expression* expr = forAll->getSourceExpr();
  Expression* mapExpr = expr->getMapExpr();
  Identifier* mapId = mapExpr->getId();

  cout<<"ENTERED................................................."<<"\n";

  if(mapId->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER){
     main.pushString("for(int i = 0 ; i < ");
     generateExpr(expr, isMainFile);
     main.pushstr_newL(".size() ; i++)");
     main.pushString("{ ");
     sprintf(strBuffer, "int %s = ", iterator->getIdentifier());
     main.pushString(strBuffer);
     generateExpr(expr, isMainFile);
     main.pushstr_newL("[i] ;");
  } 
  
 // cout<<"REACHED HERE AFTER COMPLETING "<<"\n";
  
  }
  else
  {
    Identifier* sourceId = forAll->getSource();
    if(sourceId !=NULL)
       {
          if(sourceId->getSymbolInfo()->getType()->gettypeId()==TYPE_SETN)
          {

           main.pushstr_newL("std::set<int>::iterator itr;");
           sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",sourceId->getIdentifier(),sourceId->getIdentifier());
           main.pushstr_newL(strBuffer);

          }  
          else if(sourceId->getSymbolInfo()->getType()->gettypeId()==TYPE_UPDATES)
              {
                 sprintf(strBuffer,"for(int i = 0 ; i < %s.size() ; i++)",sourceId->getIdentifier());
                 main.pushstr_newL(strBuffer);

              }

               else if(sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER){

                 sprintf(strBuffer,"for(int i = 0 ; i < %s.size() ; i++)",sourceId->getIdentifier());
                 main.pushstr_newL(strBuffer); 
                 main.pushString("{ ");
                 sprintf(strBuffer, "int %s = %s[i] ;", iterator->getIdentifier(), sourceId->getIdentifier());
                 main.pushstr_newL(strBuffer); 

          }  
       }
   }
}
else {

 dsl_cpp_generator::generateForAllSignature(forAll, true);

} 
 }


}

void dsl_dyn_cpp_generator::generateForAll(forallStmt* forAll, bool isMainFile )
{ 

   dslCodePad& targetFile = isMainFile ? main : header;
    proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
  PropAccess* sourceField = forAll->getPropSource();
  Identifier* sourceId = forAll->getSource();
  // Identifier* extractId;
  Identifier* collectionId;
  if (sourceField != NULL) {
    collectionId = sourceField->getIdentifier1();
  }
  if (sourceId != NULL) {
    collectionId = sourceId;
  }
  Identifier* iteratorMethodId;
  if (extractElemFunc != NULL)
    iteratorMethodId = extractElemFunc->getMethodId();
  statement* body = forAll->getBody();
  char strBuffer[1024];
  if (forAll->isForall()) {  // IS FORALL

    /*
    if (forAll->hasFilterExpr()) {
      Expression* filterExpr = forAll->getfilterExpr();
      Expression* lhs = filterExpr->getLeft();
      Identifier* filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
      TableEntry* tableEntry = filterId != NULL ? filterId->getSymbolInfo() : NULL;


      if (tableEntry != NULL && tableEntry->getId()->get_fp_association()) {
        generateForAll_header(forAll, false);
      } else {
        generateForAll_header(forAll, false);
      }

    } else {
      //~ std::cout << "\t\tFFOOOOOORRRRR" << '\n';
      generateForAll_header(forAll, isMainFile);
    }
    */
    printf("Entered here for forall \n");

    if (!isOptimized) {
      std::cout<< "============EARLIER NOT OPT=============" << '\n';
      usedVariables usedVars = getVarsForAll(forAll);
      list<Identifier*> vars = usedVars.getVariables();
      
      

      for (Identifier* iden : vars) {

        if(iden == NULL || iden->getIdentifier() == NULL){
            cout << "iden is NULL ---------------";
        }
        std::cout<< "varName:" << iden->getIdentifier() << '\n';
      
       // Type* type = iden->getSymbolInfo()->getType();

       // if (type->isPrimitiveType())
         // generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
        /*else if(type->isPropType())
          {
            Type* innerType = type->getInnerTargetType();
            string dIden = "d_" + string(iden->getIdentifier());
            generateCudaMemCpyStr(dIden.c_str(), iden->getIdentifier(), convertToCppType(innerType), "V", true);
          }*/
      }
    }
    /*memcpy to symbol*/

    //main.pushString(getCurrentFunc()->getIdentifier()->getIdentifier());
    main.pushString("_kernel");
    main.pushString("<<<");
    main.pushString("numBlocks, threadsPerBlock");
    main.pushString(">>>");
    main.push('(');
    main.pushString("V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next");
    //  if(currentFunc->getParamList().size()!=0)
    // main.pushString(",");
    if (!isOptimized) {
      std::cout<< "NOT OPTIMESED ---------------" << '\n';
      usedVariables usedVars = getVarsForAll(forAll);
      list<Identifier*> vars = usedVars.getVariables();
      for (Identifier* iden : vars) {
        Type* type = iden->getSymbolInfo()->getType();
        if (type->isPropType()) {
          main.pushString(",");
          main.pushString("d_");
          main.pushString(/*createParamName(*/ iden->getIdentifier());
        }
      }
    } else {
      std::cout<< "INN OPTIMESED ---------------" << '\n';
      for (Identifier* iden : forAll->getUsedVariables()) {
        std::cout<< "_" << '\n';
        Type* type = iden->getSymbolInfo()->getType();
        if (type->isPropType()) {
          main.pushString(",");
          main.pushString("d_");
          main.pushString(/*createParamName(*/ iden->getIdentifier());
        }
      }
    }
    main.pushString(")");
    main.push(';');
    main.NewLine();

    main.pushString("cudaDeviceSynchronize();");
    main.NewLine();

    if (!isOptimized) {
      usedVariables usedVars = getVarsForAll(forAll);
      list<Identifier*> vars = usedVars.getVariables();
      for (Identifier* iden : vars) {
        Type* type = iden->getSymbolInfo()->getType();
        if (type->isPrimitiveType())
          generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), false);
        /*else if(type->isPropType())
          {
            Type* innerType = type->getInnerTargetType();
            string dIden = "d_" + string(iden->getIdentifier());
            generateCudaMemCpyStr(iden->getIdentifier(),dIden.c_str(), convertToCppType(innerType), "V", false);
          }*/
      }
      /*memcpy from symbol*/
    }

    main.NewLine();
    // main.pushString("cudaMemcpyFromSymbol(&diff_check, diff, sizeof(float));");
    main.NewLine();
    // main.pushString("diff = diff_check;");
    main.NewLine();

    //~ main.pushString("// cudaDeviceSynchronize(); //SSSP");
    //~ main.NewLine();
    //~ main.NewLine();

    //~ if (forAll->hasFilterExpr()) {
    //~ blockStatement* changedBody = includeIfToBlock(forAll);
    //~ cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
    //~ forAll->setBody(changedBody);
    //~ // cout<<"FORALL BODY
    //~ // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
    //~ }

    addCudaKernel(forAll);

  }

  else {  // IS FOR

    generateForAllSignature(forAll, false);  // FOR LINE

    if (forAll->hasFilterExpr()) {
      blockStatement* changedBody = includeIfToBlock(forAll);
      cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
      forAll->setBody(changedBody);
      // cout<<"FORALL BODY
      // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
    }

    if (extractElemFunc != NULL) {
      forallStack.push_back(make_pair(forAll->getIterator(),forAll->getExtractElementFunc())); 

      if (neighbourIteration(iteratorMethodId->getIdentifier())) {  // todo forall neigbour iterion
        cout << "\t ITERATE Neighbour \n";

        //~ char* tmpStr = forAll->getSource()->getIdentifier();
        char* wItr = forAll->getIterator()->getIdentifier();  // w iterator
        std::cout << "src:" << wItr << '\n';
        //~ char* gVar = forAll->getSourceGraph()->getIdentifier();     //g variable
        //~ std::cout<< "G:" << gVar << '\n';
        char* nbrVar;

        if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS) {
          list<argument*> argList = extractElemFunc->getArgList();
          assert(argList.size() == 1);
          Identifier* nodeNbr = argList.front()->getExpr()->getId();
          //~ targetFile.pushstr_newL("FOR begin | nbr iterate"); // ITERATE BFS
          nbrVar = nodeNbr->getIdentifier();
          //~ std::cout<< "nbr?:" <<  nbrVar<< '\n';

          //~ sprintf(strBuffer, "for(unsigned i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
          //~ targetFile.pushstr_newL(strBuffer);

          //~ // HAS ALL THE STMTS IN FOR
          //~ targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.

          //~ sprintf(strBuffer, "unsigned %s = d_data[i];", wItr);
          //~ targetFile.pushstr_newL(strBuffer);

          sprintf(strBuffer, "if(d_level[%s] == -1) {", wItr);
          targetFile.pushstr_newL(strBuffer);
          sprintf(strBuffer, "d_level[%s] = *d_hops_from_source + 1;", wItr);

          targetFile.pushstr_newL(strBuffer);
          targetFile.pushstr_newL("*d_finished = false;");
          targetFile.pushstr_newL("}");

          sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source + 1) {", wItr);
          targetFile.pushstr_newL(strBuffer);

          generateBlock((blockStatement*)forAll->getBody(), false, false);

          targetFile.pushstr_newL("}");

          targetFile.pushstr_newL("}");

          // HAS ALL THE STMTS IN FOR
          //~ targetFile.pushstr_newL("{");
          //~ generateStatement(forAll->getBody(), false); //false. All these stmts should be inside kernel
          //~ targetFile.pushstr_newL("}");

          //~ sprintf(strBuffer, "if(bfsDist[%s]==bfsDist[%s]+1)",
          //~ forAll->getIterator()->getIdentifier(),
          //~ nodeNbr->getIdentifier());
          //~ targetFile.pushstr_newL(strBuffer);
          //~ targetFile.pushstr_newL("{");
        }

        /* This can be merged with above condition through or operator but
         separating both now, for any possible individual construct updation.*/

        else if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {  // ITERATE REV BFS
          char strBuffer[1024];
          list<argument*> argList = extractElemFunc->getArgList();
          assert(argList.size() == 1);
          Identifier* nodeNbr = argList.front()->getExpr()->getId();
          nbrVar = nodeNbr->getIdentifier();
          std::cout << "V?:" << nbrVar << '\n';
          //~ sprintf(strBuffer, "for(int i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
          //~ targetFile.pushstr_newL(strBuffer);

          // HAS ALL THE STMTS IN FOR
          //~ targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.
          //~ sprintf(strBuffer, "int %s = d_data[i];", wItr);
          //~ targetFile.pushstr_newL(strBuffer);
          sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source) {", wItr);
          targetFile.pushstr_newL(strBuffer);
          generateBlock((blockStatement*)forAll->getBody(), false, false);
          targetFile.pushstr_newL("} // end IF  ");
          targetFile.pushstr_newL("} // end FOR");
          targetFile.pushstr_newL("grid.sync(); // Device-level syncs across all grids. Ref:https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#grid-synchronization-cg ");

          //~ targetFile.pushstr_newL("FOR begin | nbr iterate");
          //~ if(forAll->isForall() && forAll->hasFilterExpr()){
          //~ std::cout<< "HAS FILTER?" << '\n';
          //~ }
          //~ Identifier* nodeNbr = argList.front()->getExpr()->getId();
          //~ sprintf(strBuffer, "if(bfsDist[%s]==bfsDist[%s]+1)",
          //~ forAll->getIterator()->getIdentifier(),
          //~ nodeNbr->getIdentifier());
          //~ targetFile.pushstr_newL(strBuffer);
          //~ targetFile.pushstr_newL("{");
        } 
        
        else {
          //~ std::cout<< "FOR BODY BEGIN" << '\n';
          //~ targetFile.pushstr_newL("{ // FOR BEGIN ITR BEGIN");
          generateStatement(forAll->getBody(), isMainFile);
          targetFile.pushstr_newL("} //  end FOR NBR ITR. TMP FIX!");
          std::cout << "FOR BODY END" << '\n';
        }

        //~ if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS ||
        //~ forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
        //~ targetFile.pushstr_newL("}");
        //~ targetFile.pushstr_newL("}");

      } 
      
      else {
        printf("FOR NORML");
        generateStatement(forAll->getBody(), false);
      }

      if (forAll->isForall() && forAll->hasFilterExpr()) {
        Expression* filterExpr = forAll->getfilterExpr();
        generatefixedpt_filter(filterExpr, false);
      }

    } else {
      if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {  //FOR SET
        if (body->getTypeofNode() == NODE_BLOCKSTMT) {
          targetFile.pushstr_newL("{");  // uncomment after fixing NBR FOR brackets } issues.
          //~ targetFile.pushstr_newL("//HERE");
          printf("FOR");
          sprintf(strBuffer, "int %s = *itr;", forAll->getIterator()->getIdentifier());
          targetFile.pushstr_newL(strBuffer);
          generateBlock((blockStatement*)body, false);  //FOR BODY for
          targetFile.pushstr_newL("}");
        } else
          generateStatement(forAll->getBody(), false);

      }
       else if(collectionId->getSymbolInfo()->getType()->gettypeId()==TYPE_UPDATES)
       {

        if(body->getTypeofNode()==NODE_BLOCKSTMT)
        main.pushstr_newL("{");
        sprintf(strBuffer,"update %s = %s[i];",forAll->getIterator()->getIdentifier(),collectionId->getIdentifier()); 
        main.pushstr_newL(strBuffer);
        if(body->getTypeofNode()==NODE_BLOCKSTMT)
        {
          generateBlock((blockStatement*)body,false);
          main.pushstr_newL("}");
        }
        else
         generateStatement(forAll->getBody(), true);  //check once

       }

       else if(collectionId->getSymbolInfo()->getType()->gettypeId()==TYPE_CONTAINER){

        if(body->getTypeofNode()==NODE_BLOCKSTMT){
           generateBlock((blockStatement*)body,false);
        }
        else
         generateStatement(forAll->getBody(),isMainFile);

        main.pushstr_newL("}");
     }
      
      
       else {
        //~ cout << iteratorMethodId->getIdentifier() << "\n";
        generateStatement(forAll->getBody(), false);
      }
      if(forAll->isForall() && (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc()))
     { 
         
       checkAndGenerateFixedPtFilter(forAll, isMainFile);
      
     }
    
    }

    if(forAll->isForall()) {
    parallelConstruct.pop_back();

 if(forAll->getMapLocal().size() > 0) {

    /* write logic here */

    set<Identifier*>  containerId = forAll->getMapLocal();
    auto it = containerId.begin();
    Identifier* id = *it;
    int start = 0;
    generateForMergeContainer(id->getSymbolInfo()->getType(), start,isMainFile);
    char val = 'k' + start + 1;
    sprintf(strBuffer, "for(int %c = 0 ; %c < omp_get_max_threads() ; %c++)", val, val, val);
    main.pushstr_newL(strBuffer);
    generateInserts(id->getSymbolInfo()->getType(), id, isMainFile);

 }   

} 
}


   
    
} 




void dsl_dyn_cpp_generator::generateInDecHeader(Function* inDecFunc, bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
  char temp[1024];
  //dslCodePad& targetFile = main;
  if(inDecFunc->getFuncType()==INCREMENTAL_FUNC)
   {
      sprintf(temp,"%s_add",fileName);
   }
  else
   {
     sprintf(temp,"%s_del",fileName);
   }

  if(inDecFunc->containsReturn()) 
     targetFile.pushString("auto ");
  else
    targetFile.pushString("void ");
        
  targetFile.pushString(temp);
  targetFile.push('(');

  generateParamList(inDecFunc->getParamList(), targetFile);
  
 /* int maximum_arginline=4;
  int arg_currNo=0;
  int argumentTotal=inDecFunc->getParamList().size();
  list<formalParam*> paramList=inDecFunc->getParamList();
  list<formalParam*>::iterator itr;
  for(itr=paramList.begin();itr!=paramList.end();itr++)
  {
      arg_currNo++;
      argumentTotal--;

      Type* type=(*itr)->getType();
      targetFile.pushString(convertToCppType(type));*/
      /*if(type->isPropType())
      {
          targetFile.pushString("* ");
      }
      else 
      {*/
         // targetFile.pushString(" ");
         // targetFile.space();
      //}   
      /*targetFile.pushString(/*createParamName(*//*(*itr)->getIdentifier()->getIdentifier());
    /*  if(argumentTotal>0)
         targetFile.pushString(",");

      if(arg_currNo==maximum_arginline)
      {
         targetFile.NewLine();  
         arg_currNo=0;  
      } 
     // if(argumentTotal==0)
         
  }*/

   targetFile.pushString(")");
   targetFile.NewLine();

    return; 

}

void dsl_dyn_cpp_generator::generateDynamicHeader(Function* dynFunc, bool isMainFile)
{

  dslCodePad& targetFile = isMainFile ? main : header;
  char temp[1024];
  //dslCodePad& targetFile = main;
 
 sprintf(temp,"%s",dynFunc->getIdentifier()->getIdentifier());
  
 if(dynFunc->containsReturn()) 
     targetFile.pushString("auto ");
  else
    targetFile.pushString("void ");
        
  targetFile.pushString(temp);
  targetFile.push('(');

  generateParamList(dynFunc->getParamList(), targetFile);

   targetFile.pushString(")");
   targetFile.NewLine();

    return; 


}

void dsl_dyn_cpp_generator::generateIncremental(Function* incFunc, bool isMainFile)
{
  printf("inside incremental block-------------------");

   dslCodePad& targetFile = isMainFile ? main : header;
   char strBuffer[1024];
   curFuncType = incFunc->getFuncType();
   currentFunc = incFunc;
   generateInDecHeader(incFunc, isMainFile);
   main.pushstr_newL("{");
   if(incFunc->getInitialLockDecl())
       {
         vector<Identifier*> graphVar = graphId[curFuncType][curFuncCount()]; 
         sprintf(strBuffer,"omp_lock_t* lock = (omp_lock_t*)malloc(%s.num_nodes()*sizeof(omp_lock_t));",graphVar[0]->getIdentifier());
         main.pushstr_newL(strBuffer);
         main.NewLine();
         sprintf(strBuffer,"for(%s %s = %s; %s<%s.%s(); %s++)","int","v","0","v",graphVar[0]->getIdentifier(),"num_nodes","v");
         main.pushstr_newL(strBuffer);
         sprintf(strBuffer,"omp_init_lock(&lock[%s]);","v");
         main.space();
         main.space();
         main.pushstr_newL(strBuffer);
         main.NewLine();

       }
   generatePriorDeclarations(incFunc, isMainFile);
   generateBlock(incFunc->getBlockStatement(),false);
   main.NewLine();
   main.pushstr_newL("}");
   incFuncCount(incFunc->getFuncType());
   return;

}

void dsl_dyn_cpp_generator::generateDecremental(Function* decFunc, bool isMainFile)
{

   dslCodePad& targetFile = isMainFile ? main : header;
   char strBuffer[1024];
   curFuncType = decFunc->getFuncType();
   currentFunc = decFunc;
   generateInDecHeader(decFunc, true);

   main.pushstr_newL("{");

   if(decFunc->getInitialLockDecl())
       {
         vector<Identifier*> graphVar = graphId[curFuncType][curFuncCount()]; 
         sprintf(strBuffer,"omp_lock_t* lock = (omp_lock_t*)malloc(%s.num_nodes()*sizeof(omp_lock_t));",graphVar[0]->getIdentifier());
         main.pushstr_newL(strBuffer);
         main.NewLine();
         sprintf(strBuffer,"for(%s %s = %s; %s<%s.%s(); %s++)","int","v","0","v",graphVar[0]->getIdentifier(),"num_nodes","v");
         main.pushstr_newL(strBuffer);
         sprintf(strBuffer,"omp_init_lock(&lock[%s]);","v");
         main.space();
         main.space();
         main.pushstr_newL(strBuffer);
         main.NewLine();

       }

   generateBlock(decFunc->getBlockStatement(),false);
   main.NewLine();
   main.pushstr_newL("}");
   incFuncCount(decFunc->getFuncType());
   return;

}

void dsl_dyn_cpp_generator::generateDynamicFunc(Function* dynFunc, bool isMainFile)
{

   
   char strBuffer[1024];
   curFuncType = dynFunc->getFuncType();
   currentFunc = dynFunc;
   generateDynamicHeader(dynFunc, isMainFile);

   main.pushstr_newL("{");

   if(dynFunc->getInitialLockDecl())
       {
         vector<Identifier*> graphVar = graphId[curFuncType][curFuncCount()]; 
         sprintf(strBuffer,"omp_lock_t* lock = (omp_lock_t*)malloc(%s.num_nodes()*sizeof(omp_lock_t));",graphVar[0]->getIdentifier());
         main.pushstr_newL(strBuffer);
         main.NewLine();
         sprintf(strBuffer,"for(%s %s = %s; %s<%s.%s(); %s++)","int","v","0","v",graphVar[0]->getIdentifier(),"num_nodes","v");
         main.pushstr_newL(strBuffer);
         sprintf(strBuffer,"omp_init_lock(&lock[%s]);","v");
         main.space();
         main.space();
         main.pushstr_newL(strBuffer);
         main.NewLine();

       }
   generatePriorDeclarations(dynFunc, isMainFile);
   generateBlock(dynFunc->getBlockStatement(),false);
   main.NewLine();
   main.pushstr_newL("}");
   incFuncCount(dynFunc->getFuncType());
   return;


}

void dsl_dyn_cpp_generator::generateFunction(ASTNode* func)
{

  //dslCodePad& targetFile = isMainFile ? main : header;
  Function* function = (Function*)func;
  if(function->getFuncType() == STATIC_FUNC)
     {
       generateFunc(function);
       
     }
   else if(function->getFuncType() == INCREMENTAL_FUNC)
   {
        generateIncremental(function, true);
   }
   else if(function->getFuncType() == DECREMENTAL_FUNC)
       {
         generateDecremental(function, true);
           
       }
   else if(function->getFuncType() == DYNAMIC_FUNC)
        {      
          generateDynamicFunc(function, true);
           
        }
   else if(function->getFuncType() == GEN_FUNC)
         {
            generateFunc(function);
         }         
}

bool dsl_dyn_cpp_generator::openFileforOutput()
{  

  char temp[1024];
  printf("fileName %s\n",fileName); 

  sprintf(temp, "%s/%s_dyn.h", "../graphcode/generated_cuda", fileName);
  headerFile = fopen(temp, "w");
  if (headerFile == NULL) return false;
  header.setOutputFile(headerFile);

  sprintf(temp,"%s/%s_dyn.cu","../graphcode/generated_cuda",fileName);
  bodyFile=fopen(temp,"w"); 
  if(bodyFile==NULL)
     return false;
  main.setOutputFile(bodyFile);     
  
  return true;

}

void dsl_dyn_cpp_generator::closeOutputFile() {
  if (headerFile != NULL) {
    header.outputToFile();
    fclose(headerFile);
  }
  headerFile = NULL;

  if (bodyFile != NULL) {
    main.outputToFile();
    fclose(bodyFile);
  }

  bodyFile = NULL;
}

void dsl_dyn_cpp_generator::generation_begin()
{ 
  char temp[1024];
  header.pushstr_newL("// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after");
  main.pushstr_newL("// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after");
  header.pushString("#ifndef GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#define GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#include ");
  addIncludeToFile("stdio.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("stdlib.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("limits.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("cuda.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("../graph.hpp", header, false);
  header.pushString("#include ");
  addIncludeToFile("../libcuda.cuh", header, false);

  header.pushstr_newL("#include <cooperative_groups.h>");
  //header.pushstr_newL("graph &g = NULL;");  //temporary fix - to fix the PageRank graph g instance

  header.NewLine();

  main.pushString("#include ");
  sprintf(temp, "%s.h", fileName);
  addIncludeToFile(temp, main, false);
  main.NewLine();

}

bool dsl_dyn_cpp_generator::generate()
{  
  // cout<<"FRONTEND VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();    //openFileforOutput();
   if(!openFileforOutput())
      return false;
   generation_begin(); 
   
   list<Function*> funcList=frontEndContext.getFuncList();
   for(Function* func:funcList)
   {
       generateFunction(func);

   }
   

   closeOutputFile();

   return true;

}
}


