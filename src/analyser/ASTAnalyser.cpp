#include "ASTAnalyser.h"
#include <string.h>
#include <cassert>

void ASTAnalyser::analyseStatement(statement* stmt)
{  
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
      analyseBlock((blockStatement*) stmt);
      break;
    
    case NODE_WHILESTMT:
      analyseStatement(((whileStmt*) stmt)->getBody());
      break;

    case NODE_DOWHILESTMT:
      analyseStatement(((dowhileStmt*) stmt)->getBody());
      break;

    case NODE_FIXEDPTSTMT:
      analyseStatement(((fixedPointStmt*) stmt)->getBody());
      break;

    case NODE_IFSTMT:
      statement* ifBody = ((ifStmt*) stmt)->getIfBody();
      statement* elseBody = ((ifStmt*) stmt)->getElseBody();
      if(ifBody != NULL) analyseStatement(ifBody);
      if(elseBody != NULL) analyseStatement(elseBody);
      break;

    case NODE_ITRBFS:
      analyseStatement(((iterateBFS*) stmt)->getBody());
      iterateReverseBFS* revBFS = ((iterateBFS*) stmt)->getRBFS();
      if(revBFS != NULL) analyseStatement(revBFS->getBody());
      break;

    case NODE_FORALLSTMT:
      analyseStatement(((forallStmt*) stmt)->getBody());
      break;
    }
}

void ASTAnalyser::analyseBlock(blockStatement *blockStmt)
{
    list<statement *> stmtList = blockStmt->returnStatements();
    list<statement *>::iterator itr;

    list<statement *> nodePropList, nodeAttachList;
    list<statement *> toRemoveStmt;

    for (itr = stmtList.begin(); itr != stmtList.end(); itr++)
    {
        statement *stmt = *itr;

        switch (stmt->getTypeofNode())
        {
        case NODE_PROCCALLSTMT:
        {
            proc_callExpr *procedure = proc_callStmt->getProcCallExpr();
            string methodID(procedure->getMethodId()->getIdentifier());
            if (methodID == "attachNodeProperty")
            {
                nodeAttachList.push_back(stmt);
                toRemoveStmt.push_back(stmt);
            }
        }
        break;

        case NODE_DECL:
        {
            Type *type = declStmt->getType();

            if (type->isPropType())
            {
                nodePropList.push_back(stmt);
                toRemoveStmt.push_back(stmt);
            }
        }
        break;

        default:
            analyseStatement(stmt);
    }

    for(statement* stmt :  toRemoveStmt){
        blockStmt->removeStatement(stmt);
    }

    //Assuming attachNodeProperty call is made to a single graph
    if(nodeAttachList.size() > 0)
    {
        itr = nodeAttachList.begin();
        proc_callStmt* proc_callStmt = *itr;
        proc_callExpr* procedure=proc_callStmt->getProcCallExpr();

        while(++itr != nodeAttachList.end())
        {
            proc_callExpr* nxtProcedure=(*itr)->getProcCallExpr();
            for(argument* arg : nxtProcedure->getArgList())
                procedure->addToArgList(arg);
        }

        blockStmt->addToFront(proc_callStmt);
    }

    for(statement* stmt: nodePropList){
        blockStmt->addToFront(stmt);
    }
}
void ASTAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseBlock(func->getBlockStatement());
    return;
}

void ASTAnalyser::analyse()
{
    list<Function *> funcList = frontEndContext.getFuncList();
    for (Function *func : funcList){
        analyseFunc(func);
    }
}
