#include "dsl_dyn_cpp_generator.h"

namespace spdynmpi{

    void dsl_dyn_cpp_generator::generateOnDeleteBlock(onDeleteBlock* onDeleteStmt)
    {
        insideParallelConstruct.push_back(onDeleteStmt);
        char strBuffer[1024];
        Identifier* updatesId = onDeleteStmt->getUpdateId();
        proc_callExpr* updatesFunc = onDeleteStmt->getUpdateFunc();
        string methodId(updatesFunc->getMethodId()->getIdentifier());

        if(methodId == "currentBatch")
        {   
            sprintf(strBuffer, "for(Update u : %s->%s()->%s())",updatesId->getIdentifier(),"getCurrentDeleteBatch","getUpdates");
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            generateBlock(onDeleteStmt->getStatements(),false);
            main.NewLine();
            main.pushstr_newL("}");
        }
        insideParallelConstruct.pop_back();
    }
    
    void dsl_dyn_cpp_generator::generateOnAddBlock(onAddBlock* onAddStmt)
    {
        insideParallelConstruct.push_back(onAddStmt);
        char strBuffer[1024];
        Identifier* updatesId = onAddStmt->getUpdateId();
        proc_callExpr* updatesFunc = onAddStmt->getUpdateFunc();
        string methodId(updatesFunc->getMethodId()->getIdentifier());

        if(methodId == "currentBatch")
        {   
            sprintf(strBuffer, "for(Update u : %s->%s()->%s())",updatesId->getIdentifier(),"getCurrentAddBatch","getUpdates");
            main.pushstr_newL(strBuffer);
            main.pushstr_newL("{");
            generateBlock(onAddStmt->getStatements(),false);
            main.NewLine();
            main.pushstr_newL("}");
        }
        insideParallelConstruct.pop_back();
    }
    
    void dsl_dyn_cpp_generator::generateBatchBlock(batchBlock* batchStmt)
    {
        
        Identifier* updateId = batchStmt->getUpdateId();
        char strBuffer[1024];
  
         if(updateId != NULL)
            {
                sprintf(strBuffer, "%s->%s(",updateId->getIdentifier(), "splitIntoSmallerBatches");
                main.pushString(strBuffer);
                generateExpr(batchStmt->getBatchSizeExpr());
                
                main.pushstr_newL(");");

                sprintf(strBuffer,"while(%s->%s())",updateId->getIdentifier(),"nextBatch");
                main.pushstr_newL(strBuffer);
                main.pushstr_newL("{");
                
                generateBlock(batchStmt->getStatements(),false);
                //std::cout<<"inside batch block51"<<std::endl;
                main.NewLine();
                main.pushstr_newL("}");
            }

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
        std::cout<<"here proc"<<std::endl;
        proc_callExpr* proc=(proc_callExpr*)expr;
        string methodId(proc->getMethodId()->getIdentifier());
        if(methodId == "updateCSRAdd" || methodId == "updateCSRDel") 
        {
            char strBuffer[1024];
            list<argument*> argList = proc->getArgList();
            assert(argList.size() == 1);
            Identifier* updateId = argList.front()->getExpr()->getId();
            Identifier* objectId=proc->getId1();
            assert(updateId->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES);
            
            if(methodId == "updateCSRAdd")
                sprintf(strBuffer,"%s->%s(&%s)",updateId->getIdentifier(),"updateCsrAdd",objectId->getIdentifier());
            else
                sprintf(strBuffer,"%s->%s(&%s)",updateId->getIdentifier(),"updateCsrDel",objectId->getIdentifier());
            main.pushString(strBuffer);

        }
        else if(methodId == "currentBatch")
        {
          char strBuffer[1024];
          list<argument*> argList = proc->getArgList();
          assert(argList.size() == 1);
          assert(argList.front()->getExpr()->getExpressionFamily() == EXPR_INTCONSTANT);
          int updateType = argList.front()->getExpr()->getIntegerConstant();

          if(updateType == 0)
             sprintf(strBuffer,"%s->%s()",proc->getId1()->getIdentifier(),"getCurrentDeleteBatch");
          else
             sprintf(strBuffer,"%s->%s()",proc->getId1()->getIdentifier(),"getCurrentAddBatch");
          main.pushString(strBuffer);   

        }   
        else if(methodId == "Incremental" || methodId == "Decremental") 
        {

           char strBuffer[1024];
           if(methodId == "Incremental")
               sprintf(strBuffer,"%s_add",fileName);
           if(methodId == "Decremental")
              sprintf(strBuffer, "%s_del", fileName);
           main.pushString(strBuffer);
           main.pushString("(");
           generateArgList(proc->getArgList());
           main.pushString(", world)");         

        } 
        else
        {     
            dsl_cpp_generator::generate_exprProcCall(expr);
        }
    }

    void dsl_dyn_cpp_generator::generateInDecHeader(Function* inDecFunc, bool isMainFile)
    {
 
        char temp[1024];
        dslCodePad& targetFile = isMainFile ? main : header;

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
  
        targetFile.pushString(")");
        if(!isMainFile)
            targetFile.pushString(";");
        targetFile.NewLine();

        return; 
    }

    void dsl_dyn_cpp_generator::generateDynamicHeader(Function* dynFunc, bool isMainFile )
    {
        char temp[1024];
        dslCodePad& targetFile = isMainFile ? main : header;
        sprintf(temp,"%s",dynFunc->getIdentifier()->getIdentifier());
  
        if(dynFunc->containsReturn()) 
            targetFile.pushString("auto ");
        else
            targetFile.pushString("void ");
        
        targetFile.pushString(temp);
        targetFile.push('(');

        generateParamList(dynFunc->getParamList(), targetFile);

        targetFile.pushString(")");
        if(!isMainFile)
            targetFile.pushString(";");
        targetFile.NewLine();

        return; 


    }

    void dsl_dyn_cpp_generator::generateIncremental(Function* incFunc)
    {
        char strBuffer[1024];
        curFuncType = incFunc->getFuncType();
        currentFunc = incFunc;
        generateInDecHeader(incFunc,true);
        generateInDecHeader(incFunc,false);
        main.pushstr_newL("{");
        
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
        generateInDecHeader(decFunc,true);
        generateInDecHeader(decFunc,false);

        main.pushstr_newL("{");

        generateBlock(decFunc->getBlockStatement(),false);
        main.NewLine();
        main.pushstr_newL("}");
        incFuncCount(decFunc->getFuncType());
        
        return;
    }

    void dsl_dyn_cpp_generator::generateDynamicFunc(Function* dynFunc)
    {
        std::cout<<"reached here in dynamic func"<<std::endl;
        char strBuffer[1024];
        curFuncType = dynFunc->getFuncType();
        currentFunc = dynFunc;
        generateDynamicHeader(dynFunc,true);
        generateDynamicHeader(dynFunc,false);

        main.pushstr_newL("{");

        generateBlock(dynFunc->getBlockStatement(),false);
        main.NewLine();
        main.pushstr_newL("}");
        incFuncCount(dynFunc->getFuncType());
        return;
    }

    void dsl_dyn_cpp_generator::generateFunc(ASTNode* func)
    {std::cout<<"reached dyn func"<<std::endl;

        Function* function = (Function*)func;
        if(function->getFuncType() == STATIC_FUNC ||function->getFuncType() == GEN_FUNC)
        {
            dsl_cpp_generator::generateFunc(function);
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

    void dsl_dyn_cpp_generator::generation_begin()
    {
        char temp[1024];
        
        header.pushString("#ifndef GENCPP_");
        header.pushUpper(fileName);
        
        header.pushstr_newL("_H");
        header.pushString("#define GENCPP_");
        header.pushUpper(fileName);
        header.pushstr_newL("_H");
        header.pushString("#include");
        addIncludeToFile("stdio.h", header, true);
        
        // Add other include files later if required
        header.pushString("#include");
        addIncludeToFile("stdlib.h", header, true);
        header.pushString("#include");
        addIncludeToFile("limits.h", header, true);
        header.pushString("#include");
        addIncludeToFile("atomic", header, true);
        header.pushString("#include");
        addIncludeToFile("set", header, true);
        header.pushString("#include");
        addIncludeToFile("vector", header, true);

        header.pushString("#include");
        addIncludeToFile("../mpi_header/graph_mpi.h", header, false);
        header.NewLine();
        main.pushString("#include");
        sprintf(temp, "%s_dyn.h", fileName);
        addIncludeToFile(temp, main, false);
        main.NewLine();header.pushString("#include");
        addIncludeToFile("../mpi_header/updates.h", header, false);
        header.NewLine();
    }
    bool dsl_dyn_cpp_generator::openFileforOutput()
    {  
        char temp[1024];
        printf("fileName %s\n", fileName);
        sprintf(temp, "%s/%s_dyn.h", "../graphcode/generated_mpi", fileName);
        headerFile = fopen(temp, "w");
        if (headerFile == NULL)
            return false;
        header.setOutputFile(headerFile);

        sprintf(temp, "%s/%s_dyn.cc", "../graphcode/generated_mpi", fileName);
        bodyFile = fopen(temp, "w");
        if (bodyFile == NULL)
            return false;
        main.setOutputFile(bodyFile);

        return true;

    }

}    