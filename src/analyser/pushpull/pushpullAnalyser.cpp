#include "pushpullAnalyser.h"
#include <string.h>
#include <list>
#include "../../backends/backend_multigpu/getUsedVars.h"

int pushpullAnalyser::analyseforinfor(forallStmt* forstmt,Identifier* ownvertex){
    cout << "   for" << endl;
    blockStatement *forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    int ans = 1;
    for (statement *stmt : forbodystmts){
        int pushorpull = analyseStatementinForAll(stmt, ownvertex);
        if (pushorpull == 0)
        {
            ans = 0;
       }
    }
    return ans;
}

int pushpullAnalyser::analyseassigninfor(assignment *stmt, Identifier *ownvertex)
{
    cout << "   assign" << endl;
    if (stmt->lhs_isProp())
    {
        PropAccess *leftprop = stmt->getPropId();
        Identifier *affectedId = leftprop->getIdentifier1();
        cout<<"                               ";
        cout<<ownvertex->getIdentifier()<<" "<<leftprop->getIdentifier2()->getIdentifier()<<" "<<affectedId->getIdentifier()<<" ";
        if (strcmp(ownvertex->getIdentifier(), affectedId->getIdentifier()) != 0)
        {
            push_map[string(stmt->getPropId()->getIdentifier2()->getIdentifier())]=1;
            return 0;
        }
    }
    return 1;
}

int pushpullAnalyser::analyseifinfor(ifStmt *ifstmt, Identifier *ownvertex)
{
    int ans = 1;
    cout << " if" << endl;
    blockStatement *ifbody = (blockStatement *)ifstmt->getIfBody();
    int t = analyseStatementinForAll(ifstmt->getIfBody(), ownvertex);
    
    if(t==0){
        ans=0;
    }
    blockStatement *elsebody = (blockStatement *)ifstmt->getElseBody();
    if(elsebody==NULL){
        return ans;
    }
    cout << " else" << endl;
    int l = analyseStatementinForAll(ifstmt->getElseBody(),ownvertex);
    if(l==0){
        ans=0;
    }
    return ans;
}
int pushpullAnalyser::analysereductioninfor(reductionCallStmt* stmt, Identifier *ownvertex){
    cout << "   reduction" << endl;
    list<ASTNode *> leftlist = stmt->getLeftList();
    int ans =1 ;
    for(ASTNode* stmt : leftlist){
        if(stmt->getTypeofNode()==NODE_PROPACCESS){
             Identifier *affectedId = ((PropAccess*)stmt)->getIdentifier1();
             if (strcmp(ownvertex->getIdentifier(), affectedId->getIdentifier()) != 0)
             {
                 PropAccess* stmt1 = (PropAccess*) stmt;
                 push_map[string(stmt1->getIdentifier2()->getIdentifier())] = 1;
                 ans =0;
             }
        }
    }
    PropAccess* leftprop = stmt->getPropAccess();
    if(leftprop!=NULL){
        Identifier *affectedId = leftprop->getIdentifier1();
        if (strcmp(ownvertex->getIdentifier(), affectedId->getIdentifier()) != 0)
        {
             push_map[string(leftprop->getIdentifier2()->getIdentifier())] = 1;
             ans =0;
        }
    }
    return ans;
}

int pushpullAnalyser::analyseexprinfor(unary_stmt* input,Identifier *ownvertex){
    cout << " expression" << endl;
    Expression* stmt = input->getUnaryExpr();
    while (stmt->isUnary())
    {
        stmt = stmt->getUnaryExpr();
    }
    if(stmt->isPropIdExpr()){
        PropAccess *propaccess = stmt->getPropId();
        Identifier *affectedId = propaccess->getIdentifier1();
        if (strcmp(ownvertex->getIdentifier(), affectedId->getIdentifier()) != 0)
        {
             push_map[string(propaccess->getIdentifier2()->getIdentifier())] = 1;
             return 0;
        }
    }
        return 1;
}

int pushpullAnalyser::analyseBlockinfor(blockStatement* bstmt,Identifier* ownvertex){
        list<statement *> currStmts = bstmt->returnStatements();
        int ans =1;
        for (statement *bstmt : currStmts)
        {
           int pp = analyseStatementinForAll(bstmt,ownvertex);
           if(pp=0){
            ans=0;
           }
        }
        return ans;
}


int pushpullAnalyser::analyseStatementinForAll(statement* stmt,Identifier *ownvertex){
        switch (stmt->getTypeofNode())
        {
            case NODE_BLOCKSTMT:
            {
                return analyseBlockinfor((blockStatement*)stmt, ownvertex);
            }
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
            case NODE_UNARYSTMT:
            {
                return analyseexprinfor((unary_stmt*)stmt,ownvertex);
            }
            case NODE_IFSTMT:
            {
                return analyseifinfor((ifStmt*)stmt, ownvertex);
            }
    }
    return 1;
}

int pushpullAnalyser::analyseForAll(forallStmt *forstmt)
{
    blockStatement *forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    Identifier *ownvertex = forstmt->getIterator();
    int ans =1;
    for (statement *stmt : forbodystmts)
    {
        int pushorpull = analyseStatementinForAll(stmt,ownvertex);
        if (pushorpull == 0)
        {
            ans =0;
        }
    }
    return ans;
}


void pushpullAnalyser::analyseStatement(statement *stmt)
{
    // printf("pushpullAnalysing %p %d\n", stmt, stmt->getTypeofNode());
    switch (stmt->getTypeofNode())
    {
    case NODE_FIXEDPTSTMT:
       {
        cout<<"fixedpoint"<<endl;
        fixedPointStmt *fixedstmt = (fixedPointStmt*)stmt;
        allGpuUsedVars.insert(string(fixedstmt->getFixedPointId()->getIdentifier()));
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
            multigpu::usedVariables usedVars = multigpu::getVarsForAll(forStmt);
            list<Identifier *> vars = usedVars.getVariables();
            for (Identifier *iden : vars)
            {
                allGpuUsedVars.insert(string(iden->getIdentifier()));
            }
            int pushorpull = analyseForAll((forallStmt *)stmt);
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