#include "cudaMemcpyAnalyser.h"
#include "../../backends/backend_cuda/getUsedVars.h"
#include <string.h>
#include <list>

void cudaMemcpyAnalyser::analysedowhile(dowhileStmt *dowhile, list<Identifier *> &primvars)
{
    usedVariables usedvarswhile = getVarsDoWhile(dowhile);
    list<Identifier*> currlist;
    for (Identifier *iden : primvars)
    {
        Type *type = iden->getSymbolInfo()->getType();
        if (type->isPrimitiveType() && !usedvarswhile.isUsedVar(iden, 2))
        {
            dowhile->insert_vars_cudamemcpy(iden);
            currlist.push_back(iden);
        }
    }
    for(Identifier* iden:currlist){
        primvars.remove(iden);
    }
    
}

void cudaMemcpyAnalyser::analysefixedpoint(fixedPointStmt *fixedPointConstruct, list<Identifier *> &primvars)
{

    usedVariables usedvarsfixed = getVarsFixedPoint(fixedPointConstruct);
    list<Identifier*> currlist;
    for (Identifier *iden : primvars)
    {
        Type *type = iden->getSymbolInfo()->getType();
        if (type->isPrimitiveType() && !usedvarsfixed.isUsedVar(iden, 2))
        {
            // generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
            fixedPointConstruct->insert_vars_cudamemcpy(iden);
            currlist.push_back(iden);
        }
    }
    for (Identifier *iden : currlist)
    {
        primvars.remove(iden);
    }
}

void cudaMemcpyAnalyser::analyseForAll(forallStmt *forAll)
{
    usedVariables usedVars = getVarsForAll(forAll);
    list<Identifier *> vars = usedVars.getVariables(1);
    list<Identifier*> primvars;
    for (Identifier *iden : vars)
    {
        Type *type = iden->getSymbolInfo()->getType();
        if (type->isPrimitiveType())
            // generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
            primvars.push_back(iden);
    }

   

    list<ASTNode*> parents;
    ASTNode* currstmt = forAll;
    while(currstmt->getTypeofNode()!=NODE_FUNC){
        if (currstmt->getTypeofNode() == NODE_WHILESTMT || currstmt->getTypeofNode() == NODE_DOWHILESTMT || currstmt->getTypeofNode() == NODE_FIXEDPTSTMT){
            parents.push_front(currstmt);
        }
        currstmt = currstmt->getParent();
    }
   
    for(ASTNode* currnode:parents){
        if(currnode->getTypeofNode()==NODE_DOWHILESTMT){
            analysedowhile((dowhileStmt*)currnode,primvars);
            
        }
        if (currnode->getTypeofNode() == NODE_FIXEDPTSTMT)
        {
            analysefixedpoint((fixedPointStmt *)currnode, primvars);
        }
    }
    for(Identifier* iden:primvars){
        forAll->insert_vars_cudamemcpy(iden);
    }

    
    list<Identifier *> writevars = usedVars.getVariables(2);
    for (Identifier *iden : writevars)
    {
        if (usedVars.isUsedVar(iden, 1))
        {
            continue;
        }
        Type *type = iden->getSymbolInfo()->getType();
        if (type->isPrimitiveType()){
            forAll->insert_vars_cudamemcpy(iden);
        }
            
    }
}


void cudaMemcpyAnalyser::analyseStatement(statement *stmt)
{
    // printf("cudaMemcpyAnalysing %p %d\n", stmt, stmt->getTypeofNode());
    switch (stmt->getTypeofNode())
    {
    case NODE_FIXEDPTSTMT:
       {
        cout<<"fixedpoint"<<endl;
        fixedPointStmt *fixedstmt = (fixedPointStmt*)stmt;
        analyseBlock((blockStatement *)fixedstmt->getBody());
        break;
       }
    case NODE_DOWHILESTMT:
       {
        cout << "dowhile" << endl;
        dowhileStmt *dowhilestmt = (dowhileStmt*)stmt;
        analyseBlock((blockStatement *)dowhilestmt->getBody());
        break;
       }
    case NODE_FORALLSTMT:
        {
            cout << "forall" << endl;
            forallStmt *forStmt = (forallStmt *)stmt;
            if (forStmt->isForall())
            {
               analyseForAll((forallStmt *)stmt);
            }
            else analyseBlock((blockStatement *)forStmt->getBody());
            break;
        }
    case NODE_BLOCKSTMT:
            cout << "block" << endl;
            analyseBlock((blockStatement *)stmt);
            break;
    default:
        return;
    }
    return;
}

void cudaMemcpyAnalyser::analyseBlock(blockStatement *stmt)
{
    list<statement *> currStmts = stmt->returnStatements();
    for (statement *bstmt : currStmts)
    {
        analyseStatement(bstmt);
    }
    return;
}

void cudaMemcpyAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseStatement(func->getBlockStatement());
    return;
}

void cudaMemcpyAnalyser::analyse(list<Function *> funcList)
{
    cout<<endl;
    cout<<"CUDAMEMCPYANALYSER:"<<endl;
    for (Function *func : funcList)
        analyseFunc(func);
}