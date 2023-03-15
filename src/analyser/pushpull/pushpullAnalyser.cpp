#include "pushpullAnalyser.h"
#include <string.h>
#include <list>


int pushpullAnalyser::analyseforinfor(forallStmt* forstmt,Identifier* ownvertex){
    blockStatement *forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    for (statement *stmt : forbodystmts){
       int pushorpull = analyseStatementinForAll(stmt,ownvertex);
       if(pushorpull==0){
        return 0;
       }
    }
    return 1;
}

int pushpullAnalyser::analyseassigninfor(assignment *stmt, Identifier *ownvertex)
{
    if (stmt->lhs_isProp())
    {
        PropAccess *leftprop = stmt->getPropId();
        Identifier *affectedId = leftprop->getIdentifier1();
        if (strcmp(ownvertex->getIdentifier(), affectedId->getIdentifier()) != 0)
        {
             return 0;
        }
    }
    return 1;
}

int pushpullAnalyser::analysereductioninfor(reductionCallStmt* stmt, Identifier *ownvertex){
    list<ASTNode *> leftlist = stmt->getLeftList();
    for(ASTNode* stmt : leftlist){
        if(stmt->getTypeofNode()==NODE_PROPACCESS){
             Identifier *affectedId = ((PropAccess*)stmt)->getIdentifier1();
             if (strcmp(ownvertex->getIdentifier(), affectedId->getIdentifier()) != 0)
             {
                 return 0;
             }
        }
    }

    return 1;
}
int pushpullAnalyser::analyseStatementinForAll(statement* stmt,Identifier *ownvertex){
    switch(stmt->getTypeofNode()){
        case NODE_FORALLSTMT:
        {
            return analyseforinfor((forallStmt *)stmt, ownvertex);
        }
        case NODE_ASSIGN:
        {
            return analyseassigninfor((assignment*)stmt,ownvertex);
        }
        case NODE_REDUCTIONCALLSTMT:
        {
            return analysereductioninfor((reductionCallStmt*)stmt, ownvertex);
        }
    }
    return 1;
}

int pushpullAnalyser::analyseForAll(forallStmt *forstmt)
{
    blockStatement *forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    Identifier *ownvertex = forstmt->getIterator();
    for (statement *stmt : forbodystmts)
    {
        int pushorpull = analyseStatementinForAll(stmt,ownvertex);
        if (pushorpull == 0)
        {
            return 0;
        }
    }
    return 1;
}


void pushpullAnalyser::analyseStatement(statement *stmt)
{
    // printf("pushpullAnalysing %p %d\n", stmt, stmt->getTypeofNode());
    switch (stmt->getTypeofNode())
    {
    case NODE_FIXEDPTSTMT:
       {
        fixedPointStmt *fixedstmt = (fixedPointStmt*)stmt;
        analyseBlock((blockStatement *)fixedstmt->getBody());
        break;
       }
    case NODE_DOWHILESTMT:
       {
        dowhileStmt *dowhilestmt = (dowhileStmt*)stmt;
        analyseBlock((blockStatement *)dowhilestmt->getBody());
        break;
       }
    case NODE_FORALLSTMT:
        {
            forallStmt *forStmt = (forallStmt *)stmt;
            if (forStmt->isForall()){
                int pushorpull = analyseForAll((forallStmt *)stmt);
                if(pushorpull==1){
                    cout<<"                       The kernel is PULL type                    "<<endl;
                }
                else{
                    cout<<"                       The kernel is PUSH type                    "<<endl;
                }
            }
            else analyseBlock((blockStatement *)forStmt->getBody());
            break;
        }
    case NODE_BLOCKSTMT:
        analyseBlock((blockStatement *)stmt);
        break;
    default:
        return;
    }
    return;
}

void pushpullAnalyser::analyseBlock(blockStatement *stmt)
{
    list<statement *> currStmts = stmt->returnStatements();
    for (statement *bstmt : currStmts)
    {
        analyseStatement(bstmt);
    }
    return;
}

void pushpullAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseStatement(func->getBlockStatement());
    return;
}

void pushpullAnalyser::analyse(list<Function *> funcList)
{
    for (Function *func : funcList)
        analyseFunc(func);
}