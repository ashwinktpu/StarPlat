/*#include "dsl_cpp_generator.h"
#include<string.h>


void dsl_cpp_generator::generate()
{  
   openFileforOutput();

   generation_begin(); 

   generateFunc();

   generation_end();

   closeOutputFile();

}

void dsl_cpp_generator::addIncludeToFile(char* includeName,dslCodePad file,bool isCppLib)
{  
    if(!isCppLib)
      file.push('"');
    else 
      file.push('<');  
     
     file.push(includeName);
     if(!isCppLib)
       file.push('"');
     else 
       file.push('>');
     file.NewLine();     
 }

void dsl_cpp_generator::generation_begin()
{ 
  char temp[1024];  
  header.push("#ifndef GENCPP_");
  header.pushUpper(fileName);
  header.push("_H");
  header.push("define GENCPP_");
  header.push("_H");
  header.NewLine();
  addIncludeToFile("stdio.h",header,true);
  addIncludeToFile("stdlib.h",header,true);
  addIncludeToFile("omp.h",header,true);
  header.NewLine();
  sprintf(temp,"%s.h",fileName);
  addIncludeToFile(temp,main,false);
  main.NewLine();

}

void generateStatement(statement* stmt)
{
   if(stmt->getTypeofNode()==NODE_BLOCKSTMT)
     {
       generateBlock((blockStatement*)stmt);
     }
   if(stmt->getTypeofNode()==NODE_DECL)
   {
     
      generateVariableDecl((declaration*)stmt);

   } 
   if(stmt->getTypeofNode==NODE_ASSIGN)
     {
       generateAssignmentStmt((assignment*)stmt);
     }
    
   if(stmt->getTypeofNode==NODE_WHILESTMT) 
   {
     generateWhileStmt((whileStmt*) stmt);
   }
   
   if(stmt->getTypeofNode==NODE_IFSTMT)
   {
      generateIfStmt((ifStmt*)stmt);
   }

   if(stmt->getTypeofNode==NODE_DOWHILESTMT)
   {
      generateBlock((blockStatement*) stmt);
   }

    if(stmt->getTypeofNode==NODE_FORALLSTMT)
     {
       generateForAll(forallStmt*) stmt);
     }
  
    if(stmt->getTypeofNode==NODE_FIXEDPTSTMT)
    {
      generateFixedPoint((fixedPointStmt*)stmt);
    }
    if(stmt->getTypeofNode==NODE_REDUCTIONCALLSTMT)
    {
      generateReductionStmt((reductionCallStmt*) stmt);
    }
    if(stmt->getTypeofNode==NODE_ITRBFS)
    {
      generateBFSAbstraction((iterateBFS*) stmt);
    }
    if(stmt->getTypeofNode==NODE_PROCCALLSTMT)
    {
      generateProcCall((proc_callStmt*) stmt);
    }


}

void dsl_cpp_generator::findTargetGraph(vector<Identifier*> graphTypes,Type* type)
{   
    if(graphTypes.size()==1)
    {
      cerr<<"TargetGraph can't match";
    }
    else
    {
      Identifier* graphId=graphTypes[0];
      type->setTargetGraph(graphId);
    }
    
    
}

void dsl_cpp_generator::getValueTobeAssigned(Expression* expr)
     {
        if(expr->get)
      

     }
void dsl_cpp_generator::generateProcCall(proc_callStmt* proc_callStmt)
{
   proc_callExpr* procedure=proc_callStmt->getProcCallExpr();
   if(procedure->getMethodId()=="attachNodeProperty")
       {  char strBuffer[1024];
          list<argument*> argList=procedure->getArgList();
          list<argument*>::iterator itr;
          main.push("#pragma omp parallel for");
          sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int","t","t",procedure->getId1(),"num_nodes");
          main.push("{");
          for(itr=argList.begin();itr!=argList.end();itr++)
              { 
                assignment* assign=(*itr)->assignExpr;
                Identifier* lhsId=assign->getId();
                Expression* exprAssigned=assign->getExpr();
                char* valueTobeAssigned=getValueTobeAssigned(exprAssigned);
                sprintf(strBuffer,"%s[%s] = %s",lhsId->getIdentifier(),"t",valueTobeAssigned);
              }
              


       }
}

void dsl_cpp_generator::generatePropertyDefination(Type* type,char* Id)
{ 
  Type* targetType=type->getInnerTargetType();
  if(targetType->gettypeId==TYPE_INT)
  {
     main.push("=");
     main.push(INTALLOCATION);
     main.push("[");
     findTargetGraph(graphTypes,type);
     char strBuffer[100];
     sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     main.push("]");
     main.push(";");
  }
  if(targetType->gettypeId==TYPE_BOOL)
  {
     main.push("=");
     main.push(BOOLALLOCATION);
     main.push("[");
     findTargetGraph(graphTypes,type);
     char strBuffer[100];
     sprintf(strBuffer,"%s.%s()",type->getTargetGraph()->getIdentifier(),"num_nodes");
     main.push("]");
     main.push(";");
  }

     


}

void dsl_cpp_generator:: generateVariableDecl(declaration* declStmt)
{
   Type* type=declStmt->getType();
   if(type->isPropType())
   {
     if(type->getInnerTargetType()->isPrimitiveType())
     { 
       Type* innerType=type->getInnerTargetType();
       main.push(convertToCppType(innerType));
       main.space();
       main.push(declStmt->getdeclId()->getIdentifier());
       generatePropertyDefination(type,declStmt->getdeclId()->getIdentifier());
     }
   }



}

void dsl_cpp_generator::generateBlock(blockStatement* blockStmt)
{
   list<statement*> stmtList=blockStmt->returnStatements();
   list<statement*> ::iterator itr;
   main.push("{");
   for(itr=stmtList.begin();itr!=stmtList.end();itr++)
   {
     statement* stmt=*itr;
     generateStatement(stmt);

   }
   main.push("}");
}
void dsl_cpp_generator::generateFunc(Function* proc)
{
   generateProcCall(proc,false);
   generateProcCall(proc,true);
   generateStatement(proc->getBlockStatement());
   main.NewLine();

   return;

}

void generateProcCall(Function* proc,bool isMainFile)
{
  dslCodePad targetFile=isMainFile?main:Header;
  targetFile.push(proc->getIdentifier()->getIdentifier());
  targetFile.push('(');
  
  int maximum_arginline=2;
  int arg_currNo=0;
  int argumentTotal=proc->getParamList().size();
  list<formalParam*> paramList=proc->getParamList();
  list<formalParam*>::iterator itr;
  for(itr=paramList.begin();itr!=paramList.end();itr++)
  {
      arg_currNo++;
      argumentTotal--;

      Type* type=(*itr)->getType();
      targetFile.push(convertToCppType(type));
      if(type->isPrimitive()||type->isProperty()
      {
          targetFile.push(" ");
      }
      else 
          targetFile.push("&");

      targetFile.push(createParamName((*itr)->getIdentifier()->getIdentifier());
      if(argumentTotal>0)
         targetFile.push(",");

      if(arg_currNo==maximum_arginline||argumentTotal==0)
      {
         targetFile.NewLine();  
         arg_currNo=0;  
      } 
  }

   targetFile.push(")");
   if(!isMainFile)
       targetFile.push(";");
    targetFile.NewLine();

    return;   
        
}
*/
