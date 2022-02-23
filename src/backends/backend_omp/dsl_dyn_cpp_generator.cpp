#include "dsl_dyn_cpp_generator.hpp"



void dsl_dyn_cpp_generator::generate_exprPropId(PropAccess* propId) //This needs to be made more generic.
{ 
  
  char strBuffer[1024];
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  ASTNode* propParent=propId->getParent();
  bool relatedToReduction = propParent!=NULL?propParent->getTypeofNode()==NODE_REDUCTIONCALLSTMT:false;
  if(id2->getSymbolInfo()!=NULL&&id2->getSymbolInfo()->getId()->get_fp_association()&&relatedToReduction)
    {
      sprintf(strBuffer,"%s_nxt[%s]",id2->getIdentifier(),id1->getIdentifier());
    }
    else
    {
      if(id2->getSymbolInfo()==NULL)
        {
          sprintf(strBuffer, "%s.%s",id1->getIdentifier(),id2->getIdentifier());
        }
      else 
       {
        if(curFuncType == INCREMENTAL_FUNC || curFuncType == DECREMENTAL_FUNC)
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
    if(forAll->hasFilterExpr())
     {
        Expression* filterExpr=forAll->getfilterExpr();
        Expression* lhs=filterExpr->getLeft();
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

  if(forAll->isForall()&&forAll->hasFilterExpr())
       { 
        Expression* filterExpr=forAll->getfilterExpr();
        generatefixedpt_filter(filterExpr);
      
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
    
    if(forAll->isForall()&&forAll->hasFilterExpr())
     { 
        Expression* filterExpr=forAll->getfilterExpr();
        generatefixedpt_filter(filterExpr);
      
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
  targetFile.pushString("void ");
  targetFile.pushString(temp);
  targetFile.push('(');
  
  int maximum_arginline=4;
  int arg_currNo=0;
  int argumentTotal=inDecFunc->getParamList().size();
  list<formalParam*> paramList=inDecFunc->getParamList();
  list<formalParam*>::iterator itr;
  for(itr=paramList.begin();itr!=paramList.end();itr++)
  {
      arg_currNo++;
      argumentTotal--;

      Type* type=(*itr)->getType();
      targetFile.pushString(convertToCppType(type));
      /*if(type->isPropType())
      {
          targetFile.pushString("* ");
      }
      else 
      {*/
          targetFile.pushString(" ");
         // targetFile.space();
      //}   
      targetFile.pushString(/*createParamName(*/(*itr)->getIdentifier()->getIdentifier());
      if(argumentTotal>0)
         targetFile.pushString(",");

      if(arg_currNo==maximum_arginline)
      {
         targetFile.NewLine();  
         arg_currNo=0;  
      } 
     // if(argumentTotal==0)
         
  }

   targetFile.pushString(")");
   targetFile.NewLine();

    return; 



}


void dsl_dyn_cpp_generator::generateIncremental(Function* incFunc)
{
   char strBuffer[1024];
   curFuncType = incFunc->getFuncType();
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

void dsl_dyn_cpp_generator::generateFunction(ASTNode* func)
{
  Function* function = (Function*)func;
  if(function->getFuncType()==STATIC_FUNC)
    {
       generateFunc(function);
       
    }
   else if(function->getFuncType()==INCREMENTAL_FUNC)
   {
        generateIncremental(function);
   }
   else if(function->getFuncType()==DECREMENTAL_FUNC)
       {
         generateDecremental(function);
           
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

