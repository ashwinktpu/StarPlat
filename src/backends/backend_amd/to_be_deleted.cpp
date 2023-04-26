#include "dsl_cpp_generator.h"
#include<string.h>
#include<cassert>


char *fixedpointVarforSSSP = "";

void dsl_cpp_generator::addIncludeToFile(char* includeName,dslCodePad& file,bool isCppLib)
{  //cout<<"ENTERED TO THIS ADD INCLUDE FILE"<<"\n";
    if(!isCppLib)
      file.push('"');
    else 
      file.push('<');  
     
     file.pushString(includeName);
     if(!isCppLib)
       file.push('"');
     else 
       file.push('>');
     file.NewLine();     
 }

void dsl_cpp_generator::generation_begin()
{ 
  char temp[1024];  
  header.pushString("#ifndef GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#define GENCPP_");
   header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#include");
  addIncludeToFile("stdio.h",header,true);
  header.pushString("#include");
  addIncludeToFile("stdlib.h",header,true);
  header.pushString("#include");
  addIncludeToFile("limits.h",header,true);
  header.pushString("#include");
  addIncludeToFile("cuda.h",header,true);
  header.pushString("#include");
  addIncludeToFile("graph.hpp",header,false);
  header.NewLine();
  main.pushString("#include");
  sprintf(temp,"%s.h",fileName);
  addIncludeToFile(temp,main,false);
  main.NewLine();

}

void add_InitialDeclarations(dslCodePad* main,iterateBFS* bfsAbstraction)
{
   
  char strBuffer[1024];
  char* graphId=bfsAbstraction->getGraphCandidate()->getIdentifier();
  sprintf(strBuffer,"std::vector<std::vector<int>> %s(%s.%s()) ;","levelNodes",graphId,"num_nodes");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"std::vector<int>  %s(%s.%s()) ;","levelNodes_later",graphId,"num_nodes");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"std::vector<int>  %s(%s.%s()) ;","levelCount",graphId,"num_nodes");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("int phase = 0 ;");
  sprintf(strBuffer,"levelNodes[phase].push_back(%s) ;",bfsAbstraction->getRootNode()->getIdentifier());
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"std::%s bfsCount = {%s} ;","atomic_int","1");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("levelCount[phase] = bfsCount;");



}

 void add_BFSIterationLoop(dslCodePad* main, iterateBFS* bfsAbstraction)
 {
   
    char strBuffer[1024];
    char* iterNode=bfsAbstraction->getIteratorNode()->getIdentifier();
    char* graphId=bfsAbstraction->getGraphCandidate()->getIdentifier();
    main->pushstr_newL("while ( bfsCount > 0 )");
    main->pushstr_newL("{");
    main->pushstr_newL(" int prev_count = bfsCount ;");
    main->pushstr_newL("bfsCount = 0 ;");
    main->pushstr_newL("#pragma omp for all");
    sprintf(strBuffer,"for( %s %s = %s; %s = prev_count ; %s++)","int","l","0","l","l");
    main->pushstr_newL(strBuffer);
    main->pushstr_newL("{");
    sprintf(strBuffer,"int %s = levelNodes[phase][%s] ;",iterNode,"l");
    main->pushstr_newL(strBuffer);
    sprintf(strBuffer,"for(%s %s = %s.%s[%s] ; %s < %s.%s[%s+1] ; %s++) ","int","edge",graphId,"indexofNodes",iterNode,"edge",graphId,"indexofNodes",iterNode,"edge");
     main->pushString(strBuffer);
     main->pushstr_newL("{");
     sprintf(strBuffer,"%s %s = %s.%s[%s] ;","int","nbr",graphId,"edgeList","edge");
     main->pushstr_newL(strBuffer);
     main->pushstr_newL("int dnbr ;");
     sprintf(strBuffer,"dnbr = %s(&dist[nbr],-1,dist[%s]+1);","__sync_val_compare_and_swap",iterNode);
     main->pushstr_newL(strBuffer);
     main->pushstr_newL("if (dnbr < 0)");
     main->pushstr_newL("{");
     sprintf(strBuffer,"int %s = bfsCount.fetch_add(%s,%s) ;","loc","1","std::memory_order_relaxed");
     main->pushstr_newL(strBuffer);
     sprintf(strBuffer," levelNodes_later[%s]=nbr ;","loc");
     main->pushstr_newL(strBuffer);
     main->pushstr_newL("}");
     main->pushstr_newL("}");
  
  }

  void add_RBFSIterationLoop(dslCodePad* main, iterateBFS* bfsAbstraction)
  {
   
    char strBuffer[1024];    
    main->pushstr_newL("while (phase > 0)") ;
    main->pushstr_newL("{");
    main->pushstr_newL("#pragma omp parallel for");
    sprintf(strBuffer,"for( %s %s = %s; %s = levelCount[phase] ; %s++)","int","i","0","l","l"); 
    main->pushstr_newL(strBuffer);
    main->pushstr_newL("{");
    sprintf(strBuffer,"int %s = levelCount[phase][i] ;",bfsAbstraction->getIteratorNode()->getIdentifier());
    main->pushstr_newL(strBuffer);



  }

 void dsl_cpp_generator::generateBFSAbstraction(iterateBFS* bfsAbstraction)
 {
   add_InitialDeclarations(&main,bfsAbstraction);
  //printf("BFS ON GRAPH %s",bfsAbstraction->getGraphCandidate()->getIdentifier());
   add_BFSIterationLoop(&main,bfsAbstraction);
   statement* body=bfsAbstraction->getBody();
   assert(body->getTypeofNode()==NODE_BLOCKSTMT);
   blockStatement* block=(blockStatement*)body;
   list<statement*> statementList=block->returnStatements();
   for(statement* stmt:statementList)
   {
       generateStatement(stmt);
   }
   main.pushstr_newL("}");

   main.pushstr_newL("phase = phase + 1 ;");
   main.pushstr_newL("levelCount[phase] = bfsCount ;");
   main.pushstr_newL(" levelNodes[phase].assign(levelNodes_later.begin(),levelNodes_later.begin()+bfsCount);");
   main.pushstr_newL("}");
   main.pushstr_newL("phase = phase -1 ;");
   add_RBFSIterationLoop(&main,bfsAbstraction);
   blockStatement* revBlock=(blockStatement*)bfsAbstraction->getRBFS()->getBody();
   list<statement*> revStmtList=revBlock->returnStatements();

    for(statement* stmt:revStmtList)
    {
       generateStatement(stmt);
    }
   
   main.pushstr_newL("}");
   main.pushstr_newL("}");


 }
void dsl_cpp_generator::generateStatement1(statement* stmt)
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
    // generateWhileStmt((whileStmt*) stmt);
   }
   
   if(stmt->getTypeofNode()==NODE_IFSTMT)
   {
      generateIfStmt((ifStmt*)stmt);
   }

   if(stmt->getTypeofNode()==NODE_DOWHILESTMT)
   {
    //  generateBlock((blockStatement*) stmt);
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
    { cout<<"IS REDUCTION STMT HI"<<"\n";
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


}

void dsl_cpp_generator::generateStatement(statement* stmt)
{  
   if(stmt->getTypeofNode()==NODE_BLOCKSTMT)
     {
       generateBlock((blockStatement*)stmt);

     }
   if(stmt->getTypeofNode()==NODE_DECL)
   {
     
       // generateVariableDecl((declaration*)stmt);
        generateVariableDeclForEdge((declaration*)stmt); //this is only for int e = edge (will be changed later)

   } 
   if(stmt->getTypeofNode()==NODE_ASSIGN)
     { 
       
       //generateAssignmentStmt((assignment*)stmt);
     }
    
   if(stmt->getTypeofNode()==NODE_WHILESTMT) 
   {
    // generateWhileStmt((whileStmt*) stmt);
   }
   
   if(stmt->getTypeofNode()==NODE_IFSTMT)
   {
      generateIfStmt((ifStmt*)stmt);
   }

   if(stmt->getTypeofNode()==NODE_DOWHILESTMT)
   {
    //  generateBlock((blockStatement*) stmt);
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
    { cout<<"IS REDUCTION STMT HI"<<"\n";
      generateReductionStmt((reductionCallStmt*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_ITRBFS)
    {
      generateBFSAbstraction((iterateBFS*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_PROCCALLSTMT)
    { 
      //generateProcCall((proc_callStmt*) stmt);
    }

}

//note :: this will only generate the assignment and declration including propNode
void dsl_cpp_generator::generateStatementForSSSPBody(statement* stmt)
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
    // generateWhileStmt((whileStmt*) stmt);
   }
   
   if(stmt->getTypeofNode()==NODE_IFSTMT)
   {
      //generateIfStmt((ifStmt*)stmt);
   }

   if(stmt->getTypeofNode()==NODE_DOWHILESTMT)
   {
    //  generateBlock((blockStatement*) stmt);
   }

    if(stmt->getTypeofNode()==NODE_FORALLSTMT)
     {
       //generateForAll((forallStmt*) stmt);
     }
  
    if(stmt->getTypeofNode()==NODE_FIXEDPTSTMT)
    {
      //generateFixedPoint((fixedPointStmt*)stmt);
    }
    if(stmt->getTypeofNode()==NODE_REDUCTIONCALLSTMT)
    { cout<<"IS REDUCTION STMT HI"<<"\n";
      //generateReductionStmt((reductionCallStmt*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_ITRBFS)
    {
      //generateBFSAbstraction((iterateBFS*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_PROCCALLSTMT)
    { 
      generateProcCall((proc_callStmt*) stmt);
    }


}

void dsl_cpp_generator::generateAtomicBlock()
{

   main.pushstr_newL("atomicMin(&gpu_dist[nbr] , dist_new);");
   main.pushstr_newL("gpu_modified_next[nbr]=true;");
   main.pushstr_newL("gpu_finished[0] = false;");

}

void dsl_cpp_generator::generateReductionStmt(reductionCallStmt* stmt)
{ char strBuffer[1024];
  reductionCall* reduceCall=stmt->getReducCall(); 
  if(reduceCall->getReductionType()==REDUCE_MIN)
  {
    
    if(stmt->isListInvolved())
      {
        cout<<"INSIDE THIS OF LIST PRESENT"<<"\n";
        list<argument*> argList=reduceCall->getargList();
        list<ASTNode*>  leftList=stmt->getLeftList();
        int i=0;
      /*  ASTNode* a=leftList.front();
        PropAccess* p=(PropAccess*)a;
        cout<<" a's id 1"<<p->getIdentifier1()->getIdentifier()<<"\n";
        cout<<" a's id 2"<<p->getIdentifier2()->getIdentifier()<<"\n";
        for(ASTNode* l:leftList)
           {
             PropAccess* p=(PropAccess*)l;
             cout<<"ID 1 DSL"<<p->getIdentifier1()->getIdentifier()<<"\n";
             cout<<"ID 2 DSL"<<p->getIdentifier2()->getIdentifier()<<"\n";
           }*/
        list<ASTNode*> rightList=stmt->getRightList();
        printf("LEFT LIST SIZE %d \n",leftList.size());
      
            main.space();
            if(stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType())
            { Type* type=stmt->getAssignedId()->getSymbolInfo()->getType();
              
              main.pushstr_space(convertToCppType(type->getInnerTargetType()));
            }
            cout<<"INSIDE ARG ID"<<stmt->getAssignedId()->getSymbolInfo()->getType()->gettypeId()<<"\n";

          //  
            sprintf(strBuffer,"%s_new",stmt->getAssignedId()->getIdentifier());
            main.pushString(strBuffer);
            //printf("llllllllllllllllllll=%s",strBuffer);
            
            list<argument*>::iterator argItr;
             argItr=argList.begin();
             argItr++; 
            main.pushString(" = ");
            generateExpr((*argItr)->getExpr());
            main.pushstr_newL(";");
            
            //main.pushString("if (");
           // if(stmt->isTargetId())
           // generate_exprIdentifier(stmt->getTargetId());
            //else
            //  generate_exprPropId(stmt->getTargetPropId());
           // main.space();
            //main.pushstr_space(">");
           // main.pushString(strBuffer);
            //main.pushstr_newL(")");
            //main.pushstr_newL("{");
            list<ASTNode*>::iterator itr1;
            list<ASTNode*>::iterator itr2;
            itr2=rightList.begin();
            itr1=leftList.begin();
            itr1++;
            for( ;itr1!=leftList.end();itr1++)
            {   ASTNode* node=*itr1;
                ASTNode* node1=*itr2;
                
                if(node->getTypeofNode()==NODE_ID)
                    {
                      
                      main.pushstr_space(convertToCppType(((Identifier*)node)->getSymbolInfo()->getType()));
                      sprintf(strBuffer,"%s_new",((Identifier*)node)->getIdentifier());
                      main.pushString(strBuffer);
                      //printf("bbbbbbbbbbbbbbbbbbbb=%s",strBuffer);
                      main.pushString(" = ");
                      generateExpr((Expression*)node1);
                      
                    } 
                    if(node->getTypeofNode()==NODE_PROPACCESS)
                    {
                      /*
                      PropAccess* p=(PropAccess*)node;
                      Type* type=p->getIdentifier2()->getSymbolInfo()->getType();
                      if(type->isPropType())
                      {
                        main.pushstr_space(convertToCppType(type->getInnerTargetType()));
                      }
                      
                      sprintf(strBuffer,"%s_new",p->getIdentifier2()->getIdentifier());
                      main.pushString(strBuffer);
                      main.pushString(" = ");
                      Expression* expr=(Expression*)node1;
                      generateExpr((Expression*)node1);
                      main.pushstr_newL(";");
                      */
                    }
                    itr2++;
            }
          if(stmt->isTargetId())
          { 
             Identifier* p=stmt->getTargetId();
             //sprintf(strBuffer,"omp_set_lock(&(lock[%s])) ;",stmt->getTargetId()->getIdentifier());
          }
          if(stmt->isTargetPropId())
          {
            //sprintf(strBuffer,"omp_set_lock(&(lock[%s])) ;",stmt->getTargetPropId()->getIdentifier1()->getIdentifier());
            
          }
           // main.pushstr_newL(strBuffer);

             main.pushString("if (");
             main.pushString("gpu_");
             //generate_exprPropId(stmt->getTargetPropId());
             main.pushString("dist[id] != MAX_VAL");
             main.space();
             main.pushString("&&");
             main.space();
            
            if(stmt->isTargetId())
            ;//generate_exprIdentifier(stmt->getTargetId());
            else{
              
             // main.pushString("gpu_");
              generate_exprPropId(stmt->getTargetPropId());

            }
              
            main.space();
            main.pushstr_space(">");
            
            generate_exprIdentifier(stmt->getAssignedId());
            main.pushString("_new");
            main.pushstr_newL(")");
            main.pushstr_newL("{");
            generateAtomicBlock();
           
            
            /*
            itr1=leftList.begin();
            i=0;
            for( ;itr1!=leftList.end();itr1++)
            {   ASTNode* node=*itr1;

              if(node->getTypeofNode()==NODE_ID)
                    {
                        generate_exprIdentifier((Identifier*)node);
                    }
               if(node->getTypeofNode()==NODE_PROPACCESS)
                {
                  generate_exprPropId((PropAccess*)node);
                } 
                main.space();
                main.pushstr_space("=");
                if(node->getTypeofNode()==NODE_ID)
                    {
                        generate_exprIdentifier((Identifier*)node);
                    }
               if(node->getTypeofNode()==NODE_PROPACCESS)
                {
                  generate_exprIdentifier(((PropAccess*)node)->getIdentifier2());
                } 
                main.pushString("_new");
                main.pushstr_newL(";");    

            }
            */
           // main.pushstr_newL("}");
            if(stmt->isTargetId())
             { 
             Identifier* p=stmt->getTargetId();
            
              //sprintf(strBuffer,"omp_unset_lock(&(lock[%s]));",stmt->getTargetId()->getIdentifier());
             }
            if(stmt->isTargetPropId())
             {
               //sprintf(strBuffer,"omp_unset_lock(&(lock[%s]));",stmt->getTargetPropId()->getIdentifier1()->getIdentifier());
             }
              //main.pushstr_newL(strBuffer);
             // main.pushstr_newL("}");
      }
  }

}

void dsl_cpp_generator::generateIfStmt(ifStmt* ifstmt)
{ cout<<"INSIDE IF STMT"<<"\n";
  Expression* condition=ifstmt->getCondition();
  main.pushString("if (");
  cout<<"TYPE OF IFSTMT"<<condition->getTypeofExpr()<<"\n";
  //generateExpr(condition);
  main.pushString("gpu_modified_prev[id]");
  main.pushString(" )");
  generateStatement(ifstmt->getIfBody());
  if(ifstmt->getElseBody()==NULL)
     return;
  main.pushstr_newL("else");
  generateStatement(ifstmt->getElseBody());   
}

void dsl_cpp_generator::findTargetGraph(vector<Identifier*> graphTypes,Type* type)
{   
    if(graphTypes.size()>1)
    {
      cerr<<"TargetGraph can't match";
    }
    else
    { 
      
      Identifier* graphId=graphTypes[0];
     
      type->setTargetGraph(graphId);
    }
    
    
}

void dsl_cpp_generator::generateAssignmentStmt(assignment* asmt)
{  
   
   if(asmt->lhs_isIdentifier())
   { 
     Identifier* id=asmt->getId();
     main.pushString(id->getIdentifier());
   }
   else if(asmt->lhs_isProp())  //the check for node and edge property to be carried out.
   {
     PropAccess* propId=asmt->getPropId();
     if(asmt->getAtomicSignal())
      { 
        /*if(asmt->getParent()->getParent()->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS)
           if(asmt->getExpr()->isArithmetic()||asmt->getExpr()->isLogical())*/
            // main.pushstr_newL("#pragma omp atomic");
           
      }
     main.pushString(propId->getIdentifier2()->getIdentifier());
     main.push('[');
     main.pushString(propId->getIdentifier1()->getIdentifier());
     main.push(']');
     
   }

   main.pushString(" = ");
   generateExpr(asmt->getExpr());
   main.pushstr_newL(";");
}


void dsl_cpp_generator::generateProcCall(proc_callStmt* proc_callStmt)
{ // cout<<"INSIDE PROCCALL OF GENERATION"<<"\n";
   proc_callExpr* procedure=proc_callStmt->getProcCallExpr();
  // cout<<"FUNCTION NAME"<<procedure->getMethodId()->getIdentifier();
   string methodID(procedure->getMethodId()->getIdentifier());
   string IDCoded("attachNodeProperty");
   int x=methodID.compare(IDCoded);
   if(x==0)
       {  
         // cout<<"MADE IT TILL HERE";
          char strBuffer[1024];
          list<argument*> argList=procedure->getArgList();
          list<argument*>::iterator itr;
          //main.pushstr_newL("#pragma omp parallel for");
          sprintf(strBuffer,"for (%s %s = 0; %s < %s; %s ++) ","int","t","t","V","t");
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("{");
          for(itr=argList.begin();itr!=argList.end();itr++)
              { 
                assignment* assign=(*itr)->getAssignExpr();
                Identifier* lhsId=assign->getId();
                Expression* exprAssigned=assign->getExpr();
                sprintf(strBuffer,"%s[%s] = ",lhsId->getIdentifier(),"t");
                main.pushString(strBuffer);
                generateExpr(exprAssigned);

                main.pushstr_newL(";");
                
              }
             
        main.pushstr_newL("}");

       }
}

void dsl_cpp_generator::generatePropertyDefination(Type* type,char* Id)
{ 
  Type* targetType=type->getInnerTargetType();
  if(targetType->gettypeId()==TYPE_INT)
  {
     main.pushString("=");
     main.pushString(INTALLOCATION);
     main.pushString("[");
    // printf("%d SIZE OF VECTOR",)
    // findTargetGraph(graphId,type);
    
    if(graphId.size()>1)
    {
      cerr<<"TargetGraph can't match";
    }
    else
    { 
      
      Identifier* id=graphId[0];
     
      type->setTargetGraph(id);
    }
     char strBuffer[100];
     sprintf(strBuffer,"%s","V");
     main.pushString(strBuffer);
     main.pushString("]");
     main.pushstr_newL(";");
  }
  if(targetType->gettypeId()==TYPE_BOOL)
  {
     main.pushString("=");
     main.pushString(BOOLALLOCATION);
     main.pushString("[");
     //findTargetGraph(graphId,type);
     if(graphId.size()>1)
    {
      cerr<<"TargetGraph can't match";
    }
    else
    { 
      
      Identifier* id=graphId[0];
     
      type->setTargetGraph(id);
    }
     char strBuffer[100];
     sprintf(strBuffer,"%s","V");
     main.pushString(strBuffer);
     main.pushString("]");
     main.pushstr_newL(";");
  }

   if(targetType->gettypeId()==TYPE_FLOAT)
  {
     main.pushString("=");
     main.pushString(FLOATALLOCATION);
     main.pushString("[");
     //findTargetGraph(graphId,type);
     if(graphId.size()>1)
    {
      cerr<<"TargetGraph can't match";
    }
    else
    { 
      
      Identifier* id=graphId[0];
     
      type->setTargetGraph(id);
    }
     char strBuffer[100];
     sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     main.pushString(strBuffer);
     main.pushString("]");
     main.pushstr_newL(";");
  }


}

  void dsl_cpp_generator::getDefaultValueforTypes(int type)
  {
    switch(type)
    {
      case TYPE_INT:
      case TYPE_LONG:
          main.pushstr_space(" = 0");
          break;
      case TYPE_FLOAT:
      case TYPE_DOUBLE:
          main.pushstr_space(" = 0.0");
          break;
      case TYPE_BOOL:
          main.pushstr_space(" = false");
       default:
         assert(false);
         return;        
    }




  }

void dsl_cpp_generator::generateForAll_header()
{
  //main.pushstr_newL("#pragma omp parallel for"); //This needs to be changed to checked for 'For' inside a parallel section.

}

bool dsl_cpp_generator::allGraphIteration(char* methodId)
{
   string methodString(methodId);
   
   return (methodString=="nodes"||methodString=="edges");


}

bool dsl_cpp_generator::neighbourIteration(char* methodId)
{
  string methodString(methodId);
   return (methodString=="neighbors");
}

void dsl_cpp_generator::generateForAllSignature(forallStmt* forAll)
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
        //sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_nodes",iterator->getIdentifier());
      }
      else
        //sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_edges",iterator->getIdentifier());

      main.pushstr_newL(strBuffer);

    }
    else if(neighbourIteration(iteratorMethodId->getIdentifier()))
    { 
       
       char* graphId=sourceGraph->getIdentifier();
       char* methodId=iteratorMethodId->getIdentifier();
       list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
       sprintf(strBuffer,"for (%s %s = %s[%s]; %s < %s[%s+1]; %s ++) ","int","edge","gpu_OA","id","edge","gpu_OA","id","edge");
       main.pushstr_newL(strBuffer);
       main.pushString("{");
       main.NewLine();
       sprintf(strBuffer,"%s %s = %s[%s] ;","int",iterator->getIdentifier(),"gpu_edgeList","edge"); //needs to move the addition of
       main.pushstr_newL(strBuffer);           //statement to a different method.

    }
  }

}

blockStatement* dsl_cpp_generator::includeIfToBlock(forallStmt* forAll)
{ 
   
  Expression* filterExpr=forAll->getfilterExpr();
  statement* body=forAll->getBody();
  if(filterExpr->getExpressionFamily()==EXPR_RELATIONAL)
  {
    Expression* expr1=filterExpr->getLeft();
    Expression* expr2=filterExpr->getRight();
    if(expr1->isPropIdExpr()&&expr2->isBooleanLiteral()) //specific to sssp. Need to revisit again to change it.
    {   PropAccess* propId=expr1->getPropId();
        bool value=expr2->getBooleanConstant();
        Expression* exprBool=(Expression*)Util::createNodeForBval(!value);
        assignment* assign=(assignment*)Util::createAssignmentNode(propId,exprBool);
        if(body->getTypeofNode()==NODE_BLOCKSTMT)
        {
          blockStatement* newbody=(blockStatement*)body;
          newbody->addToFront(assign);
          body=newbody;
        }
    }

  }
  ifStmt* ifNode=(ifStmt*)Util::createNodeForIfStmt(filterExpr,forAll->getBody(),NULL);
  blockStatement* newBlock=new blockStatement();
  newBlock->setTypeofNode(NODE_BLOCKSTMT);
  newBlock->addStmtToBlock(ifNode);
  return newBlock;

  
}

void dsl_cpp_generator::generateCudaIndex()
{
  main.pushstr_newL("unsigned int id = threadIdx.x + (blockDim.x * blockIdx.x);");


}

void dsl_cpp_generator::generateForAll(forallStmt* forAll)
{ 
   proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
    Identifier* iteratorMethodId=extractElemFunc->getMethodId();
    statement* body=forAll->getBody();
  if(forAll->isForall())
  {
    generateForAll_header();
  }

  generateForAllSignature(forAll);

  if(forAll->hasFilterExpr())
  { 
    blockStatement* changedBody=includeIfToBlock(forAll);
   // cout<<"CHANGED BODY TYPE"<<(changedBody->getTypeofNode()==NODE_BLOCKSTMT);
    forAll->setBody(changedBody);
   // cout<<"FORALL BODY TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
  }
   
  if(neighbourIteration(iteratorMethodId->getIdentifier()))
  { 
   
    if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS)
     {   
        
         char strBuffer[1024];
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

     generateBlock((blockStatement*)body,false);
     if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS||forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRRBFS)
       main.pushstr_newL("}");
    main.pushstr_newL("}");
  }
  else
  { 
    cout<<"ENTER INSIDE FOR NORMAL ITER FORALL"<<"\n";
    cout<<iteratorMethodId->getIdentifier()<<"\n";
    generateStatement(forAll->getBody());
  }

} 
void dsl_cpp_generator:: generateVariableDeclForEdge(declaration* declStmt)
{
  
  Type* type=declStmt->getType();
    if(type->isNodeEdgeType())
        {
          main.pushstr_space(convertToCppType(type));
          main.pushString(declStmt->getdeclId()->getIdentifier());
          if(declStmt->isInitialized())
           {
              main.pushString(" = ");
             generateExpr(declStmt->getExpressionAssigned());
             main.pushstr_newL(";");
           }
        }

        //generateLocalInitForID(); //this needs to be changed later
}

void dsl_cpp_generator:: generateLocalInitForID()
{
  main.pushstr_newL("unsigned int v = id;");
  
}
void dsl_cpp_generator:: generateVariableDecl(declaration* declStmt)
{
   Type* type=declStmt->getType();
   //bool value=type->gettypeId()==TYPE_BOOL;
   
   if(type->isPropType())
   {   
     if(type->getInnerTargetType()->isPrimitiveType())
     { 
       Type* innerType=type->getInnerTargetType();
       //printf("%s CPP VALUE",convertToCppType(innerType));
       main.pushString(convertToCppType(innerType)); //convertToCppType need to be modified.
       main.pushString("*");
       main.space();
       main.pushString(declStmt->getdeclId()->getIdentifier());
       generatePropertyDefination(type,declStmt->getdeclId()->getIdentifier());
     }
   }
   /*
   else if(type->isPrimitiveType())
   { 
     main.pushstr_space(convertToCppType(type));
     main.pushString(declStmt->getdeclId()->getIdentifier());
     if(declStmt->isInitialized())
     {
       main.pushString(" = ");
       generateExpr(declStmt->getExpressionAssigned());
       main.pushstr_newL(";");
     }
     else
     {
        main.pushString(" = ");
        getDefaultValueforTypes(type->gettypeId());
        main.pushstr_newL(";");
     }

   }
   */
   else if(type->isNodeEdgeType())
        {
          main.pushstr_space(convertToCppType(type));
          main.pushString(declStmt->getdeclId()->getIdentifier());
          if(declStmt->isInitialized())
           {
              main.pushString(" = ");
             generateExpr(declStmt->getExpressionAssigned());
             main.pushstr_newL(";");
           }
        }
}

void dsl_cpp_generator::generate_exprLiteral(Expression* expr)
     {  
      
        char valBuffer[1024];
       
           int expr_valType=expr->getExpressionFamily();
          
           switch(expr_valType)
           { 
             case EXPR_INTCONSTANT:
                sprintf(valBuffer,"%ld",expr->getIntegerConstant());
                break;
            
             case EXPR_DOUBLECONSTANT:
                sprintf(valBuffer,"%lf",expr->getFloatConstant());
                break;
             case EXPR_BOOLCONSTANT:
                 sprintf(valBuffer,"%s",expr->getBooleanConstant()?"true":"false");
                 break;
             default:
              assert(false);
            
             // return valBuffer;       
           } 
           
           // printf("VALBUFFER %s",valBuffer);
            main.pushString(valBuffer); 
         
      

     }

 void dsl_cpp_generator::generate_exprInfinity(Expression* expr)
 {
              char valBuffer[1024];
              if(expr->getTypeofExpr()!=NULL)
                   {
                     int typeClass=expr->getTypeofExpr();
                     switch(typeClass)
                     {
                       case TYPE_INT:
                         sprintf(valBuffer,"%s",expr->isPositiveInfinity()?"INT_MAX":"INT_MIN");
                        break;
                       case TYPE_LONG:
                           sprintf(valBuffer,"%s",expr->isPositiveInfinity()?"LLONG_MAX":"LLONG_MIN");
                        break;
                       case TYPE_FLOAT:
                            sprintf(valBuffer,"%s",expr->isPositiveInfinity()?"FLT_MAX":"FLT_MIN");
                        break;  
                       case TYPE_DOUBLE:
                            sprintf(valBuffer,"%s",expr->isPositiveInfinity()?"DBL_MAX":"DBL_MIN");
                        break;
                        default:
                            sprintf(valBuffer,"%s",expr->isPositiveInfinity()?"INT_MAX":"INT_MIN");
                        break;


                      }
                          
                   }
                   else
                 
                   {
                 sprintf(valBuffer,"%s",expr->isPositiveInfinity()?"INT_MAX":"INT_MIN");
                   }    
                 
                 main.pushString(valBuffer);

    }    
  
  
  const char* dsl_cpp_generator::getOperatorString(int operatorId)
  {  
    switch(operatorId)
    {
      case OPERATOR_ADD:
       return "+";
      case OPERATOR_DIV:
       return "/";
      case OPERATOR_MUL:
       return "*";
      case OPERATOR_MOD:
       return "%";
      case OPERATOR_SUB:
       return "-";
      case OPERATOR_EQ:
       return "==";
      case OPERATOR_NE:
       return "!=";
      case OPERATOR_LT:
       return "<";
      case OPERATOR_LE:
       return "<=";
      case OPERATOR_GT:
      return ">";
      case OPERATOR_GE:
      return ">=";
      case OPERATOR_AND:
      return "&&";
      case OPERATOR_OR:
      return "||";
      default:
      return "NA";         
    }

    
  }

  void  dsl_cpp_generator::generate_exprRelational(Expression* expr)
  {
    cout<<"INSIDE RELATIONAL EXPR"<<"\n";
    
    generateExpr(expr->getLeft());
    
    main.space();
    const char* operatorString=getOperatorString(expr->getOperatorType());
    main.pushstr_space(operatorString);
    generateExpr(expr->getRight());
  }

void dsl_cpp_generator::generate_exprIdentifier(Identifier* id)
{
   main.pushString(id->getIdentifier());
}

void dsl_cpp_generator::generateExpr(Expression* expr)
{ 
  if(expr->isLiteral())
     { 
      // cout<<"INSIDE THIS FOR LITERAL"<<"\n";
       generate_exprLiteral(expr);
     }
     else if(expr->isInfinity())
       {
         generate_exprInfinity(expr);
       }

       else if(expr->isIdentifierExpr())
       {
         generate_exprIdentifier(expr->getId());
       }
       else if(expr->isPropIdExpr())
       {
         generate_exprPropId(expr->getPropId());
       }
       else if(expr->isArithmetic()||expr->isLogical())
       {
         generate_exprArL(expr);
       }
       else if(expr->isRelational())
       {
          generate_exprRelational(expr);
       }
       else if(expr->isProcCallExpr())
       {
         generate_exprProcCall(expr);
       }
       else 
       {
         assert(false);
       }



}

void dsl_cpp_generator::generate_exprArL(Expression* expr)
{
  //cout<<"INSIDE RELATIONAL EXPR"<<"\n";
    
    generateExpr(expr->getLeft());
    
    main.space();
    const char* operatorString=getOperatorString(expr->getOperatorType());
    main.pushstr_space(operatorString);
    
    generateExpr(expr->getRight());

}


void dsl_cpp_generator::generate_exprProcCall(Expression* expr)
{
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if(methodId=="get_edge")
  {
    main.pushString("edge"); //To be changed..need to check for a neighbour iteration 
                             // and then replace by the iterator.
  }
}

void dsl_cpp_generator::generate_exprPropId(PropAccess* propId) //This needs to be made more generic.
{ char strBuffer[1024];
  //PropAccess* propId=(PropAccess*)expr->getPropId();
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  if (id2->getIdentifier() == "dist") {
    printf("hhhhhhhhhhhhhhiiiiiiiiiiiiiiiiiii");
    sprintf(strBuffer,"gpu_%s[%s]",id2->getIdentifier(),id1->getIdentifier());
    main.pushString(strBuffer);
  }
  sprintf(strBuffer,"gpu_%s[%s]",id2->getIdentifier(),id1->getIdentifier());
  printf("ggggggggggg=%s",id2->getIdentifier());
  main.pushString(strBuffer);
}

void dsl_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct)
{ 
  
  char strBuffer[1024];
  Expression* convergeExpr=fixedPointConstruct->getDependentProp();
  Identifier* fixedPointId=fixedPointConstruct->getFixedPointId();
  statement* blockStmt=fixedPointConstruct->getBody();
  assert(convergeExpr->getExpressionFamily()==EXPR_UNARY||convergeExpr->getExpressionFamily()==EXPR_ID);
  /*
  main.pushString("while ( ");
  main.push('!');
  main.pushString(fixedPointId->getIdentifier());
  main.pushstr_newL(" )");
  main.pushstr_newL("{");
  */
  fixedpointVarforSSSP = fixedPointId->getIdentifier(); //handles SSSP fixed point var
  //cout << "nnnnnnnnnnnnnnnnnnnnn" <<fixedpointVarforSSSP;

  if(fixedPointConstruct->getBody()->getTypeofNode()!=NODE_BLOCKSTMT)
    generateStatement(fixedPointConstruct->getBody());
  else
    generateBlock((blockStatement*)fixedPointConstruct->getBody(),false);
    
  Identifier* dependentId=NULL;
  bool isNot=false;
  assert(convergeExpr->getExpressionFamily()==EXPR_UNARY||convergeExpr->getExpressionFamily()==EXPR_ID);
  if(convergeExpr->getExpressionFamily()==EXPR_UNARY)
  {
    if(convergeExpr->getUnaryExpr()->getExpressionFamily()==EXPR_ID)
    {
      dependentId=convergeExpr->getUnaryExpr()->getId();
      isNot=true;
    }
  }
  /*
  if(convergeExpr->getExpressionFamily()==EXPR_ID)
     dependentId=convergeExpr->getId();
     if(dependentId!=NULL)
     {
       if(dependentId->getSymbolInfo()->getType()->isPropType())
       {   
        sprintf(strBuffer,"bool %s_fp = false ;",dependentId->getIdentifier());
        main.pushstr_newL(strBuffer);
         if(dependentId->getSymbolInfo()->getType()->isPropNodeType())
         {  Type* type=dependentId->getSymbolInfo()->getType();
              main.pushString("#pragma omp parallel for");
           sprintf(strBuffer," reduction(||:%s_fp)",dependentId->getIdentifier());
           main.pushstr_newL(strBuffer);
           if(graphId.size()>1)
             {
               cerr<<"GRAPH AMBIGUILTY";
             }
             else
            sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int","v","v",graphId[0]->getIdentifier(),"num_nodes","v");
           main.pushstr_newL(strBuffer);
           sprintf(strBuffer,"%s_fp = %s_fp || %s[%s] ;",dependentId->getIdentifier(),dependentId->getIdentifier(),dependentId->getIdentifier(),"v");
           main.pushstr_newL(strBuffer);
           if(isNot)
           {
            sprintf(strBuffer,"%s = !%s_fp ;",fixedPointId->getIdentifier(),dependentId->getIdentifier());
            main.pushstr_newL(strBuffer);
             }
             else
             {
               sprintf(strBuffer,"%s = %s_fp ;",fixedPointId->getIdentifier(),dependentId->getIdentifier());
               main.pushString(strBuffer);
             }

        }
      }
     }
     */

     main.pushstr_newL("}");
}



void dsl_cpp_generator::generateBlock(blockStatement* blockStmt,bool includeBrace)
{  cout<<"INSIDE BLOCK OF NORMAL ITER"<<"\n";
   list<statement*> stmtList=blockStmt->returnStatements();
   list<statement*> ::iterator itr;
   if(includeBrace)
   {
   main.pushstr_newL("{");
   }
   for(itr=stmtList.begin();itr!=stmtList.end();itr++)
   {
     statement* stmt=*itr;
     generateStatement(stmt);

   }


   if(includeBrace)
   {
   main.pushstr_newL("}");
   }
}

//note :: this is specific to SSSP body for cudaMemCpy generation
void dsl_cpp_generator::generateCudaMemCpyForSSSPBody()
{
   char strBuffer[1024];
   sprintf(strBuffer,"%s (%s, %s, sizeof(int) *%s ,%s);","cudaMemcpy","gpu_OA","OA","(1+V)","cudaMemcpyHostToDevice");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s (%s, %s, sizeof(int) *%s ,%s);","cudaMemcpy","gpu_edgeList","edgeList","(E)","cudaMemcpyHostToDevice");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s (%s, %s, sizeof(int) *%s ,%s);","cudaMemcpy","gpu_edgeLen","cpu_edgeLen ","(E)","cudaMemcpyHostToDevice");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s (%s, %s, sizeof(int) *%s ,%s);","cudaMemcpy","gpu_dist","dist","(V)","cudaMemcpyHostToDevice");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s (%s, %s, sizeof(bool) *%s ,%s);","cudaMemcpy","gpu_modified_prev","modified ","(V)","cudaMemcpyHostToDevice");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s (%s, %s, sizeof(bool) *%s ,%s);","cudaMemcpy","gpu_finished","finished ","(1)","cudaMemcpyHostToDevice");
   main.pushstr_newL(strBuffer);

}


//note :: This doesn't include code generation for the fixed point contsruct. Fixed point-Sepeartedly handled
void dsl_cpp_generator::generateBlockForSSSPBody(blockStatement* blockStmt,bool includeBrace)
{  cout<<"INSIDE BLOCK OF NORMAL ITER"<<"\n";
   list<statement*> stmtList=blockStmt->returnStatements();
   list<statement*> ::iterator itr;
   if(includeBrace)
   {
   main.pushstr_newL("{");
   }
   for(itr=stmtList.begin();itr!=stmtList.end();itr++)
   {
     statement* stmt=*itr;
     generateStatementForSSSPBody(stmt);

   }

   if(includeBrace)
   {
   main.pushstr_newL("}");
   }
}

void dsl_cpp_generator::generateInitialization(){

  char strBuffer[1024];
   main.pushstr_newL("template <typename T>");
   main.pushstr_newL("__global__ void initKernel(unsigned V, T* init_array, T initVal)");
   main.pushstr_newL("{");
   main.pushstr_newL("unsigned id = threadIdx.x + blockDim.x * blockIdx.x;");
   main.pushstr_newL("if(id < V)");
   main.pushstr_newL("{");
   main.pushstr_newL("init_array[id]=initVal;");
   main.pushstr_newL("}");
   main.pushstr_newL("}");

}

void dsl_cpp_generator:: generateFuncCUDASizeAllocation()
{
  char strBuffer[1024];
  main.NewLine();
  sprintf(strBuffer,"%s %s %s;","unsigned","int","block_size");
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,"%s %s %s;","unsigned","int","num_blocks");
  main.pushstr_newL(strBuffer);
  main.pushstr_newL(" if(V <= 1024)");
  main.pushstr_newL(" {");
  sprintf(strBuffer,"%s = %s;","block_size ","V");
  main.pushstr_newL(strBuffer);
  sprintf(strBuffer,"%s = %s;","block_size ","1");
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("}");
  main.pushstr_newL("else");
  main.pushstr_newL("{");
  main.pushstr_newL("block_size = 1024;");
  main.pushstr_newL("num_blocks = ceil(((float)V) / block_size);");

  //main.pushstr_newL("}");

}

void dsl_cpp_generator:: generateFuncCudaMalloc()
{
   char strBuffer[1024];
   main.NewLine();
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_OA","int","(1+V)");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_edgeList","int","(E)");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_edgeLen","int","(E)");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_dist","int","(V)");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_modified_prev","bool","(V)");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_modified_next","bool","(V)");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s(&%s,sizeof(%s) *%s);","cudaMalloc","gpu_finished","bool","(1)");
   main.pushstr_newL(strBuffer);

}
void dsl_cpp_generator:: generateFuncSSSPBody()
{

   char strBuffer[1024];
   main.NewLine();
   main.pushstr_newL("void SSSP(int * OA , int * edgeList , int* cpu_edgeLen  , int src ,int V, int E )");
   main.pushstr_newL("{");
   main.pushstr_newL("int MAX_VAL = 2147483647 ;");
   sprintf(strBuffer,"%s * %s;","int","gpu_edgeList");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s * %s;","int","gpu_edgeLen");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s * %s;","int","gpu_dist");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s * %s;","int","gpu_OA");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s * %s;","bool","gpu_modified_prev");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s * %s;","bool","gpu_modified_next");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s * %s;","bool","gpu_finished");
   main.pushstr_newL(strBuffer);
   main.NewLine();
   generateFuncCudaMalloc();
   generateFuncCUDASizeAllocation(); 
   main.pushstr_newL("}");
   sprintf(strBuffer,"%s * %s = %s %s[1];","bool",fixedpointVarforSSSP ,"new","bool");
   main.pushstr_newL(strBuffer);
   sprintf(strBuffer,"%s[0] = %s;",fixedpointVarforSSSP ,"false");
   main.pushstr_newL(strBuffer);

   return;

}

void dsl_cpp_generator::generateFuncPrintingSSSPOutput()
{

  char strBuffer[1024];
  main.pushstr_newL("char *outputfilename = \"output.txt\";");
  main.pushstr_newL("FILE *outputfilepointer;");
  main.pushstr_newL("outputfilepointer = fopen(outputfilename, \"w\");");
  //main.pushstr_newL( "fprintf(outputfilepointer, \"%d\", gpu_time_use);");
  main.pushstr_newL( "for (int i = 0; i <V; i++)");
  main.pushstr_newL( "{");
  //sprintf(strBuffer,"%s * %s = %s %s[1];","bool",fixedpointVarforSSSP ,"new","bool");
 // main.pushstr_newL(strBuffer);
  main.pushstr_newL( "fprintf(outputfilepointer, \"%d  %d\\n\", i, dist[i]);");
  main.pushstr_newL( "}");
}


void dsl_cpp_generator::generateFuncVariableINITForSSSP()
{
  main.NewLine();
  main.pushstr_newL( "int k =0;");
  main.NewLine();

}
void dsl_cpp_generator::generateFuncTerminatingConditionForSSSP()
{
    char strBuffer[1024];
    char * initValForInitKernel = "false";
    main.pushString("while ( ");
    sprintf(strBuffer,"!%s[0]",fixedpointVarforSSSP);
    main.pushString(strBuffer);
    main.pushstr_newL(" )");
    main.pushstr_newL("{");

    sprintf(strBuffer,"%s[0] =%s;",fixedpointVarforSSSP, "true");
    main.pushString(strBuffer);
    main.pushstr_newL("");
    sprintf(strBuffer,"%s<%s><<<%s,%s>>>(%s, %s, %s);","initKernel","bool","1","1","1","gpu_finished","true");
    main.pushString(strBuffer);
    main.pushstr_newL("");

    main.pushstr_newL("Compute_SSSP_kernel<<<num_blocks , block_size>>>(gpu_OA,gpu_edgeList, gpu_edgeLen ,gpu_dist,src, V ,MAX_VAL , gpu_modified_prev, gpu_modified_next, gpu_finished);");
    main.pushstr_newL("cudaDeviceSynchronize();");
    sprintf(strBuffer,"%s<%s><<<%s,%s>>>(%s, %s, %s);","initKernel","bool","num_blocks","block_size","V","gpu_modified_prev",initValForInitKernel);
    main.pushString(strBuffer);
    main.pushstr_newL("");
    main.pushstr_newL("cudaDeviceSynchronize();");

    main.pushstr_newL("bool *tempModPtr  = gpu_modified_next;");
    main.pushstr_newL("gpu_modified_next = gpu_modified_prev;");
    main.pushstr_newL("gpu_modified_prev = tempModPtr;");
    
    main.pushstr_newL("++k;");
    
    main.pushstr_newL("if(k==V)");
    main.pushstr_newL("{");
    main.pushstr_newL("break;");
  
    main.pushstr_newL("}");
    main.pushstr_newL("}");
    
}
void dsl_cpp_generator::generateFunc(ASTNode* proc)
{ 
  
   generateInitialization();
   char strBuffer[1024];
   Function* func=(Function*)proc;
   generateFuncHeader(func,false);
   generateFuncHeader(func,true);
   main.pushstr_newL("{");
   generateCudaIndex();
   generateLocalInitForID();
   main.pushstr_newL( "if (id < V) ");
   
   generateBlock(func->getBlockStatement(),false);
   main.NewLine();
   main.push('}');
   
   generateFuncSSSPBody();
   //to generate the assgnments and declration including propNode
   generateBlockForSSSPBody(func->getBlockStatement(),false);
   //generate the cudaMemCpy
   generateCudaMemCpyForSSSPBody();
   generateFuncVariableINITForSSSP();
   
   //generate the while loop body (terminating condition)
    generateFuncTerminatingConditionForSSSP();

   main.NewLine();
   main.pushstr_newL("cudaMemcpy(dist,gpu_dist , sizeof(int) * (V), cudaMemcpyDeviceToHost);");
   //printing the output to file
   generateFuncPrintingSSSPOutput();
   main.NewLine();
   main.pushstr_newL("}");

} 

const char* dsl_cpp_generator:: convertToCppType(Type* type)
{
  if(type->isPrimitiveType())
  {
      int typeId=type->gettypeId();
      switch(typeId)
      {
        case TYPE_INT:
          return "int";
        case TYPE_BOOL:
          return "bool";
        case TYPE_LONG:
          return "long";
        case TYPE_FLOAT:
          return "float";
        case TYPE_DOUBLE:
          return "double";
        case TYPE_NODE:
          return "int";
        case TYPE_EDGE:
          return "int";   
        default:
         assert(false);          
      }
  }
  else if(type->isPropType())
  {
    Type* targetType=type->getInnerTargetType();
    if(targetType->isPrimitiveType())
    { 
      int typeId=targetType->gettypeId();
      cout<<"TYPEID IN CPP"<<typeId<<"\n";
      switch(typeId)
      {
        case TYPE_INT:
          return "int*";
        case TYPE_BOOL:
          return "bool*";
        case TYPE_LONG:
          return "long*";
        case TYPE_FLOAT:
          return "float*";
        case TYPE_DOUBLE:
          return "double*";
        default:
         assert(false);          
      }

    }
  }
  else if(type->isNodeEdgeType())
  {
     return "int"; //need to be modified.
      
  }
  else if(type->isGraphType())
  {
    return "graph";
  }

  return "NA";
}

void dsl_cpp_generator:: generateFuncHeader(Function* proc,bool isMainFile)
{ 
  dslCodePad &targetFile = isMainFile ? main : header;

  if (isMainFile)
  {
    targetFile.pushString("__global__ void");
    targetFile.push(' ');
  }

  targetFile.pushString(proc->getIdentifier()->getIdentifier());
  if(isMainFile) {
  targetFile.pushString("_kernel");
  }
  
  targetFile.push('(');
  
  int maximum_arginline=4;
  int arg_currNo=0;
  int argumentTotal=proc->getParamList().size();
  list<formalParam*> paramList=proc->getParamList();
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
   if(!isMainFile)
       targetFile.pushString(";");
    targetFile.NewLine();
    return;  

        
}


bool dsl_cpp_generator::openFileforOutput()
{ 

  char temp[1024];
  printf("fileName %s\n",fileName);
  sprintf(temp,"%s/%s.h","../graphcode/generated_cuda",fileName);
  headerFile=fopen(temp,"w");
  if(headerFile==NULL)
     return false;
  header.setOutputFile(headerFile);

  sprintf(temp,"%s/%s.cu","../graphcode/generated_cuda",fileName);
  bodyFile=fopen(temp,"w"); 
  if(bodyFile==NULL)
     return false;
  main.setOutputFile(bodyFile);     
  
  return true;
}
void dsl_cpp_generator::generation_end()
  {
     header.NewLine();
     header.pushstr_newL("#endif");   
  }

 void dsl_cpp_generator::closeOutputFile()
 { 
   if(headerFile!=NULL)
   {
     header.outputToFile();
     fclose(headerFile);

   }
   headerFile=NULL;

   if(bodyFile!=NULL)
   {
     main.outputToFile();
     fclose(bodyFile);
   }

   bodyFile=NULL;

 } 

bool dsl_cpp_generator::generate()
{  

      
  // cout<<"FRONTEND VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();    //openFileforOutput();
   if(!openFileforOutput())
      return false;
   generation_begin(); 
   
   list<Function*> funcList=frontEndContext.getFuncList();
   for(Function* func:funcList)
   {
       generateFunc(func);
   }
   

   generation_end();

   closeOutputFile();

   return true;

}


  void dsl_cpp_generator::setFileName(char* f) // to be changed to make it more universal.
  {

    char *token = strtok(f, "/");
	  char* prevtoken;
   
   
    while (token != NULL)
    {   
		prevtoken=token;
    token = strtok(NULL, "/");
    }
    fileName=prevtoken;

  }
