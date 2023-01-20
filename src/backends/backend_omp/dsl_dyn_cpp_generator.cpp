#include "dsl_dyn_cpp_generator.hpp"

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



void dsl_dyn_cpp_generator::generateOnDeleteBlock(onDeleteBlock* onDeleteStmt)
{
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

void dsl_dyn_cpp_generator::generateOnAddBlock(onAddBlock* onAddStmt)
{
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

void dsl_dyn_cpp_generator::generateFreeInCurrentBatch()
{
  char strBuffer[1024];
  for(int i=0 ; i < freeIdStore.size() ; i++)
     { 
       Identifier* id = freeIdStore[i];
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

void dsl_dyn_cpp_generator::generateBatchBlock(batchBlock* batchStmt)
{
  
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
      generateExpr(batchStmt->getBatchSizeExpr());
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
      generateFreeInCurrentBatch();
      main.pushstr_newL("}");

      resetBatchEnv();

    }

  

    insideBatchBlock = false;


}

void dsl_dyn_cpp_generator::generateStatement(statement* stmt)
{ 
   if(stmt->getTypeofNode()==NODE_BLOCKSTMT)
     {
       generateBlock((blockStatement*)stmt);

     }
   if(stmt->getTypeofNode()==NODE_DECL)
   {
     
      generateVariableDecl((declaration*)stmt);

   } 
   if(stmt->getTypeofNode()==NODE_ASSIGN)
     { 
       
       generateAssignmentStmt((assignment*)stmt);
     }
    
   if(stmt->getTypeofNode()==NODE_WHILESTMT) 
   {
     generateWhileStmt((whileStmt*) stmt);
   }
   
   if(stmt->getTypeofNode()==NODE_IFSTMT)
   {
      generateIfStmt((ifStmt*)stmt);
   }

   if(stmt->getTypeofNode()==NODE_DOWHILESTMT)
   {
      generateDoWhileStmt((dowhileStmt*) stmt);
   }

    if(stmt->getTypeofNode()==NODE_FORALLSTMT)
     {
       generateForAll((forallStmt*) stmt);
     }
  
    if(stmt->getTypeofNode()==NODE_FIXEDPTSTMT)
    {
      generateFixedPoint((fixedPointStmt*)stmt);
    }
    if(stmt->getTypeofNode()==NODE_REDUCTIONCALLSTMT)
    { 
      generateReductionStmt((reductionCallStmt*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_ITRBFS)
    {
      generateBFSAbstraction((iterateBFS*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_PROCCALLSTMT)
    { 
      generateProcCall((proc_callStmt*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_UNARYSTMT)
    {
        unary_stmt* unaryStmt=(unary_stmt*)stmt;
        generateExpr(unaryStmt->getUnaryExpr());
        main.pushstr_newL(";");
        
    }
    if(stmt->getTypeofNode() == NODE_RETURN)
       {
         returnStmt* returnStmtNode = (returnStmt*)stmt;
         main.pushstr_space("return");
         generateExpr(returnStmtNode->getReturnExpression());
         main.pushstr_newL(";");
       }
    if(stmt->getTypeofNode() == NODE_BATCHBLOCKSTMT)
       {

         generateBatchBlock((batchBlock*)stmt);

       }
    if(stmt->getTypeofNode() == NODE_ONADDBLOCK)
      {
        
        generateOnAddBlock((onAddBlock*)stmt);

      }
    if(stmt->getTypeofNode() == NODE_ONDELETEBLOCK)
      {
        
        generateOnDeleteBlock((onDeleteBlock*)stmt);
        
      }  
         

}


void dsl_dyn_cpp_generator::generate_exprProcCall(Expression* expr)
{
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
   printf("HERE PRESENT %s\n",proc->getMethodId()->getIdentifier());

  if(methodId=="get_edge")
  {
   
   // if(curFuncType == INCREMENTAL_FUNC || curFuncType == DECREMENTAL_FUNC)
        getEdgeTranslation(expr);
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
           generateArgList(proc->getArgList());         

        } 
   else
    {  /* for userdefined function calls */
        char strBuffer[1024];
        list<argument*> argList=proc->getArgList();
        Identifier* objectId = proc->getId1();
        if(objectId!=NULL)
          {
             Identifier* id2 = proc->getId2();
             if(id2 == NULL)
                 sprintf(strBuffer,"%s.%s.%s",objectId->getIdentifier(), id2->getIdentifier(), proc->getMethodId()->getIdentifier());
             else
                 sprintf(strBuffer,"%s.%s",objectId->getIdentifier(), proc->getMethodId()->getIdentifier());    
          }
        else
          {
            sprintf(strBuffer,"%s",proc->getMethodId()->getIdentifier());
          } 

        main.pushString(strBuffer);
        generateArgList(argList);   

    }
  

}


void dsl_dyn_cpp_generator::generate_exprPropId(PropAccess* propId) //This needs to be made more generic.
{ 
  
  char strBuffer[1024];
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  ASTNode* propParent=propId->getParent();
  //bool relatedToReduction = propParent!=NULL?propParent->getTypeofNode()==NODE_REDUCTIONCALLSTMT:false;
   if(id2->getSymbolInfo()!=NULL&&id2->getSymbolInfo()->getId()->get_fp_association())
    {
       bool relatedToUpdation = propParent!=NULL?((propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT|| propParent->getTypeofNode() == NODE_ASSIGN) && checkFixedPointAssociation(propId)):false;
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
  main.pushString(strBuffer);

}


void dsl_dyn_cpp_generator::generateForAllSignature(forallStmt* forAll)
{
  
  
  if(curFuncType == GEN_FUNC || curFuncType == STATIC_FUNC)
    {
      dsl_cpp_generator::generateForAllSignature(forAll);
    } 
  else
   { 
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
       }                                                                                                    //statement to a different method.

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
       }
   }
}

}

void dsl_dyn_cpp_generator::generateForAll(forallStmt* forAll)
{ 
   proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
   PropAccess* sourceField=forAll->getPropSource();
   Identifier* sourceId = forAll->getSource();
   //Identifier* extractId;
    Identifier* collectionId;
    if(sourceField!=NULL)
      {
        collectionId=sourceField->getIdentifier1();
        
      }
    if(sourceId!=NULL)
     {
        collectionId=sourceId;
       
     }  
    Identifier* iteratorMethodId;
    if(extractElemFunc!=NULL)
     iteratorMethodId=extractElemFunc->getMethodId();
    statement* body=forAll->getBody();
     char strBuffer[1024];
  if(forAll->isForall())
  { 
    if(forAll->hasFilterExpr() || forAll->hasFilterExprAssoc())
     {  
        Expression* filterExpr = NULL;
        if(forAll->hasFilterExpr())
           filterExpr = forAll->getfilterExpr();
        else
           filterExpr = forAll->getAssocExpr();   

        Expression* lhs = filterExpr->getLeft();
        Identifier* filterId=lhs->isIdentifierExpr()?lhs->getId():NULL;
        TableEntry* tableEntry=filterId!=NULL?filterId->getSymbolInfo():NULL;
         if(tableEntry!=NULL&&tableEntry->getId()->get_fp_association())
             {
               main.pushstr_newL("#pragma omp parallel");
               main.pushstr_newL("{");
               main.pushstr_newL("#pragma omp for");
             }
             else
             {
                generateForAll_header(forAll);
             }    
     }
     else
      generateForAll_header(forAll);
  }

  generateForAllSignature(forAll);

  if(forAll->hasFilterExpr())
  { 

    blockStatement* changedBody=includeIfToBlock(forAll);
   // cout<<"CHANGED BODY TYPE"<<(changedBody->getTypeofNode()==NODE_BLOCKSTMT);
    forAll->setBody(changedBody);
   // cout<<"FORALL BODY TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
  }
   
  if(extractElemFunc!=NULL)
  {  
   
  forallStack.push_back(make_pair(forAll->getIterator(),forAll->getExtractElementFunc())); 
  if(neighbourIteration(iteratorMethodId->getIdentifier()))
  { 
   
    if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS)
     {   
       
        
         list<argument*>  argList=extractElemFunc->getArgList();
         assert(argList.size()==1);
         Identifier* nodeNbr=argList.front()->getExpr()->getId();
         sprintf(strBuffer,"if(bfsDist[%s]==bfsDist[%s]+1)",forAll->getIterator()->getIdentifier(),nodeNbr->getIdentifier());
         main.pushstr_newL(strBuffer);
         main.pushstr_newL("{");
       
     }

     /* This can be merged with above condition through or operator but separating 
        both now, for any possible individual construct updation.*/

      if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRRBFS)
       {  

         char strBuffer[1024];
         list<argument*>  argList=extractElemFunc->getArgList();
         assert(argList.size()==1);
         Identifier* nodeNbr=argList.front()->getExpr()->getId();
         sprintf(strBuffer,"if(bfsDist[%s]==bfsDist[%s]+1)",forAll->getIterator()->getIdentifier(),nodeNbr->getIdentifier());
         main.pushstr_newL(strBuffer);
         main.pushstr_newL("{");

       }

     generateBlock((blockStatement*)forAll->getBody(),false);
     if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS||forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRRBFS)
       main.pushstr_newL("}");
    main.pushstr_newL("}");
  }
  else
  { 
    
    generateStatement(forAll->getBody());
     
  }

   if(forAll->isForall() && (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc()))
     { 
      
       checkAndGenerateFixedPtFilter(forAll);
      
     }
   
    forallStack.pop_back();
   
  }
  else 
  {   
    
   if(collectionId->getSymbolInfo()->getType()->gettypeId()==TYPE_SETN)
     {
      if(body->getTypeofNode()==NODE_BLOCKSTMT)
      main.pushstr_newL("{");
      sprintf(strBuffer,"int %s = *itr;",forAll->getIterator()->getIdentifier()); 
      main.pushstr_newL(strBuffer);
      if(body->getTypeofNode()==NODE_BLOCKSTMT)
      {
        generateBlock((blockStatement*)body,false);
        main.pushstr_newL("}");
      }
      else
         generateStatement(forAll->getBody());
        
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
         generateStatement(forAll->getBody());

       }
     else
      {
     
       cout<<iteratorMethodId->getIdentifier()<<"\n";
       generateStatement(forAll->getBody());
  
    }
    
 if(forAll->isForall() && (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc()))
     { 
       
       checkAndGenerateFixedPtFilter(forAll);
      
     }
  }
    
} 




void dsl_dyn_cpp_generator::generateInDecHeader(Function* inDecFunc)
{
 
  char temp[1024];
  dslCodePad& targetFile = main;
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

void dsl_dyn_cpp_generator::generateDynamicHeader(Function* dynFunc)
{
  char temp[1024];
  dslCodePad& targetFile = main;
 
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

void dsl_dyn_cpp_generator::generateIncremental(Function* incFunc)
{
   char strBuffer[1024];
   curFuncType = incFunc->getFuncType();
   currentFunc = incFunc;
   generateInDecHeader(incFunc);
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

   generateBlock(incFunc->getBlockStatement(),false);
   main.NewLine();
   main.pushstr_newL("}");
   incFuncCount(incFunc->getFuncType());
   return;


}

void dsl_dyn_cpp_generator::generateDecremental(Function* decFunc)
{
   char strBuffer[1024];
   curFuncType = decFunc->getFuncType();
   currentFunc = decFunc;
   generateInDecHeader(decFunc);

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

void dsl_dyn_cpp_generator::generateDynamicFunc(Function* dynFunc)
{

   char strBuffer[1024];
   curFuncType = dynFunc->getFuncType();
   currentFunc = dynFunc;
   generateDynamicHeader(dynFunc);

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

   generateBlock(dynFunc->getBlockStatement(),false);
   main.NewLine();
   main.pushstr_newL("}");
   incFuncCount(dynFunc->getFuncType());
   return;


}

void dsl_dyn_cpp_generator::generateFunction(ASTNode* func)
{
  Function* function = (Function*)func;
  if(function->getFuncType() == STATIC_FUNC)
     {
       generateFunc(function);
       
     }
   else if(function->getFuncType() == INCREMENTAL_FUNC)
   {
        generateIncremental(function);
   }
   else if(function->getFuncType() == DECREMENTAL_FUNC)
       {
         generateDecremental(function);
           
       }
   else if(function->getFuncType() == DYNAMIC_FUNC)
        {      
          generateDynamicFunc(function);
           
        }    

}

bool dsl_dyn_cpp_generator::openFileforOutput()
{  
  char temp[1024];
  printf("fileName %s\n",fileName); 

  sprintf(temp,"%s/%s_dyn.cc","../graphcode/generated_omp",fileName);
  bodyFile=fopen(temp,"w"); 
  if(bodyFile==NULL)
     return false;
  main.setOutputFile(bodyFile);     
  
  return true;

}

void dsl_dyn_cpp_generator::closeOutputFile()
{
   if(bodyFile!=NULL)
   {
     main.outputToFile();
     fclose(bodyFile);
   }

   bodyFile=NULL;
}

void dsl_dyn_cpp_generator::generation_begin()
{ 
  char temp[1024];  
  main.pushString("#include");
  addIncludeToFile("stdio.h",main,true);
  main.pushString("#include");
  addIncludeToFile("stdlib.h",main,true);
  main.pushString("#include");
  addIncludeToFile("limits.h",main,true);
  main.pushString("#include");
  addIncludeToFile("atomic",main,true);
  main.pushString("#include");
  addIncludeToFile("omp.h",main,true);
  main.pushString("#include");
  addIncludeToFile("../graph.hpp",main,false);
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

