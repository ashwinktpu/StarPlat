#include "mpi_cpp_generator.h"
#include<string.h>
#include<cassert>
int count = 0;
bool is_fixedPoint = false;
int num_messages = 0;


void mpi_cpp_generator::addIncludeToFile(char* includeName,dslCodePad& file,bool isCppLib)
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

void mpi_cpp_generator::generation_begin()
{ 
  char temp[1024];  
  header.pushString("#ifndef GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#define GENCPP_");
   header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushstr_space("#include");
  addIncludeToFile("iostream",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("cstdlib",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("climits",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("cstdbool",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("fstream",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("sys/time.h",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("boost/mpi.hpp",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("vector",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("map",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("bits/stdc++.h",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("boost/serialization/map.hpp",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("boost/serialization/vector.hpp",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("boost/mpi/collectives.hpp",header,true);
  header.pushstr_space("#include");
  addIncludeToFile("../graph.hpp",header,false);

  header.NewLine();
  header.pushstr_newL("using namespace std;");
  header.pushstr_newL("namespace mpi = boost::mpi;");
  header.NewLine();
  main.pushString("#include");
  sprintf(temp,"%s.h",fileName);
  addIncludeToFile(temp,main,false);
  main.NewLine();

}

void generateIsFinished(dslCodePad* header)
{
  header->pushstr_newL("bool is_finished (int startv,int endv,vector <int> modified)");
  header->pushstr_newL("{");
  header->pushstr_newL("int sum = 0,res = 0;");
  header->pushstr_newL("res = modified.size();");
  header->pushstr_newL("MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);");
  header->pushstr_newL("if(sum > 0) return true;");
  header->pushstr_newL("else return false;");
  header->pushstr_newL("}");
}

void add_InitialDeclarations(dslCodePad* main,iterateBFS* bfsAbstraction)
{
   
  char strBuffer[1024];
  char* graphId=bfsAbstraction->getGraphCandidate()->getIdentifier();
  Identifier* root = bfsAbstraction->getRootNode();
  /*
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
  main->pushstr_newL("levelCount[phase] = bfsCount;");*/
  main->pushstr_newL("int phase = 0 ;");
  main->pushstr_newL("vector <int> active;");
  main->pushstr_newL("vector<int> active_next;");
  sprintf(strBuffer,"vector <vector<int>> p (%s.num_nodes());",graphId);
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"int* d = new int[%s.num_nodes()];",graphId);
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"for (int t = 0; t < %s.num_nodes(); t++)",graphId);
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("{");
  main->pushstr_newL("d[t] = -1;");
  main->pushstr_newL("}");
  sprintf(strBuffer,"active.push_back(%s);",root->getIdentifier());
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"d[%s] = 0;",root->getIdentifier());
  main->pushstr_newL(strBuffer);

}

 void add_BFSIterationLoop(dslCodePad* main, dslCodePad* header, iterateBFS* bfsAbstraction)
 {
   
    char strBuffer[1024];
    char* iterNode=bfsAbstraction->getIteratorNode()->getIdentifier();
    char* graphId=bfsAbstraction->getGraphCandidate()->getIdentifier();
    /*
    main->pushstr_newL("while ( bfsCount > 0 )");
    main->pushstr_newL("{");
    main->pushstr_newL(" int prev_count = bfsCount ;");
    main->pushstr_newL("bfsCount = 0 ;");
    main->pushstr_newL("#pragma omp parallel for");
    sprintf(strBuffer,"for( %s %s = %s; %s < prev_count ; %s++)","int","l","0","l","l");
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
     sprintf(strBuffer,"dnbr = %s(&bfsDist[nbr],-1,bfsDist[%s]+1);","__sync_val_compare_and_swap",iterNode);
     main->pushstr_newL(strBuffer);
     main->pushstr_newL("if (dnbr < 0)");
     main->pushstr_newL("{");
     sprintf(strBuffer,"int %s = bfsCount.fetch_add(%s,%s) ;","loc","1","std::memory_order_relaxed");
     main->pushstr_newL(strBuffer);
     sprintf(strBuffer," levelNodes_later[%s]=nbr ;","loc");
     main->pushstr_newL(strBuffer);"
     main->pushstr_newL("}");
     main->pushstr_newL("}");
     */
    main->pushstr_newL("while(is_finished(startv,endv,active))");
    generateIsFinished(header);
    main->pushstr_newL("{");
      main->pushstr_newL("vector < vector <float> > send_data(np);");
      main->pushstr_newL("vector < vector <float> > receive_data(np);");
      main->pushstr_newL("int dest_pro;");
      main->pushstr_newL("while (active.size() > 0)");
      main->pushstr_newL("{");
        sprintf(strBuffer,"int %s = active.back();",iterNode);
        main->pushstr_newL(strBuffer);
        main->pushstr_newL("active.pop_back();");
          main->pushstr_newL("if(v >=startv && v<= endv)");
          main->pushstr_newL("{");

  
  }

  void add_RBFSIterationLoop(dslCodePad* main, iterateBFS* bfsAbstraction,Identifier* graphID)
  {
   
    char strBuffer[1024];    
    char* iterNode=bfsAbstraction->getIteratorNode()->getIdentifier();
    sprintf(strBuffer,"bool* modified = new bool[%s.num_nodes()];",graphID->getIdentifier());
    main->pushstr_newL(strBuffer);
    sprintf(strBuffer,"for (int t = 0; t < %s.num_nodes(); t++)",graphID->getIdentifier());
    main->pushstr_newL(strBuffer);
    main->pushstr_newL("{");
    main->pushstr_newL("modified[t] = false;");
    main->pushstr_newL("}");
    main->pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
    main->pushstr_newL("while (phase > 0)") ;
    main->pushstr_newL("{");
    //main->pushstr_newL("#pragma omp parallel for");
    main->pushstr_newL("vector <vector <float> > send_data(np);");
    main->pushstr_newL("vector <vector <float> > receive_data(np);");
    main->pushstr_newL("int dest_pro;");
    //sprintf(strBuffer,"for( %s %s = %s; %s < levelCount[phase] ; %s++)","int","l","0","l","l"); 
    //main->pushstr_newL(strBuffer);


  }

 void mpi_cpp_generator::generateBFSAbstraction(iterateBFS* bfsAbstraction)
 {
   char strBuffer[1024];
   add_InitialDeclarations(&main,bfsAbstraction);
   Identifier* iterNode=bfsAbstraction->getIteratorNode();
  //printf("BFS ON GRAPH %s",bfsAbstraction->getGraphCandidate()->getIdentifier());
  Identifier* graphID = bfsAbstraction->getGraphCandidate();
   add_BFSIterationLoop(&main,&header, bfsAbstraction);
   statement* body=bfsAbstraction->getBody();
   assert(body->getTypeofNode()==NODE_BLOCKSTMT);
   blockStatement* block=(blockStatement*)body;
   list<statement*> statementList=block->returnStatements();
   for(statement* stmt:statementList)
   {
       generateStatement(stmt);
   }
   main.pushstr_newL("}");
    main.pushstr_newL("}");

       //Receive part
    generate_sendCall(body);
    generate_receiveCallBFS(body,2,NULL);
   main.pushstr_newL("active.swap(active_next);");
   main.pushstr_newL("active_next.clear();");
   main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
   main.pushstr_newL("send_data.clear();");
   main.pushstr_newL("receive_data.clear();");
   main.pushstr_newL("phase = phase + 1 ;");
   //main.pushstr_newL("levelCount[phase] = bfsCount ;");
   //main.pushstr_newL(" levelNodes[phase].assign(levelNodes_later.begin(),levelNodes_later.begin()+bfsCount);");
   main.pushstr_newL("}");
   
   main.pushstr_newL("phase = phase -1 ;");



   add_RBFSIterationLoop(&main,bfsAbstraction,graphID);
   blockStatement* revBlock=(blockStatement*)bfsAbstraction->getRBFS()->getBody();
   list<statement*> revStmtList=revBlock->returnStatements();
   Expression* filter = bfsAbstraction->getRBFS()->getBFSFilter();

  // main.pushstr_newL("}");    //if
  // main.pushstr_newL("}");    //for

    for(statement* stmt:revStmtList)
    {
      if(stmt->getTypeofNode() != NODE_FORALLSTMT)
      {
        cout<<"Not forall\n";
        
        if(bfsAbstraction->getRBFS()->hasFilter())
        {
            cout<<"rbfs has filter\n";
            sprintf(strBuffer,"for (int %s=startv;%s<=endv;%s++)",iterNode->getIdentifier(),iterNode->getIdentifier(),iterNode->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
              main.pushstr_space("if(");
                generateExpr(filter);
              sprintf(strBuffer," && modified[%s] == true)",iterNode->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"modified[%s] = false;",iterNode->getIdentifier());
                main.pushstr_newL(strBuffer);
                generateStatement(stmt);
              main.pushstr_newL("}");
            main.pushstr_newL("}");
        }
        else
        {
          cout<<"rbfs has no filter\n";
            sprintf(strBuffer,"for (int %s=startv;%s<=endv;%s++)",iterNode->getIdentifier(),iterNode->getIdentifier(),iterNode->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
              sprintf(strBuffer,"if( modified[%s] == true )",iterNode->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"modified[%s] = false;",iterNode->getIdentifier());
                main.pushstr_newL(strBuffer);
                generateStatement(stmt);
              main.pushstr_newL("}");
            main.pushstr_newL("}");
        }
      }
      else
       generateStatement(stmt);
    }
   
   //generate_sendCall(bfsAbstraction->getRBFS()->getBody());
   //generate_receiveCallBFS(bfsAbstraction->getRBFS()->getBody(),3,iterNode);
   main.pushstr_newL("phase--;");
   main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
   main.pushstr_newL("send_data.clear();");
   main.pushstr_newL("receive_data.clear();");
   main.pushstr_newL("}"); //outer while


 }


void mpi_cpp_generator::generateStatement(statement* stmt)
{  
   if(stmt->getTypeofNode()==NODE_BLOCKSTMT)
     {
       generateBlock((blockStatement*)stmt);

     }
   if(stmt->getTypeofNode()==NODE_DECL)
   {
      cout<<"calling var decl"<<endl;
      generateVariableDecl((declaration*)stmt);

   } 
   if(stmt->getTypeofNode()==NODE_ASSIGN)
     { 
       cout<<"calling node assign"<<endl;
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
       cout<<"calling forall"<<endl;
       forallStmt* f = (forallStmt*) stmt;
       if(f->isForall())
          generateForAll((forallStmt*) stmt);
       else
          generateFor((forallStmt*) stmt);
     }
  
    if(stmt->getTypeofNode()==NODE_FIXEDPTSTMT)
    {
      cout<<"calling fixed point"<<endl;
      generateFixedPoint((fixedPointStmt*)stmt);
    }
    if(stmt->getTypeofNode()==NODE_REDUCTIONCALLSTMT)
    { cout<<"IS REDUCTION STMT HI"<<"\n";
      generateReductionStmt((reductionCallStmt*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_ITRBFS)
    {
      cout<<"calling iterate in bfs"<<endl;
      generateBFSAbstraction((iterateBFS*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_PROCCALLSTMT)
    { 
      cout<<"calling iteraterbfs"<<endl;
      generateProcCall((proc_callStmt*) stmt);
    }


}

void mpi_cpp_generator::generateReductionStmt(reductionCallStmt* stmt)
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
            list<argument*>::iterator argItr;
             argItr=argList.begin();
             argItr++; 
            main.pushString(" = ");
            generateExpr((*argItr)->getExpr());
            main.pushstr_newL(";");
            
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
                      main.pushString(" = ");
                      generateExpr((Expression*)node1);
                    } 
                    if(node->getTypeofNode()==NODE_PROPACCESS)
                    {
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
                    }
                    itr2++;
            }
         
             main.pushString("if (");
            if(stmt->isTargetId())
            generate_exprIdentifier(stmt->getTargetId());
            else
              generate_exprPropId(stmt->getTargetPropId());
            main.space();
            main.pushstr_space(">");
            //generateExpr((*argItr)->getExpr())
            generate_exprIdentifier(stmt->getAssignedId());            
            main.pushString("_new");
            main.pushstr_newL(")");
            main.pushstr_newL("{");
            
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
            main.pushstr_newL("}");

             // main.pushstr_newL("}");
          

      }
  }

}

void mpi_cpp_generator::generateInnerReductionStmt(reductionCallStmt* stmt)
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
   
        list<ASTNode*> rightList=stmt->getRightList();
        printf("LEFT LIST SIZE %d \n",leftList.size());
      
            //main.space();
            if(stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType())
            { Type* type=stmt->getAssignedId()->getSymbolInfo()->getType();
              
              main.pushstr_space(convertToCppType(type->getInnerTargetType()));
            }
            cout<<"INSIDE ARG ID"<<stmt->getAssignedId()->getSymbolInfo()->getType()->gettypeId()<<"\n";

          //  
            sprintf(strBuffer,"%s_new",stmt->getAssignedId()->getIdentifier());
            main.pushString(strBuffer);
            list<argument*>::iterator argItr;
             argItr=argList.begin();
             argItr++; 
            main.pushString(" = ");
            //generateExpr((*argItr)->getExpr());
            /*
            Expression* e = (*argItr)->getExpr();
            if(e->isPropIdExpr())
              generate_exprPropIdReceive(e->getPropId());
            else if(e->isArithmetic())
              generate_exprArLReceive(e);
            else
              generateExpr((*argItr)->getExpr());
              */
            main.pushstr_newL("x.second;");
            
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
                      main.pushString(" = ");
                      if(node1->getTypeofNode()==NODE_PROPACCESS)
                        generate_exprPropIdReceive(((Expression*)node1)->getPropId());
                      else 
                        generateExpr((Expression*)node1);
                    } 
                    if(node->getTypeofNode()==NODE_PROPACCESS)
                    {
                      PropAccess* p=(PropAccess*)node;
                      Type* type=p->getIdentifier2()->getSymbolInfo()->getType();
                      if(type->isPropType())
                      {
                        main.pushstr_space(convertToCppType(type->getInnerTargetType()));
                      }
                      
                      sprintf(strBuffer,"%s_new",p->getIdentifier2()->getIdentifier());
                      main.pushString(strBuffer);
                      main.pushString(" = ");
                      if(node1->getTypeofNode()==NODE_PROPACCESS)
                        generate_exprPropIdReceive(((Expression*)node1)->getPropId());
                      else 
                        generateExpr((Expression*)node1);
                      //Expression* expr=(Expression*)node1;
                      //generateExpr((Expression*)node1);
                      main.pushstr_newL(";");
                    }
                    itr2++;
            }
         
            if(stmt->isTargetId())
            {
              //generate_exprIdentifier(stmt->getTargetId());
              sprintf(strBuffer,"%s = x.first;",stmt->getTargetId()->getIdentifier());
              main.pushstr_newL(strBuffer);
            }
            else
            {
              //generate_exprPropId(stmt->getTargetPropId());
              sprintf(strBuffer,"int %s = x.first;",stmt->getTargetPropId()->getIdentifier1()->getIdentifier());
              main.pushstr_newL(strBuffer);
            }

            main.pushString("if (");
            if(stmt->isTargetId())
              generate_exprIdentifier(stmt->getTargetId());
            else
              generate_exprPropId(stmt->getTargetPropId());
            main.space();
            main.pushstr_space(">");
            //generateExpr((*argItr)->getExpr())
            generate_exprIdentifier(stmt->getAssignedId());            
            main.pushString("_new");
            main.pushstr_newL(")");
            main.pushstr_newL("{");
            
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
            main.pushstr_newL("}");

              //main.pushstr_newL("}");
          

      }
  }

}

void mpi_cpp_generator::generateReductionStmtForSend(reductionCallStmt* stmt,bool send)
{ char strBuffer[1024];
  reductionCall* reduceCall=stmt->getReducCall();
  if(reduceCall->getReductionType()==REDUCE_MIN)
  {
    if(send)
    {
        if(stmt->isListInvolved())
          {
            cout<<"INSIDE THIS OF LIST PRESENT"<<"\n";
        list<argument*> argList=reduceCall->getargList();
        list<ASTNode*>  leftList=stmt->getLeftList();
        int i=0;
   
        list<ASTNode*> rightList=stmt->getRightList();
        printf("LEFT LIST SIZE %d \n",leftList.size());
         
            list<ASTNode*>::iterator itr1;
            list<ASTNode*>::iterator itr2;
            list<argument*>::iterator argItr;
            
            itr2=rightList.begin();
            itr1=leftList.begin();
            argItr=argList.begin();
            
           Identifier* id;
           Expression* to_be_updated;
           main.pushstr_newL("if (itr != send_data[dest_pro].end())");
           main.pushstr_space("itr->second = min(");
           main.insert_indent();
            // main.pushString("if (");
            if(stmt->isTargetId())
            {
              //generate_exprIdentifier(stmt->getTargetId());
              id = stmt->getTargetId();
            }
            else
            {
              //generate_exprPropId(stmt->getTargetPropId());
              PropAccess* p1 = stmt->getTargetPropId();
              string p1id1(p1->getIdentifier1()->getIdentifier());
              string p1id2(p1->getIdentifier2()->getIdentifier());
              
              id = p1->getIdentifier1();
              Identifier* i2 = p1->getIdentifier2();
              for( ;argItr!=argList.end();argItr++)
              {
                PropAccess* p2 = NULL;
                Expression* e = (*argItr)->getExpr();
                if(e->isPropIdExpr()) 
                {               
                  p2 = e->getPropId();

                  string p2id1(p2->getIdentifier1()->getIdentifier());
                  string p2id2(p2->getIdentifier2()->getIdentifier());
                  if(p1id1.compare(p2id1)==0 && p1id2.compare(p2id2)==0)
                  {
                    cout<<"both ids match"<<endl;
                    sprintf(strBuffer,"send_data[dest_pro][%s]",id->getIdentifier());
                    main.pushString(strBuffer);
                  }
                }
                else
                {
                  to_be_updated = (*argItr)->getExpr();
                  generateExpr((*argItr)->getExpr());
                }
                ++i;
                if(i!=argList.size())
                  main.pushstr_space(",");

              }
            }
            main.pushstr_newL(");");
            main.decrease_indent();

            //Else part of send
            main.pushstr_newL("else");
            main.insert_indent();
            sprintf(strBuffer,"send_data[dest_pro][%s] = ",id->getIdentifier());
            main.pushString(strBuffer);
            generateExpr(to_be_updated);
            main.pushstr_newL(";");
            main.decrease_indent();
            
            }
      }
      else
      {
          if(stmt->isListInvolved())
          {
            cout<<"INSIDE THIS OF LIST PRESENT"<<"\n";
            list<argument*> argList=reduceCall->getargList();
            list<ASTNode*>  leftList=stmt->getLeftList();
            int i=0;
          
            list<ASTNode*> rightList=stmt->getRightList();
            printf("LEFT LIST SIZE %d \n",leftList.size());
              
                list<argument*>::iterator argItr;
                argItr=argList.begin();
                argItr++; 
                
                
                Expression* expr = (*argItr)->getExpr();
                if(expr->isArithmetic())
                {
                  cout<<"The exprression in reduction is arithmetic ***"<<endl;
                  Expression *left = expr->getLeft();
                  Expression *right = expr ->getRight();
                  if(left->isPropIdExpr())
                  {
                    main.pushstr_space("int");
                    generate_exprPropIdReceive(left->getPropId());
                    main.pushstr_newL(" = recv_data[(t*3*max_degree)+k];");
                    //generateExpr(left);
                    //main.pushstr_newL(";");
                    
                  }
                  if(right->isPropIdExpr())
                  {
                    main.pushstr_space("int");
                    generate_exprPropIdReceive(right->getPropId());
                    main.pushstr_newL(" = recv_data[(t*3*max_degree)+k+1];");
                    //generateExpr(right);                    
                    //main.pushstr_newL(";");
                  }
                }

                Identifier *id;
                if(stmt->isTargetId())
                  generate_exprIdentifier(stmt->getTargetId());
                else
                  id = (stmt->getTargetPropId())->getIdentifier1();
                  //generate_exprPropId(stmt->getTargetPropId());
                  main.pushstr_space("int");
                sprintf(strBuffer,"%s = recv_data[(t*3*max_degree)+k+2];",id->getIdentifier());
                main.pushstr_newL(strBuffer);
                //generateExpr((*argItr)->getExpr());
                //main.pushstr_newL(";");
            }
      }
  }

}

void mpi_cpp_generator::generateExprForSend(Expression* expr,int send, Identifier* remote,Identifier* replace)
{
  cout<<" reached generateExprForSend\n";
  if(expr->isArithmetic() || expr->isLogical())
  {
    cout<<"Expression is arithmetics...calling arl\n";
    generateArLForSend( expr,send,remote,replace);
  }
  else if(expr->isPropIdExpr())
  {
    cout<<"Expression is propid...calling propidgen\n";
    //expr->getPropId()->getIdentifier2()->getSymbolInfo()->getType();
    generatePropAccessForSend(expr->getPropId(),send,remote,replace);
  }
  else if(expr->isLiteral())
  {
    cout<<"Expression is literal...calling literalforsend\n";
    generate_exprLiteralForSend(expr,send,remote,replace);
  }
  else 
       {
         assert(false);
       }
}

void mpi_cpp_generator::generateAssignmentForSend(assignment* stmt,int send, Identifier* remote, Identifier* replace)
{
  cout<<"reaced generateAssignmentForSend\n";
  if(stmt->lhs_isProp())
  {
    PropAccess* lhs = stmt->getPropId();
    if(send==3)
      generatePropAccessForSend(lhs,send,remote,replace);
  }
  if (send == 3)
    main.pushstr_space(" =");
  Expression* rhs = stmt->getExpr();
  if(rhs->isPropIdExpr())
  {
    generatePropAccessForSend(rhs->getPropId(),send,remote,replace);
  }
  else
  {
    cout<<"Rhs of assignment is expr..calling generateExprForSend\n";
    generateExprForSend(rhs,send,remote,replace);
  }
    /*
  Expression* left = rhs->getLeft();
  if(left->isPropIdExpr())
  {
      generatePropAccessForSend(left->getPropId(),send,remote);
  }

  if (send == 3)
  {
    main.space();
    const char* operatorString=getOperatorString(rhs->getOperatorType());
    main.pushstr_space(operatorString);
  }

  Expression* right = rhs->getRight();
  if(right->isPropIdExpr())
  {
      generatePropAccessForSend(right->getPropId(),send,remote);
  }*/
    
  if (send == 3)
    main.pushstr_newL(";");
}

void mpi_cpp_generator::generatePropAccessForSend(PropAccess* stmt,int send, Identifier* remote,Identifier* replace)
{
  char strBuffer[1024];
  Identifier* id1 = stmt->getIdentifier1();
  Identifier* id2 = stmt->getIdentifier2();
  string i1(id1->getIdentifier());
  string i2(id2->getIdentifier());
  string rem(remote->getIdentifier());
  cout<<"Testing........................."<<i1 << rem <<endl;
  if(i1 == rem)
  {
    if(send == 1)
    {
      sprintf(strBuffer,"send_data[dest_pro].push_back(");
      main.pushString(strBuffer);
      generate_exprPropId(stmt);
      main.pushstr_newL(");");
      num_messages++;
    }
    else if(send == 3)
    {
      generate_exprPropIdReceive(stmt);
    }
    else
    {
      //Identifier* id2 = stmt->getIdentifier2(); 
      //PropAccess* newProp = new PropAccess();
      cout<<"send type is.sssssssssssssssssssssss 2"<<endl;
      if(replace !=NULL)
      //if(to_replace == true)
      {
         cout<<"reached here.........................replace not equal to null\n";
         //PropAccess* newProp = new PropAccess();
        //newProp = (PropAccess*)Util::createPropIdNode((ASTNode*)replace,(ASTNode*)id2);
        PropAccess* newProp;

        newProp=PropAccess::createPropAccessNode(replace,id2);
        //cout<<id2->getIdentifier() << endl;
        cout<<newProp->getIdentifier1()->getIdentifier() << endl;
        cout<<newProp->getIdentifier2()->getIdentifier() << endl;
        //TableEntry* t = id2->getSymbolInfo();
        Type* t = id2->getSymbolInfo()->getType()->getInnerTargetType();
        //TableEntry* t1 = newProp->getPropSymbT()->findEntryInST(id2);
        if(t == NULL)
          cout<<"t  NULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n";
       // if(t1 == NULL)
         // cout<<"t1  also NULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n";
        //Type* t = newProp->getIdentifier2()->getSymbolInfo()->getType()->getInnerTargetType();
        main.pushstr_space(convertToCppType(t));
        generate_exprPropIdReceive(newProp);
        sprintf(strBuffer," = receive_data[t][x+%d];",count);
        main.pushstr_newL(strBuffer);
        count++;
      }
      //((declaration*)id2)->getType();
      //stmt->getAccessType()
      //stmt->getPropSymbT()->getEntries()->g
      //Type* t = stmt->getPropSymbT()->findEntryInST(id2)->getType()->getInnerTargetType();
      //Type* type = id1->getSymbolInfo()->getType()->getInnerTargetType();
      //TableEntry* t = id2->getSymbolInfo();
      //cout<<t->getId()->getIdentifier();
      //// = convertToCppType(t); 
      //sprintf(strBuffer,"%s ",convertToCppType(t));
      //main.pushString(strBuffer);  
      else
      {  
        cout<<"reached here.........................replace  equal to null\n";
        Type* t = id2->getSymbolInfo()->getType()->getInnerTargetType();
        main.pushstr_space(convertToCppType(t));
        generate_exprPropIdReceive(stmt);
        sprintf(strBuffer," = receive_data[t][x+%d];",count);
        main.pushstr_newL(strBuffer);
        count++;
      }
      //main.pushstr_newL(");");
    }
  }
  else
  {
    if(send == 3)
      generate_exprPropId(stmt);
  }
}

void mpi_cpp_generator::generateArLForSend(Expression* stmt,int send, Identifier* remote,Identifier* replace)
{
  cout<<"Reached generateArLForSend\n";
  char strBuffer[1024];
  if(send == 3 )
  {
    if(stmt->hasEnclosedBrackets())
      main.pushstr_space("(");
  }
  Expression* left = stmt->getLeft();
  //if(left->isPropIdExpr())
  //{
    //  generatePropAccessForSend(left->getPropId(),send,remote);
  //}
  generateExprForSend(left,send,remote,replace);
  if (send == 3)
  {
    main.space();
    const char* operatorString=getOperatorString(stmt->getOperatorType());
    main.pushstr_space(operatorString);
  }
  Expression* right = stmt->getRight();
  //if(right->isPropIdExpr())
  //{
    //  generatePropAccessForSend(right->getPropId(),send,remote);
  //}
  //else
    generateExprForSend(right,send,remote,replace);
    if(send == 3)
    {
      if(stmt->hasEnclosedBrackets())
        main.pushstr_space(")");
    }
    cout<<"exiting generateArLForSend\n";
}

void mpi_cpp_generator::generate_exprLiteralForSend(Expression* expr,int send,Identifier *remote,Identifier* replace)
     {  
      
        char valBuffer[1024];
       
           int expr_valType=expr->getExpressionFamily();
          if(send ==3)
          {
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
      

     }

void mpi_cpp_generator::generateIfStmt(ifStmt* ifstmt)
{ cout<<"INSIDE IF STMT"<<"\n";
  Expression* condition=ifstmt->getCondition();
  main.pushString("if (");
  cout<<"TYPE OF IFSTMT"<<condition->getTypeofExpr()<<"\n";
  generateExpr(condition);
  main.pushString(" )");
  generateStatement(ifstmt->getIfBody());
  if(ifstmt->getElseBody()==NULL)
     return;
  main.pushstr_newL("else");
  generateStatement(ifstmt->getElseBody());   
}

void mpi_cpp_generator::findTargetGraph(vector<Identifier*> graphTypes,Type* type)
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

void mpi_cpp_generator::generateAssignmentStmt(assignment* asmt)
{  
   
   if(asmt->lhs_isIdentifier())
   { 
     Identifier* id=asmt->getId();
     main.pushString(id->getIdentifier());
   }
   else if(asmt->lhs_isProp())  //the check for node and edge property to be carried out.
   {
     PropAccess* propId=asmt->getPropId();
     
     //if(asmt->getAtomicSignal())
      //{ 
        /*if(asmt->getParent()->getParent()->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS)
           if(asmt->getExpr()->isArithmetic()||asmt->getExpr()->isLogical())*/
        //     main.pushstr_newL("#pragma omp atomic");
           
      //}
     main.pushString(propId->getIdentifier2()->getIdentifier());
     main.push('[');
     main.pushString(propId->getIdentifier1()->getIdentifier());
     main.push(']');
     
     
   }

   main.pushString(" = ");
   generateExpr(asmt->getExpr());
   main.pushstr_newL(";");


}


void mpi_cpp_generator::generateProcCall(proc_callStmt* proc_callStmt)
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
          sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int","t","t",procedure->getId1()->getIdentifier(),"num_nodes","t");
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

void mpi_cpp_generator::generatePropertyDefination(Type* type,char* Id)
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
     sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
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
     sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
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

  void mpi_cpp_generator::getDefaultValueforTypes(int type)
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

void mpi_cpp_generator::generateForAll_header()
{
  main.pushstr_newL("#pragma omp parallel for"); //This needs to be changed to checked for 'For' inside a parallel section.

}

bool mpi_cpp_generator::allGraphIteration(char* methodId)
{
   string methodString(methodId);
   
   return (methodString=="nodes"||methodString=="edges");


}

bool mpi_cpp_generator::neighbourIteration(char* methodId)
{
  string methodString(methodId);
   return (methodString=="neighbors");
}

bool mpi_cpp_generator::elementsIteration(char* extractId)
{
  string extractString(extractId);
  return (extractString=="elements");
}

void mpi_cpp_generator::generate_sendCall(statement* body)
{
    char strBuffer[1024];
    //sprintf(strBuffer,"MPI_Alltoall(send_data,3*max_degree,MPI_INT,recv_data,3*max_degree,MPI_INT,MPI_COMM_WORLD);");
    //main.pushstr_newL(strBuffer);
    main.pushstr_newL("all_to_all(world, send_data, receive_data);");

}
void mpi_cpp_generator::generate_receiveCall(statement* body)
{
    main.pushstr_newL("for(int t=0;t<np;t++)");
    main.pushstr_newL("{");
        main.pushstr_newL("if(t != my_rank)");
        main.pushstr_newL("{");
            main.pushstr_newL("for (auto x : receive_data[t])");
            main.pushstr_newL("{");
              //generate_addMessage((blockStatement*) body,false);
              generateReceiveBlock((blockStatement*)body);
              main.pushstr_newL("}");
            main.pushstr_newL("}");
        main.pushstr_newL("}");
}

void mpi_cpp_generator::generate_receiveCallBFS(statement* body,int send,Identifier* remote)
{
    //main.pushstr_newL("for(int t=0;t<np;t++)");
    //main.pushstr_newL("{");
      //  main.pushstr_newL("if(t != my_rank)");
        //main.pushstr_newL("{");
          //  main.pushstr_newL("for (int x=0; x < receive_data[t].size();x+=4)");
            //main.pushstr_newL("{");
              generate_addMessage(body,send,remote,NULL);
              //generateReceiveBlock((blockStatement*)body);
            //  main.pushstr_newL("}");
            //main.pushstr_newL("}");
        //main.pushstr_newL("}");
}

void mpi_cpp_generator::generateForAllSignature(forallStmt* forAll)
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
        sprintf(strBuffer,"for (%s %s = startv; %s <= endv; %s++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),iterator->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("{");
        
      }
      else
      {
        sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_edges",iterator->getIdentifier());
        main.pushstr_newL(strBuffer);
      }

    }
    else if(neighbourIteration(iteratorMethodId->getIdentifier()))
    { 
       
       char* graphId=sourceGraph->getIdentifier();
       char* methodId=iteratorMethodId->getIdentifier();
       list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
       sprintf(strBuffer,"for (%s %s = %s.%s[%s]; %s < %s.%s[%s+1]; %s ++) ","int","edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge");
       main.pushstr_newL(strBuffer);
       main.pushstr_newL("{");
       sprintf(strBuffer,"%s %s = %s.%s[%s] ;","int",iterator->getIdentifier(),graphId,"edgeList","edge"); //needs to move the addition of
       main.pushstr_newL(strBuffer);
       //statement to a different method.
    }
  }
  else if(forAll->isSourceField())
  {
    PropAccess* sourceField=forAll->getPropSource();
    Identifier* extractId=sourceField->getIdentifier2();
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

      }

  }




}

blockStatement* mpi_cpp_generator::includeIfToBlock(forallStmt* forAll)
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

 
void mpi_cpp_generator::generate_addMessage(statement* stmt,int send,Identifier* remote, Identifier* replace)
{
  char strBuffer[1024];
  if(stmt->getTypeofNode() == NODE_BLOCKSTMT)
  {
      list<statement*> stmtList=((blockStatement*)stmt)->returnStatements();
      list<statement*> ::iterator itr;
      for(itr=stmtList.begin();itr!=stmtList.end();itr++)
      {
        statement* stmts=*itr;
        generate_addMessage(stmts,send,remote,replace);
        //printf("CHECK IF INSIDE FOR ");//%d\n",stmt->getParent()->getParent()->getTypeofNode()==NODE_FORALLSTMT);
        //generateStatement(stmt);
        cout<< stmt->getTypeofNode() <<endl;
      }
    }
    else if(stmt->getTypeofNode()==NODE_REDUCTIONCALLSTMT)
    { cout<<"IS REDUCTION STMT HI............."<<"\n";
      generateReductionStmtForSend((reductionCallStmt*) stmt,send);    

    }
    
    else if (stmt->getTypeofNode() == NODE_ASSIGN)
    {
      cout<<"it is node assign\n";
      generateAssignmentForSend((assignment*)stmt,send, remote,replace);
    }
    else if(stmt->getTypeofNode() == NODE_EXPR)
      generateExprForSend((Expression*)stmt,send,remote,replace);

    else if (stmt->getTypeofNode() == NODE_FORALLSTMT)
    {
      forallStmt* forAll = ((forallStmt*)stmt);
      if(forAll->isSourceProcCall())
      {
        cout<<"Reached here test1\n";
        Identifier* sourceGraph=forAll->getSourceGraph();
        Identifier* iterator = forAll->getIterator();
        proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
          statement* body = forAll->getBody();
          list<argument*>  argList=extractElemFunc->getArgList();
              assert(argList.size()==1);
              Identifier* nodeNbr=argList.front()->getExpr()->getId();
              
            if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
            {
              cout<<"test here\n";
              main.pushstr_newL("for(int t=0;t<np;t++)");
              main.pushstr_newL("{");
                main.pushstr_newL("if(t != my_rank)");
                main.pushstr_newL("{");
                    sprintf(strBuffer,"for (int x=0; x < receive_data[t].size();x+=%d)",num_messages);
                    main.pushstr_newL(strBuffer);
                    num_messages = 0;
                    main.pushstr_newL("{");
              sprintf(strBuffer,"int d_%s = receive_data[t][x];",nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"int %s = receive_data[t][x+1];",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"int %s = receive_data[t][x+2];",nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              count = 3;
                generate_addMessage(/*(blockStatement*)*/body,2,nodeNbr,NULL);
              count = 0;
              sprintf(strBuffer,"if (d[%s] < 0 )",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"active_next.push_back(%s);",iterator->getIdentifier());
                main.pushstr_newL(strBuffer);
                sprintf(strBuffer,"d[%s] = d_%s + 1;",iterator->getIdentifier(),nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
              main.pushstr_newL("}");
              sprintf(strBuffer,"if (d[%s] == d_%s+1)",iterator->getIdentifier(),nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
             // main.insert_indent();
              main.pushstr_newL("{");
                sprintf(strBuffer,"p[%s].push_back(%s);",iterator->getIdentifier(),nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                generate_addMessage(/*(blockStatement*)*/ body,3,nodeNbr,NULL); 

                main.pushstr_newL("}");
                  main.pushstr_newL("}");
              main.pushstr_newL("}");

              main.pushstr_newL("}");
            }

            else if(stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
            {

              cout<<"test here rbfs add mes\n";
              //cout<<"TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT "<<
                  main.pushstr_newL("for(int t=0;t<np;t++)");
            main.pushstr_newL("{");
                main.pushstr_newL("if(t != my_rank)");
                main.pushstr_newL("{");
                    sprintf(strBuffer,"for (int x=0; x < receive_data[t].size();x+=%d)",num_messages);
                    main.pushstr_newL(strBuffer);
                    num_messages = 0;
                    main.pushstr_newL("{");
                      sprintf(strBuffer,"int %s = receive_data[t][x];",iterator->getIdentifier());
                      main.pushstr_newL(strBuffer);
                      sprintf(strBuffer,"int %s = receive_data[t][x+1];",nodeNbr->getIdentifier());
                      main.pushstr_newL(strBuffer);
                      count = 2;
              //sprintf(strBuffer,"int %s = receive_data[t][x+2];",nodeNbr->getIdentifier());
              //main.pushstr_newL(strBuffer);
                      //generate_addMessage(body,2,nodeNbr);
                      generate_addMessage(body,2,nodeNbr,iterator);
                      count = 0;
                      //generate_addMessage(body,3,nodeNbr); 
                      generate_addMessage(body,3,iterator,NULL); 
                      main.pushstr_newL("}");
                  main.pushstr_newL("}");
              main.pushstr_newL("}");
                /*
              sprintf(strBuffer,"if (d[%s] < 0 )",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"active_next.push_back(%s);",iterator->getIdentifier());
                main.pushstr_newL(strBuffer);
                sprintf(strBuffer,"d[%s] = d_%s + 1;",iterator->getIdentifier(),nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
              main.pushstr_newL("}");
              sprintf(strBuffer,"if (d[%s] == d_%s+1)",iterator->getIdentifier(),nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
              */
                  
              //main.pushstr_newL("}");
            }
              //generate_addMessage((blockStatement*)body,3,nodeNbr);
      }
    }
    else
      generateStatement(stmt);

   //}
}

void mpi_cpp_generator::generateReceiveBlock(blockStatement* body)
{
  list<statement*> stmtList=body->returnStatements();
  list<statement*> ::iterator itr;
  for(itr=stmtList.begin();itr!=stmtList.end();itr++)
   {
     statement* stmt=*itr;
     //printf("CHECK IF INSIDE FOR ");//%d\n",stmt->getParent()->getParent()->getTypeofNode()==NODE_FORALLSTMT);
     //generateStatement(stmt);
     if(stmt->getTypeofNode()==NODE_REDUCTIONCALLSTMT)
    { cout<<"IS REDUCTION STMT HI.............in receive block"<<"\n";
      generateInnerReductionStmt((reductionCallStmt*) stmt);          

    }
    if (stmt->getTypeofNode() == NODE_FORALLSTMT)
    {
      statement* b = ((forallStmt*)stmt)->getBody();
      generateReceiveBlock((blockStatement*) b);
    }

   }
}

void mpi_cpp_generator::generateForAll(forallStmt* forAll)
{ 
   proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
   PropAccess* sourceField=forAll->getPropSource();
   Identifier* iterator=forAll->getIterator();
   Identifier* extractId;
    if(sourceField!=NULL)
     extractId=sourceField->getIdentifier2();
    Identifier* iteratorMethodId;
    if(extractElemFunc!=NULL)
     iteratorMethodId=extractElemFunc->getMethodId();
    statement* body=forAll->getBody();
     char strBuffer[1024];
  
  //if(forAll->isForall())
  //{
    //generateForAll_header();
  //}

  generateForAllSignature(forAll);
  
  if(forAll->hasFilterExpr())
  { 

    //blockStatement* changedBody=includeIfToBlock(forAll);
   // cout<<"CHANGED BODY TYPE"<<(changedBody->getTypeofNode()==NODE_BLOCKSTMT);
    //forAll->setBody(changedBody);
   // cout<<"FORALL BODY TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
      Expression* filterExpr=forAll->getfilterExpr();
      statement* body=forAll->getBody();
      if(filterExpr->getExpressionFamily()==EXPR_RELATIONAL)
      {
        Expression* expr1=filterExpr->getLeft();
        Expression* expr2=filterExpr->getRight();
        main.pushstr_space("if (");
        generateExpr(expr1);
        sprintf(strBuffer," [%s]",iterator->getIdentifier());
        main.pushstr_space(strBuffer);
            const char* operatorString=getOperatorString(filterExpr->getOperatorType());
            main.pushstr_space(operatorString);
            //main.pushstr_space(" =");
            generateExpr(expr2);
        //generateExpr(filterExpr);
        main.pushstr_newL(" )");
        main.pushstr_newL("{");
        
        if(/*expr1->isPropIdExpr()&& */expr2->isBooleanLiteral()) //specific to sssp. Need to revisit again to change it.
        {   
            //PropAccess* propId=expr1->getPropId();
            bool value = expr2->getBooleanConstant();
            bool neg = !value;
            //Expression* exprBool=(Expression*)Util::createNodeForBval(!value);
            //assignment* assign=(assignment*)Util::createAssignmentNode(propId,exprBool);
            generateExpr(expr1);
            sprintf(strBuffer," [%s]",iterator->getIdentifier());
            main.pushstr_space(strBuffer);
            main.pushstr_space(" =");
            if(neg == true)
              main.pushString("true");
            else
              main.pushString("false");
            //generateExpr(expr2);
            main.pushstr_newL(";");
            /*
            if(body->getTypeofNode()==NODE_BLOCKSTMT)
            {
              blockStatement* newbody=(blockStatement*)body;
              newbody->addToFront(assign);
              body=newbody;
            }*/
        }
          
      }

  }
  
  if(extractElemFunc!=NULL)
  { 
    if(neighbourIteration(iteratorMethodId->getIdentifier()))
    { 
      sprintf(strBuffer,"if(%s >= startv && %s <=endv)",iterator->getIdentifier(),iterator->getIdentifier());
      main.pushstr_newL(strBuffer); 
      main.pushstr_newL("{");

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

      generateBlock((blockStatement*)body,false);
      
      if(forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS||forAll->getParent()->getParent()->getTypeofNode()==NODE_ITRRBFS)
          main.pushstr_newL("}");
        
      //main.pushstr_newL("test1");
      main.pushstr_newL("}");     //Closing local part - if
      //Else part if nbr is remote
        main.pushstr_newL("else"); 
        main.pushstr_newL("{");
          sprintf(strBuffer,"dest_pro = %s / part_size;",iterator->getIdentifier());
          main.pushstr_newL(strBuffer);
          sprintf(strBuffer,"itr = send_data[dest_pro].find(%s);",iterator->getIdentifier());
          main.pushstr_newL(strBuffer);
          list<argument*>  argList=extractElemFunc->getArgList();
          assert(argList.size()==1);
           Identifier* nodeNbr=argList.front()->getExpr()->getId();
            generate_addMessage((blockStatement*) body,1,nodeNbr,NULL);
        //main.pushstr_newL("test2");
        main.pushstr_newL("}");


      //Closing brace for filter  
      if(forAll->hasFilterExpr())
          main.pushstr_newL("}");
      //main.pushstr_newL("test3");
      main.pushstr_newL("}"); //Closing for-all
        //generate_sendCall(body);
        //generate_receiveCall(body);


      
    }
    else if(allGraphIteration(iteratorMethodId->getIdentifier()))
    {
      cout<<"Testing iterator method 1111 "<<iteratorMethodId->getIdentifier()<<"\n";
      //generateStatement(forAll->getBody());
      generateBlock((blockStatement*)forAll->getBody(),false);

      //Closing brace for filter  
      if(forAll->hasFilterExpr())
          main.pushstr_newL("}");
      main.pushstr_newL("}"); //Closing of for-all
      generate_sendCall(body);
      generate_receiveCall(body);
    }
    else
    { 
    
      cout<<"Testing iterator method 3333 "<<iteratorMethodId->getIdentifier()<<"\n";
      //generateStatement(forAll->getBody());
      generateBlock((blockStatement*)forAll->getBody(),false);


    }
  }
  else 
  {   
  
     if(elementsIteration(extractId->getIdentifier()))
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
    else if(allGraphIteration(iteratorMethodId->getIdentifier()))
    {
      cout<<"Testing iterator method 2222 "<<iteratorMethodId->getIdentifier()<<"\n";
      //generateStatement(forAll->getBody());
      generateBlock((blockStatement*)forAll->getBody(),false);
    }
     else
    { 
   
    cout<<iteratorMethodId->getIdentifier()<<"\n";
    generateStatement(forAll->getBody());
          

   /* if(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT)
       main.pushstr_newL("}");*/

    }

  }
  

} 


void mpi_cpp_generator::generateForSignature(forallStmt* forAll)
{
  cout<<"reaced generateForSig\n";
  char strBuffer[1024];
  Identifier* iterator = forAll->getIterator();
  if(!(forAll->isSourceField()))
  {
    cout<<"Reached here test\n";
    Identifier* source = forAll->getSource();
    main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
    main.pushstr_newL("gettimeofday(&start, NULL);");
    sprintf(strBuffer,"for (int i = 0; i < %s.size(); i++)",source->getIdentifier());
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("{");
    sprintf(strBuffer,"int %s = %s[i];",iterator->getIdentifier(),source->getIdentifier());
    main.pushstr_newL(strBuffer);
  }
  else if(forAll->isSourceProcCall())
  {
    cout<<"Reached here test1\n";
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
        sprintf(strBuffer,"for (%s %s = startv; %s <= endv; %s++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),iterator->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("{");
        
      }
      else
      {
        sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_edges",iterator->getIdentifier());
        main.pushstr_newL(strBuffer);
      }

    }
    else if(neighbourIteration(iteratorMethodId->getIdentifier()))
    { 
       
       char* graphId=sourceGraph->getIdentifier();
       char* methodId=iteratorMethodId->getIdentifier();
       list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
       sprintf(strBuffer,"for (%s %s = %s.%s[%s]; %s < %s.%s[%s+1]; %s ++) ","int","edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge");
       main.pushstr_newL(strBuffer);
       main.pushstr_newL("{");
       sprintf(strBuffer,"%s %s = %s.%s[%s] ;","int",iterator->getIdentifier(),graphId,"edgeList","edge"); //needs to move the addition of
       main.pushstr_newL(strBuffer);
       //statement to a different method.
    }
  }
  else if(forAll->isSourceField())
  {
    cout<<"Reached here test2\n";
    PropAccess* sourceField=forAll->getPropSource();
    Identifier* extractId=sourceField->getIdentifier2();
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

      }

  }
}

void mpi_cpp_generator::generateFor(forallStmt* forAll)
{ 
  cout<<"Reaced generate For\n";
  
  Identifier* sourceField;
  //proc_callExpr* extractElemFunc;
  
  //if(forAll->isSourceProcCall())
    //  extractElemFunc=forAll->getExtractElementFunc();
   PropAccess* sourceField1;

   
   Identifier* extractId;
    
    Identifier* iteratorMethodId;
    //if(extractElemFunc!=NULL)
     //iteratorMethodId=extractElemFunc->getMethodId();

   Identifier* iterator=forAll->getIterator();
   if(!(forAll->isSourceField()))
   {
      sourceField = forAll->getSource();
   }

   statement* body=forAll->getBody();
     char strBuffer[1024];
  
   //generateForSignature(forAll);

    //main.pushstr_newL("{");

     

      if(forAll->isSourceProcCall())
      {
        cout<<"Reached here test1\n";
        Identifier* sourceGraph=forAll->getSourceGraph();
        proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
        Identifier* iteratorMethodId=extractElemFunc->getMethodId();
        list<argument*>  argList=extractElemFunc->getArgList();
       assert(argList.size()==1);
       Identifier* nodeNbr=argList.front()->getExpr()->getId();
        if(neighbourIteration(iteratorMethodId->getIdentifier()))
        {
          cout<<"neighbor iteration\n";
          
          if(forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
          {
            cout<<"parent itrbfs"<<endl;

            generateForSignature(forAll);

            sprintf(strBuffer,"if(%s >= startv && %s <= endv)",iterator->getIdentifier(),iterator->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
              sprintf(strBuffer,"if (d[%s] < 0)",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"active_next.push_back(%s);",iterator->getIdentifier());
                main.pushstr_newL(strBuffer);
                sprintf(strBuffer,"d[%s] = d[%s] + 1;",iterator->getIdentifier(),nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
              main.pushstr_newL("}");
              sprintf(strBuffer,"if (d[%s] == d[%s] + 1)",iterator->getIdentifier(),nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.insert_indent();
                main.pushstr_newL("{");
                  sprintf(strBuffer,"p[%s].push_back(%s);",iterator->getIdentifier(),nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  generateBlock((blockStatement*)body,false);
                main.pushstr_newL("}");
            //Close of if part
            main.pushstr_newL("}");

            //Remote part
            main.pushstr_newL("else");
            main.pushstr_newL("{");
              sprintf(strBuffer,"dest_pro = %s / part_size;",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"send_data[dest_pro].push_back(d[%s]);",nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              num_messages++;
              sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              num_messages++;
              sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              num_messages++;
                generate_addMessage((blockStatement*)body,1,nodeNbr,NULL);
            main.pushstr_newL("}");

            main.pushstr_newL("}"); //Closing neighbor iteration
          }

          else if(forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
          {
            cout<<"parent itrbfs"<<endl;
                sprintf(strBuffer,"for(int %s=startv; %s<=endv; %s++)",nodeNbr->getIdentifier(),nodeNbr->getIdentifier(),nodeNbr->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            //sprintf(strBuffer,"int %s = levelNodes[phase][l] ;",bfsAbstraction->getIteratorNode()->getIdentifier());
            //main->pushstr_newL(strBuffer);
            sprintf(strBuffer,"if(d[%s] == phase)",nodeNbr->getIdentifier());
            main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"modified[%s] = true;",nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
              generateForSignature(forAll);

            sprintf(strBuffer,"if(%s >= startv && %s <= endv)",iterator->getIdentifier(),iterator->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            if(forAll->hasFilterExpr())
            { 
                Expression* filterExpr = forAll->getfilterExpr();
                cout<<"For has filter\n";
                Expression* expr1=filterExpr->getLeft();
                Expression* expr2=filterExpr->getRight();
                main.pushstr_space("if (");
                generateExpr(expr1);
                //sprintf(strBuffer," [%s]",iterator->getIdentifier());
                //main.pushstr_space(strBuffer);
                    const char* operatorString=getOperatorString(filterExpr->getOperatorType());
                    main.pushstr_space(operatorString);
                    //main.pushstr_space(" =");
                    generateExpr(expr2);
                //generateExpr(filterExpr);
                main.pushstr_newL(" )");
                main.pushstr_newL("{");

            } 
                 generateBlock((blockStatement*)body,false);
            
            if(forAll->hasFilterExpr())
            {
                main.pushstr_newL("}");
            }
            //Close of if part
            main.pushstr_newL("}");
            main.pushstr_newL("}"); //Neighbor iteration signature
            main.pushstr_newL("}");

            //Else part
            sprintf(strBuffer,"if(d[%s] == (phase+1))",nodeNbr->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
              main.pushstr_newL("for (int j=0;j<p[v].size();j++)");
              main.pushstr_newL("{");
                sprintf(strBuffer,"int w = p[%s][j];",nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
                main.pushstr_newL("if(!(w >= startv && w <= endv))");
                main.pushstr_newL("{");
                  main.pushstr_newL("dest_pro = w / part_size;");
                  sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  num_messages++;
                  sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",iterator->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  num_messages++;
                  generate_addMessage((blockStatement*)body,1,nodeNbr,NULL);
                main.pushstr_newL("}");
              main.pushstr_newL("}");
            main.pushstr_newL("}");

           // ASTNode* bfsAbst = forAll->getParent()->getParent();
           // statement* bfsAbstraction = (statement*) bfsAbst;
           // iterateBFS* babst = (iterateBFS*) bfsAbstraction;
           // statement* revBlock=babst->getRBFS()->getBody();
           main.pushstr_newL("}");
             generate_sendCall(body);
             generate_receiveCallBFS(forAll,2,nodeNbr);

           // main.pushstr_newL("}"); 
            /*
            //Remote part
            main.pushstr_newL("else");
            main.pushstr_newL("{");
              sprintf(strBuffer,"send_data[dest_pro].push_back(d[%s]);",nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
                generate_addMessage((blockStatement*)body,1,nodeNbr);
            main.pushstr_newL("}");
            */
           // main.pushstr_newL("}"); //Closing neighbor iteration
          }
        }
      }
    else if(!(forAll->isSourceField()))
    {
       generateForSignature(forAll);
      generateBlock((blockStatement*)body,false);
    //generateStatement(forAll->getBody());  

      main.pushstr_newL("}");
    }

    

} 

void mpi_cpp_generator:: generateVariableDecl(declaration* declStmt)
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

void mpi_cpp_generator::generate_exprLiteral(Expression* expr)
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

 void mpi_cpp_generator::generate_exprInfinity(Expression* expr)
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
  
  
  const char* mpi_cpp_generator::getOperatorString(int operatorId)
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

  void  mpi_cpp_generator::generate_exprRelational(Expression* expr)
  {
    cout<<"INSIDE RELATIONAL EXPR"<<"\n";
    
    generateExpr(expr->getLeft());
    
    main.space();
    const char* operatorString=getOperatorString(expr->getOperatorType());
    main.pushstr_space(operatorString);
    generateExpr(expr->getRight());
  }

void mpi_cpp_generator::generate_exprIdentifier(Identifier* id)
{
   main.pushString(id->getIdentifier());
}

void mpi_cpp_generator::generateExpr(Expression* expr)
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

void mpi_cpp_generator::generate_exprArL(Expression* expr)
{
  //cout<<"INSIDE RELATIONAL EXPR"<<"\n";
    if(expr->hasEnclosedBrackets())
      main.pushstr_space("(");
    generateExpr(expr->getLeft());
    
    main.space();
    const char* operatorString=getOperatorString(expr->getOperatorType());
    main.pushstr_space(operatorString);
    generateExpr(expr->getRight());
    if(expr->hasEnclosedBrackets())
      main.pushstr_space(")");

}

void mpi_cpp_generator::generate_exprArLReceive(Expression* expr)
{
  //cout<<"INSIDE RELATIONAL EXPR"<<"\n";
    if((expr->getLeft())->isPropIdExpr())
      generate_exprPropIdReceive((expr->getLeft())->getPropId());
    else
       generateExpr(expr->getLeft());
    
    main.space();
    const char* operatorString=getOperatorString(expr->getOperatorType());
    main.pushstr_space(operatorString);
    if((expr->getRight())->isPropIdExpr())
      generate_exprPropIdReceive((expr->getRight())->getPropId());
    else
      generateExpr(expr->getRight());

}


void mpi_cpp_generator::generate_exprProcCall(Expression* expr)
{
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if(methodId=="get_edge")
  {
    main.pushString("edge"); //To be changed..need to check for a neighbour iteration 
                             // and then replace by the iterator.
  }
}

void mpi_cpp_generator::generate_exprPropId(PropAccess* propId) //This needs to be made more generic.
{ char strBuffer[1024];
  //PropAccess* propId=(PropAccess*)expr->getPropId();
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());
  main.pushString(strBuffer);



}

void mpi_cpp_generator::generate_exprPropIdReceive(PropAccess* propId) //This needs to be made more generic.
{ char strBuffer[1024];
  //PropAccess* propId=(PropAccess*)expr->getPropId();
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  sprintf(strBuffer,"%s_%s",id2->getIdentifier(),id1->getIdentifier());
  main.pushString(strBuffer);

}

void mpi_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct)
{ 
  is_fixedPoint = true;
  char strBuffer[1024];
  Expression* convergeExpr=fixedPointConstruct->getDependentProp();
  Identifier* fixedPointId=fixedPointConstruct->getFixedPointId();
  statement* blockStmt=fixedPointConstruct->getBody();
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
  if(convergeExpr->getExpressionFamily()==EXPR_ID)
     dependentId=convergeExpr->getId();
  if(dependentId!=NULL)
  {
    if(dependentId->getSymbolInfo()->getType()->isPropType())
    { 
      //sprintf(strBuffer,"bool %s_fp = false ;",dependentId->getIdentifier());
      //main.pushstr_newL(strBuffer);
      if(dependentId->getSymbolInfo()->getType()->isPropNodeType())
      {  
          Type* type=dependentId->getSymbolInfo()->getType()->getInnerTargetType();
          //char* t = convertToCppType(type);
          sprintf(strBuffer,"%s is_%s (int startv,int endv,%s* %s)",convertToCppType(type),fixedPointId->getIdentifier(),convertToCppType(type),dependentId->getIdentifier());
          header.pushstr_newL(strBuffer);
          header.pushstr_newL("{");
          header.pushstr_newL("int sum = 0,res = 0;");
          header.pushstr_newL("for(int i = startv;i <= endv; i++)");
            header.pushstr_newL("{");
            sprintf(strBuffer,"if (%s[i] == 1)",dependentId->getIdentifier());
            header.pushstr_newL(strBuffer);
            header.insert_indent();
            header.pushstr_newL("res=1;");
            header.decrease_indent();
          header.pushstr_newL("}");
          header.pushstr_newL("MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);");
          header.pushstr_newL("if(sum ==0) return false;");
          header.pushstr_newL("else return true;");
          header.pushstr_newL("}");
          //main.pushString("#pragma omp parallel for");
          //sprintf(strBuffer," reduction(||:%s_fp)",dependentId->getIdentifier());
          //main.pushstr_newL(strBuffer);
      }
    }
  }
  main.pushstr_newL("int num_iter=0;");
  main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
  main.pushstr_newL("gettimeofday(&start, NULL);");
  main.pushString("while ( ");
  sprintf(strBuffer,"is_%s(startv,endv,%s)",fixedPointId->getIdentifier(),dependentId->getIdentifier());
  main.pushstr_space(strBuffer);
  //main.push('!');
  //main.pushString(fixedPointId->getIdentifier());
  main.pushstr_newL(" )");
  main.pushstr_newL("{");
  main.pushstr_newL("num_iter++;");
  main.pushstr_newL("vector < map<int,int> > send_data(np);");
  main.pushstr_newL("vector < map<int,int> > receive_data(np);");
  main.pushstr_newL("std::map<int,int>::iterator itr;");
  main.pushstr_newL("int dest_pro;");
  if(fixedPointConstruct->getBody()->getTypeofNode()!=NODE_BLOCKSTMT)
  generateStatement(fixedPointConstruct->getBody());
  else
    generateBlock((blockStatement*)fixedPointConstruct->getBody(),false);
  
  assert(convergeExpr->getExpressionFamily()==EXPR_UNARY||convergeExpr->getExpressionFamily()==EXPR_ID);
  /*
  
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
     main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
     main.pushstr_newL("send_data.clear();");
     main.pushstr_newL("receive_data.clear();");
     main.pushstr_newL("}");
}



void mpi_cpp_generator::generateBlock(blockStatement* blockStmt,bool includeBrace)
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
     //printf("CHECK IF INSIDE FOR ");//%d\n",stmt->getParent()->getParent()->getTypeofNode()==NODE_FORALLSTMT);
     generateStatement(stmt);

   }
   if(includeBrace)
   {
   main.pushstr_newL("}");
   }
}
void mpi_cpp_generator::generateFunc(ASTNode* proc)
{  char strBuffer[1024];
   Function* func=(Function*)proc;
   generateFuncHeader(func,false);
   generateFuncHeader(func,true);
   main.pushstr_newL("{");
   //main.insert_indent();
    main.pushstr_newL("int my_rank,np,part_size,startv,endv;");
    //main.pushstr_newL("MPI_Init(&argc,&argv);");
    main.pushstr_newL("struct timeval start, end, start1, end1;");
    main.pushstr_newL("long seconds,micros;");
    //main.pushstr_newL("MPI_Init(NULL,NULL);");
    //main.pushstr_newL("MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);");
    //main.pushstr_newL("MPI_Comm_size(MPI_COMM_WORLD, &np);");
    //main.pushstr_newL("MPI_Request request;");
    main.pushstr_newL("mpi::communicator world;");
    main.pushstr_newL("my_rank = world.rank();");
    main.pushstr_newL("np = world.size();");
    main.pushstr_newL("");

    main.pushstr_newL("gettimeofday(&start1, NULL);");
    sprintf(strBuffer,"%s.parseGraph();",graphId[0]->getIdentifier());
    main.pushstr_newL(strBuffer);
    //main.pushstr_newL("g.parseGraph();");
    main.pushstr_newL("gettimeofday(&end1, NULL);");
    main.pushstr_newL("seconds = (end1.tv_sec - start1.tv_sec);");
    main.pushstr_newL("micros = ((seconds * 1000000) + end1.tv_usec) - (start1.tv_usec);");
    main.pushstr_newL("if(my_rank == 0)");
    main.pushstr_newL("{");
    main.pushstr_newL("printf(\"The graph loading time = %ld secs.\\n\",seconds);");
    main.pushstr_newL("}");
    sprintf(strBuffer,"int max_degree = %s.%s();",graphId[0]->getIdentifier(),"max_degree");
    main.pushstr_newL(strBuffer);
    sprintf(strBuffer,"int *weight = %s.getEdgeLen();",graphId[0]->getIdentifier());
    main.pushstr_newL(strBuffer);
    //main.pushstr_newL("int* weight=g.getEdgeLen();");
    main.pushstr_newL("");

    sprintf(strBuffer,"part_size = %s.%s()/np;",graphId[0]->getIdentifier(),"num_nodes");
    main.pushstr_newL(strBuffer);
    main.pushstr_newL("startv = my_rank*part_size;");
    main.pushstr_newL("endv = startv + (part_size-1);");
    main.pushstr_newL("");
  
    //Calculation of Inter-partition degree
    main.pushstr_newL("int local_ipDeg=0, global_ipDeg=0;");
    main.pushstr_newL("for (int i=startv; i<=endv;i++)");
    main.pushstr_newL("{");
      main.pushstr_newL("for (int j = g.indexofNodes[i]; j<g.indexofNodes[i+1]; j++)");
      main.pushstr_newL("{");
        main.pushstr_newL("int nbr = g.edgeList[j];");
        main.pushstr_newL("if(!(nbr >= startv && nbr <=endv))");
        main.insert_indent();
          main.pushstr_newL("local_ipDeg++;");
        main.decrease_indent();
      main.pushstr_newL("}");
    main.pushstr_newL("}");
    main.pushstr_newL("");

    main.pushstr_newL("all_reduce(world, local_ipDeg, global_ipDeg, mpi::maximum<int>());");
    main.pushstr_newL("if(my_rank==0)");
    main.insert_indent();
      main.pushstr_newL("printf(\"Global inter part degree %d\\n\",global_ipDeg);");
    main.decrease_indent();

   generateBlock(func->getBlockStatement(),false);
   main.NewLine();

   main.pushstr_newL("gettimeofday(&end, NULL);");
   main.pushstr_newL("seconds = (end.tv_sec - start.tv_sec);");
   main.pushstr_newL("micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);");
   main.pushstr_newL("if(my_rank==0)");
   main.pushstr_newL("{");
      if(is_fixedPoint)
        main.pushstr_newL("printf(\"The number of iterations taken = %d\\n\",num_iter);");
      main.pushstr_newL("printf(\"The iteration time = %ld micro secs.\\n\",micros);");
      main.pushstr_newL("printf(\"The iteration time = %ld secs.\\n\",seconds);");
   main.pushstr_newL("}");
   main.pushstr_newL("MPI_Finalize();");
   main.push('}');

   return;

} 

const char* mpi_cpp_generator:: convertToCppType(Type* type)
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
  else if(type->isCollectionType())
  { 
     int typeId=type->gettypeId();

      switch(typeId)
      {
        case TYPE_SETN:
          return "std::vector<int>";
       
        default:
         assert(false);          
      }

  }

  return "NA";
}

void mpi_cpp_generator:: generateFuncHeader(Function* proc,bool isMainFile)
{ 
  //cout<<"INSIDE THIS VIEW US"<<"\n";
 
  dslCodePad& targetFile=isMainFile?main:header;
  targetFile.pushString("void ");
  targetFile.pushString(proc->getIdentifier()->getIdentifier());
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


bool mpi_cpp_generator::openFileforOutput()
{ 

  char temp[1024];
  printf("fileName %s\n",fileName);
  sprintf(temp,"%s/%s.h","../graphcode/generated_mpi",fileName);
  headerFile=fopen(temp,"w");
  if(headerFile==NULL)
     return false;
  header.setOutputFile(headerFile);

  sprintf(temp,"%s/%s.cc","../graphcode/generated_mpi",fileName);
  bodyFile=fopen(temp,"w"); 
  if(bodyFile==NULL)
     return false;
  main.setOutputFile(bodyFile);     
  
  return true;
}
void mpi_cpp_generator::generation_end()
  {
     header.NewLine();
     header.pushstr_newL("#endif");   
  }

 void mpi_cpp_generator::closeOutputFile()
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

bool mpi_cpp_generator::generate()
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


  void mpi_cpp_generator::setFileName(char* f) // to be changed to make it more universal.
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

