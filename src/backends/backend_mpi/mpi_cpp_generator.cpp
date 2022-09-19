#include "mpi_cpp_generator.h"
#include<string.h>
#include<cassert>
#include <map>
int count_int = 0;
int count_float = 0;
int count_double = 0;
bool is_fixedPoint = false;
int num_messages = 0;
bool comm_needed = false;
reduction_details* red_details = NULL;
make_par* mp = NULL;
bool make_decl_par = false;
bool comm_needed_gbl = false;
bool convertTypeCheck = false;
vector <int> all_to_all_types;
map <string, string> function_argument_propid;
int for_all_count = 0;
vector <string> arrays_declared;
bool nodes_to = false;
bool is_an_edge = false;
bool neighbour_check = false;
bool outer_loop_forall = true;
enum{SEND_DATA, SEND_DATA_FLOAT, SEND_DATA_DOUBLE};


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
  addIncludeToFile("graph.hpp",header,false);

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

  main->pushstr_newL("int phase = 0 ;");
  main->pushstr_newL("vector <int> active;");
  main->pushstr_newL("vector<int> active_next;");
  sprintf(strBuffer,"vector <vector<int>> p (part_size);");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"int* d = new int[part_size];");
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"for (int t = 0; t < %s; t++)","part_size");
  main->pushstr_newL(strBuffer);
  main->pushstr_newL("{");
  main->pushstr_newL("d[t] = -1;");
  main->pushstr_newL("}");
  sprintf(strBuffer,"active.push_back(%s);",root->getIdentifier());
  main->pushstr_newL(strBuffer);
  
  sprintf(strBuffer,"if(%s >= startv && %s <= endv)",root->getIdentifier(), root->getIdentifier());
  main->pushstr_newL(strBuffer);
  sprintf(strBuffer,"{");
  main->pushstr_newL(strBuffer);
  if(comm_needed_gbl)
  {
    {
    sprintf(strBuffer,"d[%s - startv] = 0;",root->getIdentifier());
    main->pushstr_newL(strBuffer);
  }
  }
  else
  {
    sprintf(strBuffer,"d[%s] = 0;",root->getIdentifier());
    main->pushstr_newL(strBuffer);
  }
  sprintf(strBuffer,"}");
  main->pushstr_newL(strBuffer);

}

 void add_BFSIterationLoop(dslCodePad* main, dslCodePad* header, iterateBFS* bfsAbstraction)
 {
   
    char strBuffer[1024];
    char* iterNode=bfsAbstraction->getIteratorNode()->getIdentifier();
    char* graphId=bfsAbstraction->getGraphCandidate()->getIdentifier();
    main->pushstr_newL("while(is_finished(startv,endv,active))");
    generateIsFinished(header);
    main->pushstr_newL("{");
      main->pushstr_newL("vector < vector <int> > send_data(np);");
      main->pushstr_newL("vector < vector <int> > receive_data(np);");
      main->pushstr_newL("vector < vector <float> > send_data_float(np);");
      main->pushstr_newL("vector < vector <float> > receive_data_float(np);");
      main->pushstr_newL("vector < vector <double> > send_data_double(np);");
      main->pushstr_newL("vector < vector <double> > receive_data_double(np);");
      //main->pushstr_newL("int dest_pro;");
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
    if(comm_needed_gbl) {
      sprintf(strBuffer,"bool* modified = new bool[part_size];");
      main->pushstr_newL(strBuffer);
      sprintf(strBuffer,"for (int t = 0; t < part_size; t++)");
      main->pushstr_newL(strBuffer);
      main->pushstr_newL("{");
      main->pushstr_newL("modified[t] = false;");
      main->pushstr_newL("}");
    } else {
      sprintf(strBuffer,"bool* modified = new bool[%s.num_nodes()];",graphID->getIdentifier());
      main->pushstr_newL(strBuffer);
      sprintf(strBuffer,"for (int t = 0; t < %s.num_nodes(); t++)",graphID->getIdentifier());
      main->pushstr_newL(strBuffer);
      main->pushstr_newL("{");
      main->pushstr_newL("modified[t] = false;");
      main->pushstr_newL("}"); 
    }
    main->pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
    main->pushstr_newL("while (phase > 0)") ;
    main->pushstr_newL("{");
    //main->pushstr_newL("#pragma omp parallel for");
    main->pushstr_newL("vector <vector <int> > send_data(np);");
    main->pushstr_newL("vector <vector <int> > receive_data(np);");
    main->pushstr_newL("vector <vector <float> > send_data_float(np);");
    main->pushstr_newL("vector <vector <float> > receive_data_float(np);");
    main->pushstr_newL("vector <vector <double> > send_data_double(np);");
    main->pushstr_newL("vector <vector <double> > receive_data_double(np);");
    //main->pushstr_newL("int dest_pro;");
    //sprintf(strBuffer,"for( %s %s = %s; %s < levelCount[phase] ; %s++)","int","l","0","l","l"); 
    //main->pushstr_newL(strBuffer);


  }

//Function to generate BFS Abstraction
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
   main.pushstr_newL("vector <int> ().swap(active_next);");
   main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
   main.pushstr_newL("send_data.clear();");
   main.pushstr_newL("vector <vector <int> >().swap(send_data);");
   main.pushstr_newL("receive_data.clear();");
   main.pushstr_newL("vector <vector <int> >().swap(receive_data);");
   main.pushstr_newL("send_data_float.clear();");
   main.pushstr_newL("vector <vector <float> >().swap(send_data_float);");
   main.pushstr_newL("receive_data_float.clear();");
   main.pushstr_newL("vector <vector <float> >().swap(receive_data_float);");
   main.pushstr_newL("send_data_double.clear();");
   main.pushstr_newL("vector <vector <double> >().swap(send_data_double);");
   main.pushstr_newL("receive_data_double.clear();");
   main.pushstr_newL("vector <vector <double> >().swap(receive_data_double);");
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
              if(comm_needed_gbl) {
                sprintf(strBuffer," && modified[%s-startv] == true)",iterNode->getIdentifier());
              } else {
                sprintf(strBuffer," && modified[%s] == true)",iterNode->getIdentifier());
              }
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                if(comm_needed_gbl) {
                  sprintf(strBuffer,"modified[%s - startv] = false;",iterNode->getIdentifier());
                } else {
                  sprintf(strBuffer,"modified[%s] = false;",iterNode->getIdentifier());
                }
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
              if(comm_needed_gbl) {
                sprintf(strBuffer,"if( modified[%s-startv] == true )",iterNode->getIdentifier());
              } else {
                sprintf(strBuffer,"if( modified[%s] == true )",iterNode->getIdentifier());
              }
              
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                // sprintf(strBuffer,"modified[%s] = false;",iterNode->getIdentifier());
                if(comm_needed_gbl) {
                  sprintf(strBuffer,"modified[%s - startv] = false;",iterNode->getIdentifier());
                } else {
                  sprintf(strBuffer,"modified[%s] = false;",iterNode->getIdentifier());
                }
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
   main.pushstr_newL("vector <vector <int> >().swap(send_data);");
   main.pushstr_newL("receive_data.clear();");
   main.pushstr_newL("vector <vector <int> >().swap(receive_data);");
   main.pushstr_newL("send_data_float.clear();");
   main.pushstr_newL("vector <vector <float> >().swap(send_data_float);");
   main.pushstr_newL("receive_data_float.clear();");
   main.pushstr_newL("vector <vector <float> >().swap(receive_data_float);");
   main.pushstr_newL("send_data_double.clear();");
   main.pushstr_newL("vector <vector <double> >().swap(send_data_double);");
   main.pushstr_newL("receive_data_double.clear();");
   main.pushstr_newL("vector <vector <double> >().swap(receive_data_double);");
   main.pushstr_newL("}"); //outer while


 }

//Fuction to handle translation of different types of statements
void mpi_cpp_generator::generateStatement(statement* stmt)
{  
   if(stmt->getTypeofNode()==NODE_BLOCKSTMT)
     {
       cout<< "Here123 BLOCKSTMT\n";
       generateBlock((blockStatement*)stmt);

     }
   if(stmt->getTypeofNode()==NODE_DECL)
   {
     cout<< "Here123 DECL\n";
      cout<<"calling var decl"<<endl;
      generateVariableDecl((declaration*)stmt);

   } 
   if(stmt->getTypeofNode()==NODE_ASSIGN)
     { 
       cout<< "Here123 ASSIGN\n";
       cout<<"calling node assign"<<endl;
       generateAssignmentStmt((assignment*)stmt);
     }
    
   if(stmt->getTypeofNode()==NODE_WHILESTMT) 
   {
     outer_loop_forall = false;
    // generateWhileStmt((whileStmt*) stmt);
   }
   
   if(stmt->getTypeofNode()==NODE_IFSTMT)
   {
     cout<< "Here123 IFSTMT\n";
      generateIfStmt((ifStmt*)stmt);
   }

   if(stmt->getTypeofNode()==NODE_DOWHILESTMT)
   {
      outer_loop_forall = false;
      generateDoWhileStmt((dowhileStmt*) stmt);
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
      outer_loop_forall = false;
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
      cout<< "Here123 IFSTMT\n";
      cout<<"calling proc call"<<endl;
      generateProcCall((proc_callStmt*) stmt);
    }
    if(stmt->getTypeofNode()==NODE_UNARYSTMT)
    { 
      cout<< "Here123 UNARY\n";
      cout<<"calling generateUnary"<<endl;
      generate_exprUnary(((unary_stmt*)stmt)->getUnaryExpr());
    }


}

//Function to translate reduction statement
void mpi_cpp_generator::generateReductionStmt(reductionCallStmt* stmt)
{ char strBuffer[1024];
  
  if(stmt->get_type() == 5)
  {  
      
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
  else if(stmt->get_type() == 3)
  {
    red_details = new reduction_details();
    printf("Reduction type 3\n");
    Identifier* id = stmt->getLeftId();
    int op = stmt->reduction_op();
    Expression* rhs = stmt->getRightSide();
    main.pushstr_space(id->getIdentifier());
    const char* operatorString=getOperatorString(op);
    main.pushstr_space(operatorString);
    generateExpr(rhs);
    main.pushstr_newL(";");
    red_details->set_reductionDetails(true,op,id);
    cout<<red_details->get_reductionOp()<<endl;
    printf("Exited successfully\n");
  }

}


//Function to translate reduciton statement after receiving the data...Can be merged with previous function
void mpi_cpp_generator::generateInnerReductionStmt(reductionCallStmt* stmt,int send)
{ char strBuffer[1024];
  if(stmt->get_type() == 5)
  {    
      main.pushstr_newL("for(int t=0;t<np;t++)");
      main.pushstr_newL("{");
        main.pushstr_newL("if(t != my_rank)");
        main.pushstr_newL("{");
            main.pushstr_newL("for (auto x : receive_data[t])");
            main.pushstr_newL("{");
              
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
      main.pushstr_newL("}");
            main.pushstr_newL("}");
        main.pushstr_newL("}");
  }
  else if(stmt->get_type()==3)
  {
    red_details = new reduction_details();
    printf("Reduction type 3\n");
    Identifier* id = stmt->getLeftId();
    int op = stmt->reduction_op();
    Expression* rhs = stmt->getRightSide();
    main.pushstr_space(id->getIdentifier());
    const char* operatorString=getOperatorString(op);
    main.pushstr_space(operatorString);
    generateExpr(rhs);
    main.pushstr_newL(";");
    red_details->set_reductionDetails(true,op,id);
    cout<<red_details->get_reductionOp()<<endl;
    printf("Exited successfully\n");
  }
}


//Function to generate reduction statement for sending the data
void mpi_cpp_generator::generateReductionStmtForSend(reductionCallStmt* stmt,bool send)
{ char strBuffer[1024];
  if(stmt->get_type() == 5)
      {
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
  else if(stmt->get_type()==3)
  {
    printf("Recution for send type = 3\n");
  }
}

//Function to generate expressions after reciving the data sent
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
  else if(expr->isProcCallExpr())
  {
    cout<<"Expression is proce call....calling procCall for send\n";
    generateProcCallForSend(expr,send,remote,replace);
  }
    
     else if(expr->isInfinity())
       {
         cout<<"Expression is infinity\n";
         //generate_exprInfinity(expr);
       }

       else if(expr->isIdentifierExpr())
       {
         cout<<"Expression is identifier\n";
         generateIdentifierForSend(expr->getId(),send,remote,replace);
         //generate_exprIdentifier(expr->getId());
       }
  else 
       {
         assert(false);
       }
}



void mpi_cpp_generator::generateProcCallForSend(Expression* expr,int send, Identifier* remote, Identifier* replace)
{
    char strBuffer[1024];
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if(methodId=="get_edge")
  {
    main.pushString("edge + startv"); //To be changed..need to check for a neighbour iteration 
                             // and then replace by the iterator.
  }
  else if(methodId=="num_nodes")
  {
    Identifier* id1 = proc->getId1();
    //main.pushString(id1->getIdentifier());
    main.pushString("_actual_num_nodes");
  }
  else if(methodId == "count_inNbrs")
  {
    Identifier* id1 = proc->getId1();
    Identifier* arg;
    list<argument*> argList = proc->getArgList();
    list<argument*> :: iterator itr;
    if(argList.size() == 1)
    {
      itr = argList.begin();
      if((*itr)->isExpr())
        {
          if((*itr)->getExpr()->isIdentifierExpr())
            arg = (*itr)->getExpr()->getId();
        }
    }
    if(send==3)
    {
      if(comm_needed_gbl) {
        sprintf(strBuffer,"(local_index_nbr_1 - local_index_nbr)");
      } else {
        sprintf(strBuffer,"(%s.indexofNodes[%s+1]-%s.indexofNodes[%s])",id1->getIdentifier(),arg->getIdentifier(),id1->getIdentifier(),arg->getIdentifier());
      }
      main.pushString(strBuffer);
    }

  }
  else if(methodId == "is_an_edge")
  {
    Identifier* id1 = proc->getId1();
    Identifier* arg;
    list<argument*> argList = proc->getArgList();
    list<argument*> :: iterator itr;
    sprintf(strBuffer,"%s.check_if_nbr(",id1->getIdentifier());
    main.pushstr_space(strBuffer);
    int argumentTotal = argList.size();
    int arg_currNo=0;
    int maximum_arginline = 4;
    for(itr=argList.begin();itr!=argList.end();itr++)
    {
      arg_currNo++;
      argumentTotal--;

      if((*itr)->isExpr())
      {
          if((*itr)->getExpr()->isIdentifierExpr())
            arg = (*itr)->getExpr()->getId();
            main.pushString(arg->getIdentifier());
       }
      
      if(argumentTotal>0)
         main.pushstr_space(",");

      if(arg_currNo==maximum_arginline)
      {
         main.NewLine();  
         arg_currNo=0;  
      } 
     // if(argumentTotal==0)
         
    }
    main.pushstr_space(")");

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
  else if(stmt->lhs_isIdentifier())
  {
    Identifier* lhs = stmt->getId();
    if(send == 3)
      generateIdentifierForSend(lhs,send,remote,replace);
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
    
  if (send == 3)
    main.pushstr_newL(";");
}

void mpi_cpp_generator::generateIdentifierForSend(Identifier* stmt,int send, Identifier* remote,Identifier* replace)
{
  if(send == 3)
  {
    Identifier* id = stmt;
      char strBuffer[1024];
      if(mp!=NULL)
      {
        if(mp->check_makeParallel())
        {
          if(strcmp(mp->get_parId()->getIdentifier(),id->getIdentifier())==0)
          {
            main.pushString(id->getIdentifier());
            assert(mp->get_Iter()!=NULL);
            if(comm_needed_gbl)
              sprintf(strBuffer,"[%s-startv]",mp->get_Iter()->getIdentifier());
            else
              sprintf(strBuffer,"[%s]",mp->get_Iter()->getIdentifier());
            main.pushstr_space(strBuffer);
          }
          else
            main.pushString(id->getIdentifier());
        }
      }
      else
        main.pushString(id->getIdentifier());
  }
}

// TODO change this so that send/recv_data<float> or send/recv_data<int> are used separately
void mpi_cpp_generator::generatePropAccessForSend(PropAccess* stmt,int send, Identifier* remote,Identifier* replace)
{
  
  char strBuffer[1024];
  Identifier* id1 = stmt->getIdentifier1();
  Identifier* id2 = stmt->getIdentifier2();
  string i1(id1->getIdentifier());
  string i2(id2->getIdentifier());
   cout << "...." << std::endl;
  assert(remote != NULL);
  string rem(remote->getIdentifier());
  cout<<"Testing........................."<<i1 << rem <<endl;
  if(i1 == rem)
  {
    if(send == 1)
    {
      // check the type of stmt. then based on that update following code
      // std::cout<< "type of the stmt for send "<< convertToCppType(id2->getSymbolInfo()->getType()->getInnerTargetType())<< std::endl;
      std::string typeStr(convertToCppType(id2->getSymbolInfo()->getType()->getInnerTargetType()));
      if(typeStr == "int") {
          sprintf(strBuffer,"send_data[dest_pro].push_back(");
          num_messages++;
          all_to_all_types.push_back(SEND_DATA);
      } else if(typeStr == "float") {
          sprintf(strBuffer,"send_data_float[dest_pro].push_back(");
          all_to_all_types.push_back(SEND_DATA_FLOAT);
      } else if(typeStr == "double") {
          sprintf(strBuffer,"send_data_double[dest_pro].push_back(");
          all_to_all_types.push_back(SEND_DATA_DOUBLE);
      }
      main.pushString(strBuffer);
      
      if(nodes_to)
        generate_exprPropId_Pull(stmt, replace);
      else
        generate_exprPropId(stmt);
      main.pushstr_newL(");");
    }
    else if(send == 3)
    {
      generate_exprPropIdReceive(stmt);
    }
    else
    {
      cout<<"send type is.sssssssssssssssssssssss 2"<<endl;
      if(replace !=NULL)
      //if(to_replace == true)
      {
         cout<<"reached here.........................replace not equal to null\n";
        PropAccess* newProp;

        newProp=PropAccess::createPropAccessNode(replace,id2);
        //cout<<id2->getIdentifier() << endl;
        cout<<newProp->getIdentifier1()->getIdentifier() << endl;
        cout<<newProp->getIdentifier2()->getIdentifier() << endl;
        //TableEntry* t = id2->getSymbolInfo();
        Type* t = id2->getSymbolInfo()->getType()->getInnerTargetType();
        //TableEntry* t1 = newProp->getPropSymbT()->findEntryInST(id2);
        if(t == NULL)
          cout<<"t  NULL\n";
        main.pushstr_space(convertToCppType(t));
        generate_exprPropIdReceive(newProp);
        std::string typeStr(convertToCppType(t));
        if(typeStr == "int") {
            sprintf(strBuffer," = receive_data[t][x+%d];", count_int);
            count_int++;
        } else if(typeStr == "float") {
            sprintf(strBuffer," = receive_data_float[t][y_+%d];", count_float);
            count_float++;
        } else if(typeStr == "double") {
            sprintf(strBuffer," = receive_data_double[t][z_+%d];", count_double);
            count_double++;
        }
        main.pushstr_newL(strBuffer);
      }

      else
      {  
        cout<<"reached here.........................replace  equal to null\n";
        Type* t = id2->getSymbolInfo()->getType()->getInnerTargetType();
        main.pushstr_space(convertToCppType(t));
        generate_exprPropIdReceive(stmt);
        std::string typeStr(convertToCppType(t));
        if(typeStr == "int") {
            sprintf(strBuffer," = receive_data[t][x+%d];", count_int);
            count_int++;
        } else if(typeStr == "float") {
            sprintf(strBuffer," = receive_data_float[t][y_+%d];", count_float);
            count_float++;
        } else if(typeStr == "double") {
            sprintf(strBuffer," = receive_data_double[t][z_+%d];", count_double);
            count_double++;
        }
        main.pushstr_newL(strBuffer);
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

//Function to generate arithmetic and logical expression after receiving the data
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

  generateExprForSend(left,send,remote,replace);
  if (send == 3)
  {
    main.space();
    const char* operatorString=getOperatorString(stmt->getOperatorType());
    main.pushstr_space(operatorString);
  }
  Expression* right = stmt->getRight();

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


//Function to translate if statement
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

//Function to translate assignment statement
void mpi_cpp_generator::generateAssignmentStmt(assignment* asmt)
{  
  char strBuffer[1024];
   
   if(asmt->lhs_isIdentifier())
   { 
      Identifier* id=asmt->getId();
      if(mp!=NULL)
      {
        if(mp->check_makeParallel())
        {
          if(strcmp(mp->get_parId()->getIdentifier(),id->getIdentifier())==0)
          {
            main.pushString(id->getIdentifier());
            assert(mp->get_Iter()!=NULL);
            if(comm_needed_gbl) {
              sprintf(strBuffer,"[%s-startv]",mp->get_Iter()->getIdentifier());
            } else {
              sprintf(strBuffer,"[%s]",mp->get_Iter()->getIdentifier());
            }
            main.pushstr_space(strBuffer);
          }
          else
            main.pushString(id->getIdentifier());
        }
      }
      else
        main.pushString(id->getIdentifier());
   }
   else if(asmt->lhs_isProp())  //the check for node and edge property to be carried out.
   {
     PropAccess* propId=asmt->getPropId();
     
      if(comm_needed_gbl) {
        sprintf(strBuffer,"if(%s >= startv && %s <= endv)",propId->getIdentifier1()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.insert_indent();
        sprintf(strBuffer,"%s[%s - startv]",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
        main.pushstr_space(strBuffer);
      } else {
        main.pushString(propId->getIdentifier2()->getIdentifier());
        main.push('[');
        main.pushString(propId->getIdentifier1()->getIdentifier());
        main.push(']');
      }
   }

   main.pushString(" = ");
   generateExpr(asmt->getExpr());
   main.pushstr_newL(";");
   if (comm_needed_gbl)
      main.decrease_indent();


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
          for(itr=argList.begin();itr!=argList.end();itr++)
          { 
            assignment* assign=(*itr)->getAssignExpr();
            Identifier* lhsId=assign->getId();
            Expression* exprAssigned=assign->getExpr();
            // BC = new float[g.num_nodes()];
            // TODO
            // cout<< "TYYYPEEE "<< convertToCppType(lhsId->getSymbolInfo()->getType())<< endl;
            //convertTypeCheck = true;
            //sprintf(strBuffer,"%s = new %s[part_size]",lhsId->getIdentifier(), convertToCppType(lhsId->getSymbolInfo()->getType()));
            //convertTypeCheck = false;
            //main.pushString(strBuffer);
            //main.pushstr_newL(";");
            
          }
          if(comm_needed_gbl) {
            sprintf(strBuffer,"for (%s %s = 0; %s < part_size; %s++) ","int","t","t","t");
          } else {
            sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s++) ","int","t","t",procedure->getId1()->getIdentifier(),"num_nodes","t");
          }
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("{");
          for(itr=argList.begin();itr!=argList.end();itr++)
          { 
            assignment* assign=(*itr)->getAssignExpr();
            Identifier* lhsId=assign->getId();
            Expression* exprAssigned=assign->getExpr();
            if (comm_needed_gbl)
              sprintf(strBuffer,"%s[%s] = ",lhsId->getIdentifier(),"t");
            else
              sprintf(strBuffer,"%s[%s] = ",lhsId->getIdentifier(),"t");
            main.pushString(strBuffer);
            generateExpr(exprAssigned);

            main.pushstr_newL(";");
            
          }
             
        main.pushstr_newL("}");

       }
}

// add code here for handling additional types
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
     if(comm_needed_gbl) {
       sprintf(strBuffer,"part_size");
     } else {
       sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     }
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
     if(comm_needed_gbl) {
       sprintf(strBuffer,"part_size");
     } else {
       sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     }
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
    //  sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     if(comm_needed_gbl) {
       sprintf(strBuffer,"part_size");
     } else {
       sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     }
     main.pushString(strBuffer);
     main.pushString("]");
     main.pushstr_newL(";");
  }
  if(targetType->gettypeId()==TYPE_DOUBLE)
  {
     main.pushString("=");
     main.pushString(DOUBLEALLOCATION);
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
    //  sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     if(comm_needed_gbl) {
       sprintf(strBuffer,"part_size");
     } else {
       sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     }
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
          main.pushstr_space(" 0");
          break;
      case TYPE_FLOAT:
      case TYPE_DOUBLE:
          main.pushstr_space(" 0.0");
          break;
      case TYPE_BOOL:
          main.pushstr_space(" false");
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

//********Function to generate send collective communication call***********//
void mpi_cpp_generator::generate_sendCall(statement* body)
{
    char strBuffer[1024];
    //sprintf(strBuffer,"MPI_Alltoall(send_data,3*max_degree,MPI_INT,recv_data,3*max_degree,MPI_INT,MPI_COMM_WORLD);");
    //main.pushstr_newL(strBuffer);]
    if(all_to_all_types.size() > 0)
    {
      for (int i = 0; i < all_to_all_types.size(); i++)
      {
        if(all_to_all_types[i] == SEND_DATA)
          main.pushstr_newL("all_to_all(world, send_data, receive_data);");
        else if (all_to_all_types[i] == SEND_DATA_FLOAT)
          main.pushstr_newL("all_to_all(world, send_data_float, receive_data_float);");
        else if (all_to_all_types[i] == SEND_DATA_DOUBLE)
          main.pushstr_newL("all_to_all(world, send_data_double, receive_data_double);");
      }
      all_to_all_types.clear();
    }
    else
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

//*****This can be modified to directly use generate_addMessage() function**********//
void mpi_cpp_generator::generate_receiveCallBFS(statement* body,int send,Identifier* remote)
{
              generate_addMessage(body,send,remote,NULL);

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
    if(outer_loop_forall)
    {
      main.pushstr_newL("vector < map<int,vector<int>> > send_data(np);");
      main.pushstr_newL("vector < map<int,vector<int>> > receive_data(np);");
      main.pushstr_newL("std::map<int,vector<int>>::iterator itr;");
      outer_loop_forall = false;
    }
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
       string itr_name = "edge"+to_string(for_all_count);
       if(comm_needed_gbl)
           sprintf(strBuffer,"for (%s %s = %s[%s - startv]; %s < %s[%s - startv + 1]; %s++) ","int",itr_name.c_str(),"local_index",nodeNbr->getIdentifier(),itr_name.c_str(),"local_index",nodeNbr->getIdentifier(),itr_name.c_str());
       else
          sprintf(strBuffer,"for (%s %s = %s.%s[%s]; %s < %s.%s[%s+1]; %s++) ","int",itr_name.c_str(),graphId,"indexofNodes",nodeNbr->getIdentifier(),itr_name.c_str(),graphId,"indexofNodes",nodeNbr->getIdentifier(),itr_name.c_str());
       main.pushstr_newL(strBuffer);
       main.pushstr_newL("{");
       if(comm_needed_gbl)
          sprintf(strBuffer,"%s %s = %s[%s] ;","int",iterator->getIdentifier(),"local_edgeList",itr_name.c_str()); //needs to move the addition of
       else
          sprintf(strBuffer,"%s %s = %s.%s[%s] ;","int",iterator->getIdentifier(),graphId,"edgeList",itr_name.c_str()); //needs to move the addition of
       main.pushstr_newL(strBuffer);
       if(is_an_edge)
       {
         sprintf(strBuffer,"vector <int> %s_list;",iterator->getIdentifier());
         main.pushstr_newL(strBuffer);
       }
       for_all_count += 1;
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

 
/*********************Function to generate message send and receive part*****************
 * int send = 1 generate message send, send = 2 generate message reception, send = 3 generate computation using recieved data**********/
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
      if (send == 1)
        generateReductionStmtForSend((reductionCallStmt*) stmt,send);  
      else if(send == 3 || send == 2)
        generateInnerReductionStmt((reductionCallStmt*) stmt,send);  

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
        Identifier* iteratorMethodId=extractElemFunc->getMethodId();
          statement* body = forAll->getBody();
          list<argument*>  argList=extractElemFunc->getArgList();
          Identifier* nodeNbr;
              if(argList.size()==1)
                nodeNbr=argList.front()->getExpr()->getId();
              //parent_ids.push_back(nodeNbr);
              //nbr_ids.push_back(iterator);
              
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
              count_int = 3;
              if(count_int > count_float) {
                sprintf(strBuffer,"int y_ = x/%d;", count_int);
              } else {
                sprintf(strBuffer,"int y_ = x*%d;", count_int);
              }
              main.pushstr_newL(strBuffer);
              if(count_int > count_double) {
                sprintf(strBuffer,"int z_ = x/%d;", count_int);
              } else {
                sprintf(strBuffer,"int z_ = x*%d;", count_int);
              }
              main.pushstr_newL(strBuffer);
                generate_addMessage(/*(blockStatement*)*/body,2,nodeNbr,NULL);
              count_int = 0;
              count_float = 0;
              count_double = 0;
              sprintf(strBuffer,"if (d[%s-startv] < 0 )",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"active_next.push_back(%s);",iterator->getIdentifier());
                main.pushstr_newL(strBuffer);
                sprintf(strBuffer,"d[%s-startv] = d_%s + 1;",iterator->getIdentifier(),nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
              main.pushstr_newL("}");
              sprintf(strBuffer,"if (d[%s-startv] == d_%s+1)",iterator->getIdentifier(),nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
             // main.insert_indent();
              main.pushstr_newL("{");
                sprintf(strBuffer,"p[%s-startv].push_back(%s);",iterator->getIdentifier(),nodeNbr->getIdentifier());
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
                      count_int = 2;
                      if(count_int > count_float) {
                        sprintf(strBuffer,"int y_ = x/%d;", count_int);
                      } else {
                        sprintf(strBuffer,"int y_ = x*%d;", count_int);
                      }
                      main.pushstr_newL(strBuffer);
                      if(count_int > count_double) {
                        sprintf(strBuffer,"int z_ = x/%d;", count_int);
                      } else {
                        sprintf(strBuffer,"int z_ = x*%d;", count_int);
                      }
                      main.pushstr_newL(strBuffer);
                      generate_addMessage(body,2,nodeNbr,iterator);
                      count_int = 0;
                      count_float = 0;
                      count_double = 0;
                      generate_addMessage(body,3,iterator,NULL); 
                      // main.pushstr_newL("//TESTT2");
                      main.pushstr_newL("}");
                  main.pushstr_newL("}");
              main.pushstr_newL("}");

            }
            else if(neighbourIteration(iteratorMethodId->getIdentifier()))
            {
                generate_addMessage(body,send,remote,replace);
            }
            else
            {
               string method(iteratorMethodId->getIdentifier());
                if(method == "nodes_to")
                {
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
                                      //count_int = 1;
                                      sprintf(strBuffer,"int %s = receive_data[t][x+1];",nodeNbr->getIdentifier());
                                      main.pushstr_newL(strBuffer);
                                      // int local_index_nbr = receive_data[t][x+2];
                                      sprintf(strBuffer,"int local_index_nbr = receive_data[t][x+2];");
                                      main.pushstr_newL(strBuffer);
                                      // int local_index_nbr_1 = receive_data[t][x+3];
                                      sprintf(strBuffer,"int local_index_nbr_1 = receive_data[t][x+3];");
                                      main.pushstr_newL(strBuffer);
                                      count_int = 4;

                                      // int y_ = x/4;
                                      if(count_int > count_float) {
                                        sprintf(strBuffer,"int y_ = x/%d;", count_int);
                                      } else {
                                        sprintf(strBuffer,"int y_ = x*%d;", count_int);
                                      }
                                      main.pushstr_newL(strBuffer);
                                      if(count_int > count_double) {
                                        sprintf(strBuffer,"int z_ = x/%d;", count_int);
                                      } else {
                                        sprintf(strBuffer,"int z_ = x*%d;", count_int);
                                      }
                                      main.pushstr_newL(strBuffer);

                                      // TODO modify for pr receive part
                                      generate_addMessage(body,2,iterator,NULL);
                                      count_int = 0;
                                      count_float = 0;
                                      count_double = 0;
                                      generate_addMessage(body,3,iterator,NULL); 
                                    // main.pushstr_newL("//TESTT");
                                  main.pushstr_newL("}");
                                main.pushstr_newL("}");
                            main.pushstr_newL("}");
                }
            }
      }
      /*
      else if(!forAll->isForall())           //For statment
      {
            Identifier* sourceGraph=forAll->getSourceGraph();
          Identifier* iterator = forAll->getIterator();
          proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
          Identifier* iteratorMethodId=extractElemFunc->getMethodId();
          statement* body = forAll->getBody();
          list<argument*>  argList=extractElemFunc->getArgList();
              assert(argList.size()==1);
              Identifier* nodeNbr=argList.front()->getExpr()->getId();  
              string method(iteratorMethodId->getIdentifier());
                if(method == "nodes_to")
                {
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
                                      count_int = 1;
                                      generate_addMessage(body,2,iterator,NULL);
                                      count_int = 0;
                                      generate_addMessage(body,3,iterator,NULL); 
                                  main.pushstr_newL("}");
                                main.pushstr_newL("}");
                            main.pushstr_newL("}");
                } 
      }*/
    }
    else if(stmt->getTypeofNode() == NODE_DECL && send == 2)
    {
      std::cout << "Declration in add message" << std::endl;
    }
    else if(stmt->getTypeofNode() == NODE_IFSTMT)
    {
      ifStmt* if_stmt = (ifStmt*)stmt;
      Expression* cond = if_stmt->getCondition();
      if(cond->isProcCallExpr())
      {
        char strBuffer[1024];
        proc_callExpr* proc=(proc_callExpr*)cond;
        string methodId(proc->getMethodId()->getIdentifier());
        if (methodId == "is_an_edge")
        {
          if(send == 1)
          {
            cout << "METOD ==================> IS AN EDGE" << endl;
            neighbour_check = true;
            //assert(nbr_ids.size() >= 1);
            //Identifier* nbr_id = nbr_ids.back();
            //assert(parent_ids.size() >= 1);
            //Identifier* parent_id = parent_ids.back();
            //for (int i =0; i<nbr_ids.size();i++)
             // std::cout << nbr_ids[0]->getIdentifier()<< " ";
            //cout << endl;
            //nbr_ids.pop_back();
            //parent_ids.pop_back();
            sprintf(strBuffer, "vector <int> nbr_list;");
            main.pushstr_newL(strBuffer);
            if(remote != NULL)
            sprintf(strBuffer, "for (int t = local_index[%s - startv]; t < local_index[%s - startv +1]; t++)",remote->getIdentifier(),remote->getIdentifier());
            else
            sprintf(strBuffer, "for (int t = local_index[ - startv]; t < local_index[ - startv +1]; t++)");
            main.pushstr_newL(strBuffer);
            main.insert_indent();
              sprintf(strBuffer, "nbr_list.push_back(local_edgeList[t]);");
              main.pushstr_newL(strBuffer);
            main.decrease_indent(); 
            if(remote != NULL)      
              sprintf(strBuffer,"send_data[dest_pro][%s] = nbr_list;",remote->getIdentifier());
            else
              sprintf(strBuffer,"send_data[dest_pro][] = nbr_list;");
            main.pushstr_newL(strBuffer);
          }
        }
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
      generateInnerReductionStmt((reductionCallStmt*) stmt,2);          

    }
    if (stmt->getTypeofNode() == NODE_FORALLSTMT)
    {
      statement* b = ((forallStmt*)stmt)->getBody();
      generateReceiveBlock((blockStatement*) b);
    }

   }
}

bool checkReductionCallStmt(reductionCallStmt* stmt,Identifier* remote)
{
  char strBuffer[1024];
  bool ret;
  if(stmt->get_type() == 5)
  {  
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
          
                //if(stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType())
               // { Type* type=stmt->getAssignedId()->getSymbolInfo()->getType();
                  
                 // main.pushstr_space(convertToCppType(type->getInnerTargetType()));
                //}
                //cout<<"INSIDE ARG ID"<<stmt->getAssignedId()->getSymbolInfo()->getType()->gettypeId()<<"\n";

              //  
                //sprintf(strBuffer,"%s_new",stmt->getAssignedId()->getIdentifier());
                //main.pushString(strBuffer);
                list<argument*>::iterator argItr;
                argItr=argList.begin();
                argItr++; 
                //main.pushString(" = ");
                checkExpr((*argItr)->getExpr(),remote);
                //main.pushstr_newL(";");
                
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
                          /*
                          main.pushstr_space(convertToCppType(((Identifier*)node)->getSymbolInfo()->getType()));
                          sprintf(strBuffer,"%s_new",((Identifier*)node)->getIdentifier());
                          main.pushString(strBuffer);
                          main.pushString(" = ");
                          generateExpr((Expression*)node1);*/
                          cout <<"id\n";
                        } 
                        if(node->getTypeofNode()==NODE_PROPACCESS)
                        {
                          PropAccess* p=(PropAccess*)node;
                          ret = checkPropAccess(p,remote);
                            if(ret == true)
                              return true;
                          /*
                          Type* type=p->getIdentifier2()->getSymbolInfo()->getType();
                          if(type->isPropType())
                          {
                            main.pushstr_space(convertToCppType(type->getInnerTargetType()));
                          }
                          
                          sprintf(strBuffer,"%s_new",p->getIdentifier2()->getIdentifier());
                          main.pushString(strBuffer);
                          main.pushString(" = ");
                          */
                          Expression* expr=(Expression*)node1;
                            checkExpr(expr,remote);
                          //generateExpr((Expression*)node1);
                          //main.pushstr_newL(";");
                        }
                        itr2++;
                }
            
                //main.pushString("if (");
                if(stmt->isTargetId())
                  //checkExpr(stmt->getTargetId(),remote);
                  cout<<"exp is id\n";
                else
                  return (checkPropAccess(stmt->getTargetPropId(),remote));
                //main.space();
                //main.pushstr_space(">");
                //generateExpr((*argItr)->getExpr())
                //generate_exprIdentifier(stmt->getAssignedId());            
                //main.pushString("_new");
                //main.pushstr_newL(")");
                //main.pushstr_newL("{");
                
                itr1=leftList.begin();
                i=0;
                for( ;itr1!=leftList.end();itr1++)
                {   ASTNode* node=*itr1;

                  //if(node->getTypeofNode()==NODE_ID)
                    //    {
                            //generate_exprIdentifier((Identifier*)node);
                      //  }
                  if(node->getTypeofNode()==NODE_PROPACCESS)
                    {
                      return(checkPropAccess((PropAccess*)node,remote));
                    } 
                   // if(node->getTypeofNode()==NODE_ID)
                     //   {
                       //     generate_exprIdentifier((Identifier*)node);
                        //}
                  //if(node->getTypeofNode()==NODE_PROPACCESS)
                    //{
                      //return (checkPropAccess(((PropAccess*)node)->getIdentifier2(),remote));
                    //}  

                }
          }
      }
  }
  else if(stmt->get_type() == 3)
  {
    printf("Reduction type 3\n");
    Identifier* id = stmt->getLeftId();
    int op = stmt->reduction_op();
    Expression* rhs = stmt->getRightSide();
    checkExpr(rhs,remote);
    //main.pushstr_space(id->getIdentifier());
    //const char* operatorString=getOperatorString(op);
    //main.pushstr_space(operatorString);
    //generateExpr(rhs);
  }
  return false;
}

bool checkPropAccess(PropAccess* stmt,Identifier* remote)
{
  char strBuffer[1024];
  Identifier* id1 = stmt->getIdentifier1();
  Identifier* id2 = stmt->getIdentifier2();
  string i1(id1->getIdentifier());
  string i2(id2->getIdentifier());
  string rem(remote->getIdentifier());
  cout<<"Testing.........................prop access in comm needed or not"<<i1 << rem <<endl;
  if(i1 == rem)
  {
    cout <<"Comm needed is true\n";
    return true;
  }
  
     
}


bool checkArL(Expression* stmt,Identifier* remote)
{
  cout<<"Reached generateArLForSend\n";
  char strBuffer[1024];
  bool ret;
  
  Expression* left = stmt->getLeft();

  ret = checkExpr(left,remote);
  if(ret == true)
    return true;
  Expression* right = stmt->getRight();

    ret = checkExpr(right,remote);
    if(ret == true)
      return true;
    cout<<"exiting generateArLForSend\n";
}

bool checkExpr(Expression* expr,Identifier* remote)
{
  cout<<" reached generateExprForSend\n";
  if(expr->isArithmetic() || expr->isLogical())
  {
    cout<<"Expression is arithmetics...calling arl\n";
    return (checkArL( expr,remote));
  }
  else if(expr->isPropIdExpr())
  {
    cout<<"Expression is propid...calling propidgen\n";
    //expr->getPropId()->getIdentifier2()->getSymbolInfo()->getType();
    cout <<"return value is "<<checkPropAccess(expr->getPropId(),remote)<<endl;
    return (checkPropAccess(expr->getPropId(),remote));
  }
  else if(expr->isLiteral())
  {
    cout<<"Expression is literal...calling literalforsend\n";
    //checkexprLiteral(expr,remote);
  }
  else if(expr->isProcCallExpr())
  {
    cout<<"Expression is proce call....calling procCall for send\n";
  }
    
     else if(expr->isInfinity())
       {
         cout<<"Expression is infinity\n";
         //generate_exprInfinity(expr);
       }

       else if(expr->isIdentifierExpr())
       {
         cout<<"Expression is identifier\n";
         //generate_exprIdentifier(expr->getId());
       }
  else 
       {
         assert(false);
       }
}


bool communication_needed(blockStatement* blockStmt, Identifier* nbr)
{
   return true;
   cout <<"Checking communication needed or not\n";
   list<statement*> stmtList=blockStmt->returnStatements();
   list<statement*> ::iterator itr;
    bool ret = false;
   for(itr=stmtList.begin();itr!=stmtList.end();itr++)
   {
     statement* stmt=*itr;
     //printf("CHECK IF INSIDE FOR ");//%d\n",stmt->getParent()->getParent()->getTypeofNode()==NODE_FORALLSTMT);
     //generateStatement(stmt);
     if(stmt->getTypeofNode()==NODE_ASSIGN)
     {
        assignment* astmt = (assignment*) stmt;
                  cout<<"reaced generateAssignmentForSend\n";
        if(astmt->lhs_isProp())
        {
          PropAccess* lhs = astmt->getPropId();
            ret = checkPropAccess(lhs,nbr);
            if(ret == true)
              return true;
        }
        Expression* rhs = astmt->getExpr();
        if(rhs->isPropIdExpr())
        {
          ret=checkPropAccess(rhs->getPropId(),nbr);
          if(ret == true)
              return true;
        }
        else
        {
          cout<<"Rhs of assignment is expr..calling generateExprForSend\n";
          ret=checkExpr(rhs,nbr);
          if(ret == true)
              return true;
        }
        //return true;
     }
     else if(stmt->getTypeofNode()==NODE_REDUCTIONCALLSTMT)
     {
        ret = checkReductionCallStmt((reductionCallStmt*)stmt,nbr);
        if(ret == true)
          return true;
     }

   }
   return false;

}

void mpi_cpp_generator::generateForAllFilter(forallStmt* forAll)
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
  list<argument*>  argList=extractElemFunc->getArgList();
  if(argList.size() == 1)
  {
   Identifier* nodeNbr=argList.front()->getExpr()->getId();
  }  
    char strBuffer[1024];

  if(forAll->hasFilterExpr())
  { 
    cout<<"Forall has filter\n";
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
        main.space();
        cout<<"Reached here\n";
        if(expr1->isIdentifierExpr())
        {
          Identifier* id = expr1->getId();
          
            if(id->getSymbolInfo()!=NULL)
            {
              if(id->getSymbolInfo()->getType()->isPropType())
              {
                if(comm_needed_gbl)
                  sprintf(strBuffer,"[%s - startv]",iterator->getIdentifier());
                else
                  sprintf(strBuffer,"[%s]",iterator->getIdentifier());
                main.pushstr_space(strBuffer);
              }
            }
          
        }
        cout<<"Reached here 2\n";
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
            if(comm_needed_gbl)
              sprintf(strBuffer," [%s - startv]",iterator->getIdentifier());
            else
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
}
//****************Function to translate forAll****************//
void mpi_cpp_generator::generateForAll(forallStmt* forAll)
{ 
  cout<<"Reached forall\n";
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
  list<argument*>  argList=extractElemFunc->getArgList();
  if(argList.size() == 1)
  {
   Identifier* nodeNbr=argList.front()->getExpr()->getId();
  }
  
    char strBuffer[1024];
  //bool comm_needed = false;
  //if(forAll->isForall())
  //{
    //generateForAll_header();
  //}
  int index = 0;
  if(extractElemFunc!=NULL)
  { 
    if(allGraphIteration(iteratorMethodId->getIdentifier()))
    { 
      list<statement*> stmtList=((blockStatement*)body)->returnStatements();
      list<statement*> ::iterator itr;      
      
      for(itr=stmtList.begin();itr!=stmtList.end();itr++)
      {
        //index++;
        statement* stmt=*itr;
        if(stmt->getTypeofNode()==NODE_DECL)
        {
            cout<<"it is all graph iter and decl stmt\n";
            make_decl_par = true;            
            mp = new make_par();
            mp->set_iter(iterator);
            generateStatement(stmt);
            index+=1;
        }
        else
        {
          make_decl_par = false;
          break;
        }
      }
    }
  }
  generateForAllSignature(forAll);
  cout<<"This two!!!\n";

  cout<<"Reached cp 2\n";
  if(extractElemFunc!=NULL)
  { 
    if(neighbourIteration(iteratorMethodId->getIdentifier()))
    { 
      cout<<"It is neighbor iteration\n";
      comm_needed = communication_needed((blockStatement*)body,iterator);
      if(comm_needed_gbl)
      {   
          if(is_an_edge)
          {
            sprintf(strBuffer,"if(!(%s >= startv && %s <=endv))",iterator->getIdentifier(),iterator->getIdentifier());
            main.pushstr_newL(strBuffer);            
          }
          else
          {
            sprintf(strBuffer,"if(%s >= startv && %s <=endv)",iterator->getIdentifier(),iterator->getIdentifier());
            main.pushstr_newL(strBuffer); 
          }
          main.pushstr_newL("{");

          if(is_an_edge)
          {
            sprintf(strBuffer,"int dest_pro = %s/part_size;", iterator->getIdentifier());
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer,"%s_list = receive_data[dest_pro][%s];",iterator->getIdentifier(),iterator->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("}"); //Closing of if - Receive side - for TC
          }

          generateForAllFilter(forAll);
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
          if(!is_an_edge)
            main.pushstr_newL("}");     //Closing local part - if

          //Closing brace for filter  
          if(forAll->hasFilterExpr())
            main.pushstr_newL("}");

          //Else part if nbr is remote
          if(!is_an_edge)
          {
            main.pushstr_newL("else"); 
            main.pushstr_newL("{");
              sprintf(strBuffer,"dest_pro = %s / part_size;",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"itr = send_data[dest_pro].find(%s);",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              list<argument*>  argList=extractElemFunc->getArgList();
              assert(argList.size()==1);
              Identifier* nodeNbr=argList.front()->getExpr()->getId();
              assert(nodeNbr != NULL);
              cout << "Calling add message from neighbor iter" << endl;
                generate_addMessage((blockStatement*) body,1,nodeNbr,iterator);
            //main.pushstr_newL("test2");
            main.pushstr_newL("}");
          }

      }
      else
      {
          generateBlock((blockStatement*)body,false);
      }
      
      //main.pushstr_newL("test3");
      main.pushstr_newL("}"); //Closing for-all
        //generate_sendCall(body);
        //generate_receiveCall(body);
      
    }
    else if(allGraphIteration(iteratorMethodId->getIdentifier()))
    {
      generateForAllFilter(forAll);
      cout<<"Testing iterator method 1111 "<<iteratorMethodId->getIdentifier()<<"\n";
      
      list<statement*> stmtList=((blockStatement*)body)->returnStatements();
      list<statement*> ::iterator itr;
      list<statement*> ::iterator itr_cont;
     
           itr = stmtList.begin();
          for(itr=std::next(itr,index);itr!=stmtList.end();itr++)
          {
            statement* stmt=*itr;
            if(stmt->getTypeofNode()==NODE_FORALLSTMT)
            {
              
              make_decl_par = false;
              //mp = NULL;
              //printf("CHECK IF INSIDE FOR ");//%d\n",stmt->getParent()->getParent()->getTypeofNode()==NODE_FORALLSTMT);
              generateStatement(stmt);
              //make_parallel = false;
              //Closing brace for filter  
              if(forAll->hasFilterExpr())
                  main.pushstr_newL("}");
              main.pushstr_newL("}"); //Closing of for-all
              cout <<"Comm needed value inside forall all graph iteration"<<comm_needed<<endl;
              if(comm_needed)
              {
                generate_sendCall(body);
                Identifier* iterator = ((forallStmt*)stmt)->getIterator();
                generate_addMessage(stmt,2,NULL,NULL);
              }
              itr++;
              itr_cont = itr;
              //mp = NULL;
              //make_decl_par = false;
              break;
            }
            else 
              generateStatement(stmt);

          }
      
      if(itr_cont!=stmtList.end())
      {
        generateForAllSignature(forAll);
        cout<< "This one!!!\n";
        for(itr=itr_cont;itr!=stmtList.end();itr++)
        {
          statement* stmt=*itr;
          generateStatement(stmt);
        }
        main.pushstr_newL("}"); //Closing the forall all graph itertion second part
	
        if(red_details!=NULL)
        {
          if(red_details->contains_reduction())
          {
            int op = red_details->get_reductionOp();
            Identifier* rv = red_details->get_reductionVar();
            Type* redVarType;
            if(rv->getSymbolInfo()->getType() != NULL)
              redVarType = rv->getSymbolInfo()->getType();
            //cout<<"reduction var contains type infor\n";
            if(op == 16)  //Add assign
            {
              sprintf(strBuffer,"%s = all_reduce(world, %s, std::plus<%s>());",rv->getIdentifier(),rv->getIdentifier(),convertToCppType(redVarType));
              main.pushstr_newL(strBuffer);
            }
          }
        }
        
      }
      if(neighbour_check)
      {
        sprintf(strBuffer, "for (int %s = startv; %s <= endv; %s++)",iterator->getIdentifier(),iterator->getIdentifier(),iterator->getIdentifier() );
        main.pushstr_newL(strBuffer);
        is_an_edge = true;
        generateStatement((statement*)forAll->getBody());
      }
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
    //main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
    //main.pushstr_newL("gettimeofday(&start, NULL);");
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

       // sprintf(strBuffer,"for (%s %s = %s.%s[%s]; %s < %s.%s[%s+1]; %s ++) ","int","edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge");
       if(comm_needed_gbl) {
        //sprintf(strBuffer,"int local_v = v - startv;");
        //main.pushstr_newL(strBuffer);
        sprintf(strBuffer,"for (int j = local_index[%s - startv]; j < local_index[%s - startv + 1]; j++)",nodeNbr->getIdentifier(),nodeNbr->getIdentifier());
        main.pushstr_newL(strBuffer);
       } else {
        sprintf(strBuffer,"for (%s %s = %s.%s[%s]; %s < %s.%s[%s+1]; %s ++) ","int","edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge",graphId,"indexofNodes",nodeNbr->getIdentifier(),"edge");
        main.pushstr_newL(strBuffer);
       }
       main.pushstr_newL("{");
       if(comm_needed_gbl) 
       {
        //sprintf(strBuffer,"int begin = local_index[0];");
        //main.pushstr_newL(strBuffer);
        //sprintf(strBuffer,"%s %s = local_edgeList[local_index[local_v]-begin+j];", "int",iterator->getIdentifier());
        //main.pushstr_newL(strBuffer);
        sprintf(strBuffer,"%s %s = local_edgeList[j];", "int",iterator->getIdentifier());
        main.pushstr_newL(strBuffer);
       } 
       else 
       {
        sprintf(strBuffer,"%s %s = %s.%s[%s] ;","int",iterator->getIdentifier(),graphId,"edgeList","edge"); //needs to move the addition of
        main.pushstr_newL(strBuffer);
       }
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


//***************Function to translate for loop..Can be merged with function to generate forAll*********//
void mpi_cpp_generator::generateFor(forallStmt* forAll)
{ 
  cout<<"Reaced generate For\n";
  //make_parallel = false;
  Identifier* sourceField;
   PropAccess* sourceField1;   
   Identifier* extractId;    
    Identifier* iteratorMethodId;

   Identifier* iterator=forAll->getIterator();
   if(!(forAll->isSourceField()))
   {
      sourceField = forAll->getSource();
   }

   statement* body=forAll->getBody();
     char strBuffer[1024];
  

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
          comm_needed = communication_needed((blockStatement*)body,iterator);
          //*******************If for loop present inside iterate in bfs************//
          if(forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS)
          {
            cout<<"parent itrbfs"<<endl;

            generateForSignature(forAll);

            sprintf(strBuffer,"if(%s >= startv && %s <= endv)",iterator->getIdentifier(),iterator->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
              sprintf(strBuffer,"if (d[%s-startv] < 0)",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"active_next.push_back(%s);",iterator->getIdentifier());
                main.pushstr_newL(strBuffer);
                sprintf(strBuffer,"d[%s-startv] = d[%s-startv] + 1;",iterator->getIdentifier(),nodeNbr->getIdentifier());
                main.pushstr_newL(strBuffer);
              main.pushstr_newL("}");
              sprintf(strBuffer,"if (d[%s-startv] == d[%s-startv] + 1)",iterator->getIdentifier(),nodeNbr->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.insert_indent();
                main.pushstr_newL("{");
                  sprintf(strBuffer,"p[%s-startv].push_back(%s);",iterator->getIdentifier(),nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  cout<< "Here123456\n";
                  generateBlock((blockStatement*)body,false);
                main.pushstr_newL("}");
            //Close of if part
            main.pushstr_newL("}");

            //Remote part
            main.pushstr_newL("else");
            main.pushstr_newL("{");
              sprintf(strBuffer,"dest_pro = %s / part_size;",iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"send_data[dest_pro].push_back(d[%s-startv]);",nodeNbr->getIdentifier());
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
            all_to_all_types.push_back(SEND_DATA);
          }
           
          //*******************If for loop present inside iterate in reverse bfs************//
          else if(forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
          {
            cout<<"parent itrbfs"<<endl;
                sprintf(strBuffer,"for(int %s=startv; %s<=endv; %s++)",nodeNbr->getIdentifier(),nodeNbr->getIdentifier(),nodeNbr->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            //sprintf(strBuffer,"int %s = levelNodes[phase][l] ;",bfsAbstraction->getIteratorNode()->getIdentifier());
            //main->pushstr_newL(strBuffer);
            sprintf(strBuffer,"if(d[%s-startv] == phase)",nodeNbr->getIdentifier());
            main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                sprintf(strBuffer,"modified[%s-startv] = true;",nodeNbr->getIdentifier());
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
            sprintf(strBuffer, "if(d[%s - startv] == d[%s - startv] + 1)",iterator->getIdentifier(),nodeNbr->getIdentifier()) ; 
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            main.insert_indent();
                 generateBlock((blockStatement*)body,false);
            main.decrease_indent();
            main.pushstr_newL("}");
            
            if(forAll->hasFilterExpr())
            {
                main.pushstr_newL("}");
            }
            //Close of if part
            main.pushstr_newL("}");
            main.pushstr_newL("}"); //Neighbor iteration signature
            main.pushstr_newL("}");

            //Else part
            sprintf(strBuffer,"if(d[%s-startv] == (phase+1))",nodeNbr->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
              main.pushstr_newL("for (int j=0;j<p[v-startv].size();j++)");
              main.pushstr_newL("{");
                sprintf(strBuffer,"int w = p[%s-startv][j];",nodeNbr->getIdentifier());
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

          all_to_all_types.push_back(SEND_DATA);
          
           main.pushstr_newL("}");
             generate_sendCall(body);
             generate_receiveCallBFS(forAll,2,nodeNbr);

           
          }
        }
        else if(allGraphIteration(iteratorMethodId->getIdentifier()))
        {
          if (body != NULL)
            mp = new make_par();
            //make_parallel = true;
        }
        else
        {
          string method(iteratorMethodId->getIdentifier());
          if(method == "nodes_to")
          {
            cout<<"IT IS IN NEIGHBOR ITERATION\n";
            comm_needed = communication_needed((blockStatement*)body,iterator);
            cout<<"Val of comm needed "<<comm_needed<<endl;
            char* graphId=sourceGraph->getIdentifier();
            char* methodId=iteratorMethodId->getIdentifier();
            list<argument*>  argList=extractElemFunc->getArgList();
            assert(argList.size()==1);
            Identifier* nodeNbr=argList.front()->getExpr()->getId();
            if(comm_needed_gbl) {
              sprintf(strBuffer,"for (%s %s = local_rev_index[%s-startv]; %s < local_rev_index[%s-startv+1]; %s++) ","int","edge",nodeNbr->getIdentifier(),"edge",nodeNbr->getIdentifier(),"edge");
            } else {
              sprintf(strBuffer,"for (%s %s = %s.%s[%s]; %s < %s.%s[%s+1]; %s++) ","int","edge",graphId,"rev_indexofNodes",nodeNbr->getIdentifier(),"edge",graphId,"rev_indexofNodes",nodeNbr->getIdentifier(),"edge");
            }
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            if(comm_needed_gbl)
              sprintf(strBuffer,"%s %s = %s[%s] ;","int",iterator->getIdentifier(),"local_srcList","edge"); //needs to move the addition of
            else
              sprintf(strBuffer,"%s %s = %s.%s[%s] ;","int",iterator->getIdentifier(),graphId,"srcList","edge"); //needs to move the addition of
            main.pushstr_newL(strBuffer);
              sprintf(strBuffer,"if (%s >= startv && %s <= endv)",iterator->getIdentifier(),iterator->getIdentifier());
              main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                generateBlock((blockStatement*)body,false);
              main.pushstr_newL("}");
            main.pushstr_newL("}");
            if(comm_needed_gbl) {
              sprintf(strBuffer,"for (%s %s = local_index[%s-startv]; %s < local_index[%s-startv+1]; %s++) ","int","edge1",nodeNbr->getIdentifier(),"edge1",nodeNbr->getIdentifier(),"edge1");
            } else {
              sprintf(strBuffer,"for (int edge1 = %s.indexofNodes[%s]; edge1 < %s.indexofNodes[%s+1]; edge1++)",graphId,nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
            }
            main.pushstr_newL(strBuffer);
              main.pushstr_newL("{");
                if(comm_needed_gbl)
                  sprintf(strBuffer,"int nbr = local_edgeList[edge1];");
                else
                  sprintf(strBuffer,"int nbr = %s.edgeList[edge1];",graphId);
                main.pushstr_newL(strBuffer);
                main.pushstr_newL("if(!(nbr >= startv && nbr <= endv))");
                main.pushstr_newL("{");
                  main.pushstr_newL("dest_pro = nbr / part_size;");
                  sprintf(strBuffer,"send_data[dest_pro].push_back(%s);",nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  num_messages++;
                  sprintf(strBuffer,"send_data[dest_pro].push_back(%s);","nbr");
                  main.pushstr_newL(strBuffer);
                  num_messages++;
                  // send_data[dest_pro].push_back(local_index[v-startv]);
                  sprintf(strBuffer,"send_data[dest_pro].push_back(local_index[%s-startv]);",nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  num_messages++;
                  // send_data[dest_pro].push_back(local_index[v-startv+1]);
                  sprintf(strBuffer,"send_data[dest_pro].push_back(local_index[%s-startv+1]);",nodeNbr->getIdentifier());
                  main.pushstr_newL(strBuffer);
                  num_messages++;
                  nodes_to = true;
                  generate_addMessage((blockStatement*)body,1,iterator,nodeNbr);
                  nodes_to = false;
                  //generate_addMessage((blockStatement*)body,1,nodeNbr, iterator);
                main.pushstr_newL("}");
              main.pushstr_newL("}");
           // main.pushstr_newL("}");
           all_to_all_types.push_back(SEND_DATA);

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

//Generation of do-while
void mpi_cpp_generator:: generateDoWhileStmt(dowhileStmt* stmt)
{
   char strBuffer[1024];
   Expression* iterCond = stmt->getCondition();
   statement* body = stmt->getBody();
   main.pushstr_newL("do");
   main.pushstr_newL("{");
      main.pushstr_newL("vector < vector <int> > send_data(np);");
      main.pushstr_newL("vector < vector <int> > receive_data(np);");
      main.pushstr_newL("vector < vector <float> > send_data_float(np);");
      main.pushstr_newL("vector < vector <float> > receive_data_float(np);");
      main.pushstr_newL("vector < vector <double> > send_data_double(np);");
      main.pushstr_newL("vector < vector <double> > receive_data_double(np);");
      main.insert_indent();
      generateBlock((blockStatement*)body,false);
      
      /*
      if(red_details!=NULL)
      {
	      if(red_details->contains_reduction())
	      {
		      int op = red_details->get_reductionOp();
		      Identifier* rv = red_details->get_reductionVar();
		      Type* redVarType;
		      if(rv->getSymbolInfo()->getType() != NULL)
			      redVarType = rv->getSymbolInfo()->getType();
		      //cout<<"reduction var contains type infor\n";
		      if(op == 16)  //Add assign
		      {
			      sprintf(strBuffer,"%s = all_reduce(world, %s, std::plus<%s>());",rv->getIdentifier(),rv->getIdentifier(),convertToCppType(redVarType));
			      main.pushstr_newL(strBuffer);
		      }
	      }
      }
      */
   main.pushstr_newL("send_data.clear();");
   main.pushstr_newL("vector <vector <int> >().swap(send_data);");
   main.pushstr_newL("receive_data.clear();");
   main.pushstr_newL("vector <vector <int> >().swap(receive_data);");
   main.pushstr_newL("send_data_float.clear();");
   main.pushstr_newL("vector <vector <float> >().swap(send_data_float);");
   main.pushstr_newL("receive_data_float.clear();");
   main.pushstr_newL("vector <vector <float> >().swap(receive_data_float);");
   main.pushstr_newL("send_data_double.clear();");
   main.pushstr_newL("vector <vector <double> >().swap(send_data_double);");
   main.pushstr_newL("receive_data_double.clear();");
   main.pushstr_newL("vector <vector <double> >().swap(receive_data_double);");
    
    main.decrease_indent();
    main.pushstr_space("}");

      main.pushstr_space("while (");
      generateExpr(iterCond);
      main.pushstr_newL(");");
}

void mpi_cpp_generator:: generateVariableDecl(declaration* declStmt)
{
   char strBuffer[1024];
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
     
     if(mp==NULL || make_decl_par == false)
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
     else
     {
       if(make_decl_par)
       {
          sprintf(strBuffer,"%s* %s = new %s[part_size];",convertToCppType(type),declStmt->getdeclId()->getIdentifier(),convertToCppType(type),graphId[0]->getIdentifier());
          main.pushstr_newL(strBuffer);
          sprintf(strBuffer,"for(int i0=0;i0<part_size;i0++)",graphId[0]->getIdentifier());
          main.pushstr_newL(strBuffer);
          main.insert_indent();
          mp->set_par(true,declStmt->getdeclId());
          //parallel_identifier = declStmt->getdeclId();
          //main.pushstr_space(convertToCppType(type));
          sprintf(strBuffer,"%s[i0]", declStmt->getdeclId()->getIdentifier());
          main.pushString(strBuffer);
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
          main.decrease_indent();
       }
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
            
             case EXPR_FLOATCONSTANT:
                sprintf(valBuffer,"%lf",expr->getFloatConstant());
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
      case OPERATOR_ADDASSIGN:
        return "+=";
      case OPERATOR_ANDASSIGN:
        return "&=";
      case OPERATOR_MULASSIGN:
        return "*=";
      case OPERATOR_ORASSIGN:
        return "|=";
      case OPERATOR_SUBASSIGN:
        return "-=";
      case OPERATOR_INC:
        return "++";
      case OPERATOR_DEC:
        return "--";
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
  char strBuffer[1024];
  if(mp!=NULL)
  {
    if(mp->check_makeParallel())
    {
      if(strcmp(mp->get_parId()->getIdentifier(),id->getIdentifier())==0)
      {
        main.pushString(id->getIdentifier());
        assert(mp->get_Iter()!=NULL);
        if(comm_needed_gbl)
          sprintf(strBuffer,"[%s-startv]",mp->get_Iter()->getIdentifier());
        else
          sprintf(strBuffer,"[%s]",mp->get_Iter()->getIdentifier());
        main.pushstr_space(strBuffer);
      }
      else
        main.pushString(id->getIdentifier());
    }
  }
  else
    main.pushString(id->getIdentifier());
cout<<"Exit generate expr id\n";

}

//**************Function to translate expressions*****************//
void mpi_cpp_generator::generateExpr(Expression* expr)
{ 
  cout<<"inside generate Expr\n";
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
       else if(expr->isUnary())
       {
         generate_exprUnary(expr);
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

void mpi_cpp_generator::generate_exprUnary(Expression* expr)
{
  cout<<"Generate Unary expr\n";
  char strBuffer[1024];
  if(expr->getUnaryExpr()==NULL)
    cout<<"unary expr infois null\n";
  Expression* unaryExpr = expr->getUnaryExpr();  
  int op = expr->getOperatorType();
  if(expr->getUnaryExpr()->isIdentifierExpr())
    main.pushString(expr->getUnaryExpr()->getId()->getIdentifier());
  const char* operatorString = getOperatorString(op);
  main.pushString(operatorString);
  main.pushstr_newL(";");

}

void mpi_cpp_generator::generateCheckIfNeighbor()
{
  if(!is_an_edge)
  {
    header.pushstr_newL("bool check_if_nbr(int s, int d, int* indexofNodes,int* edgeList, int startv)");
    header.pushstr_newL("{");
    header.insert_indent();
      header.pushstr_newL("int startEdge=indexofNodes[s - startv];");
      header.pushstr_newL("int endEdge=indexofNodes[s - startv + 1]-1;");


      header.pushstr_newL("if(edgeList[startEdge]==d)");
      header.insert_indent();
          header.pushstr_newL("return true;");
      header.decrease_indent();
      header.pushstr_newL("if(edgeList[endEdge]==d)");
      header.insert_indent();
         header.pushstr_newL("return true;");
      header.decrease_indent();   

      header.pushstr_newL("int mid = (startEdge+endEdge)/2;");

      header.pushstr_newL("while(startEdge<=endEdge)");
      header.pushstr_newL("{");
      header.insert_indent(); 
          header.pushstr_newL("if(edgeList[mid]==d)");
          header.insert_indent();
             header.pushstr_newL("return true;");
          header.decrease_indent();

          header.pushstr_newL("if(d<edgeList[mid])");
          header.insert_indent();
             header.pushstr_newL("endEdge=mid-1;");
          header.decrease_indent();
          header.pushstr_newL("else");
          header.insert_indent();
            header.pushstr_newL("startEdge=mid+1;");
          header.decrease_indent();  
          
          header.pushstr_newL("mid = (startEdge+endEdge)/2;");
      header.decrease_indent();
      header.pushstr_newL("}");
      
      header.pushstr_newL("return false;");
      header.decrease_indent();

    header.pushstr_newL("}");
  }
  else
  {
    header.pushstr_newL("bool check_if_nbr_new(int s, int d, vector <int>& s_List, vector <int>& d_List)");
    header.pushstr_newL("{");
    header.insert_indent();
      header.pushstr_newL("int startEdge=0;");
      header.pushstr_newL("int endEdge=s_List.size() - 1;");


      header.pushstr_newL("if(s_List[startEdge]==d)");
      header.insert_indent();
          header.pushstr_newL("return true;");
      header.decrease_indent();
      header.pushstr_newL("if(s_List[endEdge]==d)");
      header.insert_indent();
         header.pushstr_newL("return true;");
      header.decrease_indent();   

      header.pushstr_newL("int mid = (startEdge+endEdge)/2;");

      header.pushstr_newL("while(startEdge<=endEdge)");
      header.pushstr_newL("{");
      header.insert_indent(); 
          header.pushstr_newL("if(s_List[mid]==d)");
          header.insert_indent();
             header.pushstr_newL("return true;");
          header.decrease_indent();

          header.pushstr_newL("if(d<s_List[mid])");
          header.insert_indent();
             header.pushstr_newL("endEdge=mid-1;");
          header.decrease_indent();
          header.pushstr_newL("else");
          header.insert_indent();
            header.pushstr_newL("startEdge=mid+1;");
          header.decrease_indent();  
          
          header.pushstr_newL("mid = (startEdge+endEdge)/2;");
      header.decrease_indent();
      header.pushstr_newL("}");
      
      header.pushstr_newL("return false;");
      header.decrease_indent();

    header.pushstr_newL("}");
  }
}

void mpi_cpp_generator::generate_exprProcCall(Expression* expr)
{
  char strBuffer[2048];
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  if(methodId=="get_edge")
  {
    main.pushString("edge + startv"); //To be changed..need to check for a neighbour iteration 
                             // and then replace by the iterator.
  }
  else if(methodId=="num_nodes")
  {
    //Identifier* id1 = proc->getId1();
    //main.pushString(id1->getIdentifier());
    main.pushString("_actual_num_nodes"); //To be changed..need to check for a neighbour iteration 
                             // and then replace by the iterator.
  }
  else if(methodId == "count_inNbrs")
  {
    Identifier* id1 = proc->getId1();
    Identifier* arg;
    list<argument*> argList = proc->getArgList();
    list<argument*> :: iterator itr;
    if(argList.size() == 1)
    {
      itr = argList.begin();
      if((*itr)->isExpr())
        {
          if((*itr)->getExpr()->isIdentifierExpr())
            arg = (*itr)->getExpr()->getId();
        }
    }
    if(comm_needed_gbl) {
      sprintf(strBuffer,"(local_index[%s-startv+1]-local_index[%s-startv])",arg->getIdentifier(), arg->getIdentifier());
    } else {
      sprintf(strBuffer,"(%s.indexofNodes[%s+1]-%s.indexofNodes[%s])",id1->getIdentifier(),arg->getIdentifier(),id1->getIdentifier(),arg->getIdentifier());
    }
    main.pushString(strBuffer);

  }
  else if(methodId == "is_an_edge")
  {
    Identifier* id1 = proc->getId1();
    Identifier* arg;
    list<argument*> argList = proc->getArgList();
    list<argument*> :: iterator itr;
    if(!is_an_edge)
    {
        if(comm_needed_gbl)
          sprintf(strBuffer,"check_if_nbr(");
        else
          sprintf(strBuffer,"%s.check_if_nbr(",id1->getIdentifier());
          
        main.pushString(strBuffer);
        int argumentTotal = argList.size();
        int arg_currNo=0;
        int maximum_arginline = 4;
        for(itr=argList.begin();itr!=argList.end();itr++)
        {
          arg_currNo++;
          argumentTotal--;

          if((*itr)->isExpr())
          {
              if((*itr)->getExpr()->isIdentifierExpr())
                arg = (*itr)->getExpr()->getId();
                main.pushString(arg->getIdentifier());
          }
          
          if(argumentTotal>0)
            main.pushstr_space(",");

          if(arg_currNo==maximum_arginline)
          {
            main.NewLine();  
            arg_currNo=0;  
          } 
        // if(argumentTotal==0)
            
        }
        if(comm_needed_gbl)
          main.pushString(", local_index, local_edgeList, startv");
        main.pushString(")");

        if(comm_needed_gbl)
          generateCheckIfNeighbor();
    }
    else
    {   
        vector <Identifier*> ids;
        int argumentTotal = argList.size();
        int arg_currNo=0;
        int maximum_arginline = 4;
        for(itr=argList.begin();itr!=argList.end();itr++)
        {
          arg_currNo++;
          argumentTotal--;

          if((*itr)->isExpr())
          {
              if((*itr)->getExpr()->isIdentifierExpr())
                arg = (*itr)->getExpr()->getId();
                ids.push_back(arg);
          }

          if(arg_currNo==maximum_arginline)
          {
            main.NewLine();  
            arg_currNo=0;  
          } 
        }
        assert(ids.size() == 2);
        Identifier* id1 = ids[0];
        Identifier* id2 = ids[1];
        sprintf(strBuffer,"(%s_list.size() > 0 && check_if_nbr_new(%s,%s,%s_list,%s_list)) || (%s_list.size() > 0 && check_if_nbr_new(%s,%s,%s_list,%s_list))", id1->getIdentifier(),id1->getIdentifier(),id2->getIdentifier(),id1->getIdentifier(),id2->getIdentifier(),id2->getIdentifier(),id2->getIdentifier(),id1->getIdentifier(),id2->getIdentifier(),id1->getIdentifier());
        main.pushstr_space(strBuffer);
        

        if(comm_needed_gbl)
          generateCheckIfNeighbor();      
    }
  }

}

//***************Function to translate property id***************//
void mpi_cpp_generator::generate_exprPropId(PropAccess* propId) //This needs to be made more generic.
{ char strBuffer[1024];
  //PropAccess* propId=(PropAccess*)expr->getPropId();
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  // DONE see if comm_needed_gbl true.. then based on that do this
  if(comm_needed_gbl) {
    sprintf(strBuffer,"%s[%s-startv]",id2->getIdentifier(),id1->getIdentifier());
  } else {
    sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());
  }
  main.pushString(strBuffer);
}

//***************Function to translate property id***************//
void mpi_cpp_generator::generate_exprPropId_Pull(PropAccess* propId, Identifier* replace_id) //This needs to be made more generic.
{ char strBuffer[1024];
  //PropAccess* propId=(PropAccess*)expr->getPropId();
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  // DONE see if comm_needed_gbl true.. then based on that do this
  if(comm_needed_gbl) {
    sprintf(strBuffer,"%s[%s-startv]",id2->getIdentifier(),replace_id->getIdentifier());
  } else {
    sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());
  }
  main.pushString(strBuffer);
}

//******************Function to translate property id while receiving**************//
void mpi_cpp_generator::generate_exprPropIdReceive(PropAccess* propId) //This needs to be made more generic.
{ char strBuffer[1024];
  //PropAccess* propId=(PropAccess*)expr->getPropId();
  Identifier* id1=propId->getIdentifier1();
  Identifier* id2=propId->getIdentifier2();
  sprintf(strBuffer,"%s_%s",id2->getIdentifier(),id1->getIdentifier());
  main.pushString(strBuffer);

}

//**************Function to translate fixedPoint construct***************//
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
            sprintf(strBuffer,"if (%s[i - startv] == 1)",dependentId->getIdentifier());
            header.pushstr_newL(strBuffer);
            header.insert_indent();
            header.pushstr_newL("res=1;");
            header.decrease_indent();
          header.pushstr_newL("}");
          header.pushstr_newL("MPI_Allreduce(&res, &sum, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD);");
          header.pushstr_newL("if(sum ==0) return false;");
          header.pushstr_newL("else return true;");
          header.pushstr_newL("}");
      }
    }
  }
  main.pushstr_newL("int num_iter=0;");
  //main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
  //main.pushstr_newL("gettimeofday(&start, NULL);");
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
  main.pushstr_newL("vector < map<int,float> > send_data_float(np);");
  main.pushstr_newL("vector < map<int,float> > receive_data_float(np);");
  main.pushstr_newL("vector < map<int,double> > send_data_double(np);");
  main.pushstr_newL("vector < map<int,double> > receive_data_double(np);");
  main.pushstr_newL("std::map<int,int>::iterator itr;");
 // main.pushstr_newL("int dest_pro;");
  if(fixedPointConstruct->getBody()->getTypeofNode()!=NODE_BLOCKSTMT)
    generateStatement(fixedPointConstruct->getBody());
  else
    generateBlock((blockStatement*)fixedPointConstruct->getBody(),false);
  
  assert(convergeExpr->getExpressionFamily()==EXPR_UNARY||convergeExpr->getExpressionFamily()==EXPR_ID);

     main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
     main.pushstr_newL("send_data.clear();");
     main.pushstr_newL("vector < map<int,int> >().swap(send_data);");
     main.pushstr_newL("receive_data.clear();");
     main.pushstr_newL("vector < map<int,int> >().swap(receive_data);");
     main.pushstr_newL("send_data_float.clear();");
     main.pushstr_newL("vector < map<int,float> >().swap(send_data_float);");
    main.pushstr_newL("receive_data_float.clear();");
    main.pushstr_newL("vector < map<int,float> >().swap(receive_data_float);");
    main.pushstr_newL("send_data_double.clear();");
    main.pushstr_newL("vector < map<int,double> >().swap(send_data_double);");
    main.pushstr_newL("receive_data_double.clear();");
    main.pushstr_newL("vector < map<int,double> >().swap(receive_data_double);");
     main.pushstr_newL("}");
}


//************Function to translate statements inside a block***************//
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

//*********Generate initial and final part of function*************//
void mpi_cpp_generator::generateFunc(ASTNode* proc)
{  
    char strBuffer[1024];
    Function* func=(Function*)proc;
    generateFuncHeader(func,false);
    generateFuncHeader(func,true);
    main.pushstr_newL("{");
    //main.insert_indent();

    /* 
    TODO check if communication is needed 
    then based on that do a conditional code generation

    check not done SSSP code?
    */
    blockStatement* blockStmt = func->getBlockStatement();
    list<statement*> stmtList = blockStmt->returnStatements();
    list<statement*> ::iterator itr;
    bool comm_needed = false;
    for(itr=stmtList.begin();itr!=stmtList.end();itr++)
    {
      statement* stmt=*itr;
      if(stmt->getTypeofNode()==NODE_FORALLSTMT)
      {
        forallStmt* f = (forallStmt*) stmt;
        statement* body = f->getBody();
        Identifier* iterator = f->getIterator();
        comm_needed = (comm_needed || communication_needed((blockStatement*)body, iterator));
      }
      if(stmt->getTypeofNode()==NODE_DOWHILESTMT)
      {
        dowhileStmt* f = (dowhileStmt*) stmt;
        blockStatement* bodytemp = (blockStatement*)f->getBody();
        list<statement*> stmtList2 = bodytemp->returnStatements();
        list<statement*> ::iterator itr2;
        for(itr2=stmtList2.begin();itr2!=stmtList2.end();itr2++)
        {
          statement* stmt2=*itr2;
          if(stmt2->getTypeofNode()==NODE_FORALLSTMT)
          {
            forallStmt* f = (forallStmt*) stmt2;
            statement* body = f->getBody();
            Identifier* iterator = f->getIterator();
            comm_needed = (comm_needed || communication_needed((blockStatement*)body, iterator));
          }
        }
      }
      if(stmt->getTypeofNode() == NODE_FIXEDPTSTMT)
      {
        fixedPointStmt* f = (fixedPointStmt*) stmt;
        statement* body = f->getBody();
        comm_needed = true;
      }
      if(stmt->getTypeofNode() == NODE_FORALLSTMT)
        comm_needed = true;
    }
    comm_needed_gbl = comm_needed;
    if(comm_needed) {
      // generate code snippet for communication and graph split
      main.pushstr_newL("int my_rank,np,part_size,startv,endv;");
      main.pushstr_newL("struct timeval start, end, start1, end1;");
      main.pushstr_newL("long seconds,micros;");
      main.pushstr_newL("mpi::communicator world;");
      main.pushstr_newL("my_rank = world.rank();");
      main.pushstr_newL("np = world.size();");

      main.pushstr_newL("int *index,*rev_index, *all_weight,*edgeList, *srcList;");
      main.pushstr_newL("int *local_index,*local_rev_index, *weight,*local_edgeList, *local_srcList;");
      main.pushstr_newL("int _num_nodes, _actual_num_nodes;");
      main.pushstr_newL("int dest_pro;");

      main.pushstr_newL("if(my_rank == 0)");
      main.pushstr_newL("{");
      main.pushstr_newL("gettimeofday(&start, NULL);");
      // main.pushstr_newL("g.parseGraph();");
      sprintf(strBuffer,"%s.parseGraph();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);

      sprintf(strBuffer,"_num_nodes = %s.num_nodes();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"_actual_num_nodes = %s.ori_num_nodes();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"all_weight = %s.getEdgeLen();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"edgeList = %s.getEdgeList();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"srcList = %s.getSrcList();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"index = %s.getIndexofNodes();",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"rev_index = %s.rev_indexofNodes;",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"part_size = %s.num_nodes()/np;",graphId[0]->getIdentifier());
      main.pushstr_newL(strBuffer);

      main.pushstr_newL("MPI_Bcast (&_num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);");
      main.pushstr_newL("MPI_Bcast (&_actual_num_nodes,1,MPI_INT,my_rank,MPI_COMM_WORLD);");
      main.pushstr_newL("MPI_Bcast (&part_size,1,MPI_INT,my_rank,MPI_COMM_WORLD);");
      main.pushstr_newL("local_index = new int[part_size+1];");
      main.pushstr_newL("local_rev_index = new int[part_size+1];");
          
      main.pushstr_newL("for(int i=0;i<part_size+1;i++) {");
          main.pushstr_newL("local_index[i] = index[i];");
          main.pushstr_newL("local_rev_index[i] = rev_index[i];");
      main.pushstr_newL("}");
      main.pushstr_newL("int num_ele = local_index[part_size]-local_index[0];");
      main.pushstr_newL("weight = new int[num_ele];");
      main.pushstr_newL("for(int i=0;i<num_ele;i++)");
          main.pushstr_newL("weight[i] = all_weight[i];");
      main.pushstr_newL("local_edgeList = new int[num_ele];");
      main.pushstr_newL("for(int i=0;i<num_ele;i++)");
          main.pushstr_newL("local_edgeList[i] = edgeList[i];");
      main.pushstr_newL("local_srcList = new int[num_ele];");
      main.pushstr_newL("for(int i=0;i<num_ele;i++)");
          main.pushstr_newL("local_srcList[i] = srcList[i];");
      main.pushstr_newL("for(int i=1;i<np;i++)");
      main.pushstr_newL("{");
          main.pushstr_newL("int pos = i*part_size;");
          main.pushstr_newL("MPI_Send (index+pos,part_size+1,MPI_INT,i,0,MPI_COMM_WORLD);");
          main.pushstr_newL("MPI_Send (rev_index+pos,part_size+1,MPI_INT,i,1,MPI_COMM_WORLD);");
          main.pushstr_newL("int start = index[pos];");
          main.pushstr_newL("int end = index[pos+part_size];");
          main.pushstr_newL("int count_int = end - start;");
          main.pushstr_newL("MPI_Send (all_weight+start,count_int,MPI_INT,i,2,MPI_COMM_WORLD);");
          main.pushstr_newL("MPI_Send (edgeList+start,count_int,MPI_INT,i,3,MPI_COMM_WORLD);");
          main.pushstr_newL("MPI_Send (srcList+start,count_int,MPI_INT,i,4,MPI_COMM_WORLD);");
      main.pushstr_newL("}");
      main.pushstr_newL("delete [] all_weight;");
      main.pushstr_newL("delete [] edgeList;");
      main.pushstr_newL("delete [] index;");
      main.pushstr_newL("delete [] rev_index;");
      main.pushstr_newL("delete [] srcList;");

       main.pushstr_newL("gettimeofday(&end, NULL);");
      main.pushstr_newL("seconds = (end.tv_sec - start.tv_sec);");
      main.pushstr_newL("micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);");
      main.pushstr_newL("printf(\"The graph loading & distribution time = %ld secs.\\n\",seconds);");
      main.pushstr_newL("}");

      main.pushstr_newL("else");
      main.pushstr_newL("{");
      main.pushstr_newL("MPI_Bcast (&_num_nodes,1,MPI_INT,0,MPI_COMM_WORLD); ");
      main.pushstr_newL("MPI_Bcast (&_actual_num_nodes,1,MPI_INT,0,MPI_COMM_WORLD); ");
      main.pushstr_newL("MPI_Bcast (&part_size,1,MPI_INT,0,MPI_COMM_WORLD);");

      main.pushstr_newL("local_index = new int[part_size+1];");
      main.pushstr_newL("MPI_Recv (local_index,part_size+1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);");
      main.pushstr_newL("local_rev_index = new int[part_size+1];");
      main.pushstr_newL("MPI_Recv (local_rev_index,part_size+1,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);");

      main.pushstr_newL("int num_ele = local_index[part_size]-local_index[0];");
      main.pushstr_newL("weight = new int[num_ele];");
      main.pushstr_newL("MPI_Recv (weight,num_ele,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);");
      main.pushstr_newL("local_edgeList = new int[num_ele];");
      main.pushstr_newL("MPI_Recv (local_edgeList,num_ele,MPI_INT,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);");
      main.pushstr_newL("local_srcList = new int[num_ele];");
      main.pushstr_newL("MPI_Recv (local_srcList,num_ele,MPI_INT,0,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);");
      main.pushstr_newL("int begin = local_index[0];");
      main.pushstr_newL("for (int i = 0; i < part_size+1; i++)");
      main.insert_indent();
    	main.pushstr_newL("local_index[i] = local_index[i] - begin;");
      main.decrease_indent();
      main.pushstr_newL("begin = local_rev_index[0];");
      main.pushstr_newL("for (int i = 0; i < part_size+1; i++)");
      main.insert_indent();
    	main.pushstr_newL("local_rev_index[i] = local_rev_index[i] - begin;");
      main.decrease_indent();
      main.pushstr_newL("}");
      main.pushstr_newL("startv = my_rank*part_size;");
      main.pushstr_newL("endv = startv+part_size-1;");
      main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
      main.pushstr_newL("gettimeofday(&start, NULL);");
      for (auto itr = function_argument_propid.begin(); itr != function_argument_propid.end(); ++itr) {
        sprintf(strBuffer,"%s = new %s[part_size];",const_cast<char*>(itr->first.c_str()), const_cast<char*>(itr->second.c_str()));
        main.pushstr_newL(strBuffer);
      }
    } else {
      // generate code snippet without graph split
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
      main.pushstr_newL("int dest_pro;");
      main.pushstr_newL("MPI_Barrier(MPI_COMM_WORLD);");
      main.pushstr_newL("gettimeofday(&start, NULL);");
    }
   
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

   //For Final Gather of result
   for (auto itr = function_argument_propid.begin(); itr != function_argument_propid.end(); ++itr) 
   {
        //sprintf(strBuffer,"%s = new %s[part_size];",const_cast<char*>(itr->first.c_str()), const_cast<char*>(itr->second.c_str()));
        //main.pushstr_newL(strBuffer);
        string var = "final_"+itr->first;
        string type = itr->second+"*";
        sprintf(strBuffer,"%s %s;",type.c_str(),var.c_str());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("if (my_rank == 0)");
        main.pushstr_newL("{");
          main.insert_indent();
          sprintf(strBuffer,"%s = new %s [_num_nodes];",var.c_str(), itr->second.c_str());
          main.pushstr_newL(strBuffer);
          sprintf(strBuffer,"gather(world, %s, part_size, %s, 0);",itr->first.c_str(), var.c_str());
          main.pushstr_newL(strBuffer);
          main.pushstr_newL("for (int t = 0; t < _actual_num_nodes; t++)");
          main.insert_indent();
            sprintf(strBuffer,"cout << \"%s[\" << t << \"] = \" << %s[t] << endl;",itr->first.c_str(), var.c_str());
            main.pushstr_newL(strBuffer);
          main.decrease_indent();
        main.decrease_indent();
        main.pushstr_newL("}");

        main.pushstr_newL("else");
        main.pushstr_newL("{");
        main.insert_indent();
          sprintf(strBuffer,"gather(world, %s, part_size, %s, 0);",itr->first.c_str(), var.c_str());
          main.pushstr_newL(strBuffer);
        main.decrease_indent();
        main.pushstr_newL("}");


   }
   main.pushstr_newL("MPI_Finalize();");
   main.push('}');
} 

const char* mpi_cpp_generator::convertToCppType(Type* type)
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
    if(convertTypeCheck) {
      int typeId=targetType->gettypeId();
      cout<<"TYPEID IN CPP"<<typeId<<"\n";
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
        default:
         assert(false);          
      }
    }
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
      string prop_type(convertToCppType(type));
      if(prop_type == "int*")
      {
        string arg_name((*itr)->getIdentifier()->getIdentifier());
        function_argument_propid.insert(make_pair(arg_name,"int"));
      }
      else if(prop_type == "float*")
      {
        string arg_name((*itr)->getIdentifier()->getIdentifier());
        function_argument_propid.insert(make_pair(arg_name,"float"));
      }
      else if(prop_type == "bool*")
      {
        string arg_name((*itr)->getIdentifier()->getIdentifier());
        function_argument_propid.insert(make_pair(arg_name,"bool"));
      }
      else if(prop_type == "double*")
      {
        string arg_name((*itr)->getIdentifier()->getIdentifier());
        function_argument_propid.insert(make_pair(arg_name,"double"));
      }
      else if(prop_type == "long*")
      {
        string arg_name((*itr)->getIdentifier()->getIdentifier());
        function_argument_propid.insert(make_pair(arg_name,"long"));
      }
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


//****************Opening files for storing generated code******************//
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

