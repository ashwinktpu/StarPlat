#include "readExpressionAnalyser.h"
#include "../../backends/backend_cuda/getUsedVars.h"
#include "../deviceVars/deviceVarsAnalyser.h"
#include <string.h>
#include <list>

int tempVar=0;
char *getTempVar()
{
    string var = "tempVar_" + to_string(tempVar++);
    int len = var.length();
    char *s = new char[len + 1];
    strcpy(s, var.c_str());
    return s;
}

void readExpressionAnalyser::analyseforinfor(forallStmt *forstmt, forallStmt *parent)
{
    cout << "   for" << endl;
    blockStatement *forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    for (statement *stmt : forbodystmts){
        cout<<" ";
        analyseStatementinForAll(stmt,parent);
    }
}

int readExpressionAnalyser::analyseexprinfor(Expression *expr, forallStmt *parent)
{
    cout << "   expression" << endl;
    
    if (expr->isLiteral())
    {
        expr->setreadexpr();
        return 1;
    }
    else if (expr->isInfinity())
    {
        expr->setreadexpr();
        return 1;
    }
    else if (expr->isIdentifierExpr())
    {
        //  std::cout<< "------>PROP ID"  << '\n';
       usedVariables usedVars = getVarsForAll(parent);
       list<Identifier *> vars = usedVars.getVariables(1);
       list<Identifier *> write_vars = usedVars.getVariables(2);
       for (Identifier *iden : vars)
       {
           if (usedVars.isUsedVar(iden, 2))
           {
               continue;
           }
           if (strcmp(iden->getIdentifier(), expr->getId()->getIdentifier()) == 0){
               expr->setreadexpr();
               return 1;
           }
       }
       return 0;
    }
    else if (expr->isPropIdExpr())
    {
        //  std::cout<< "------>PROP EXP"  << '\n';
        return 0;
    }
    else if (expr->isArithmetic() || expr->isLogical()|| expr->isRelational())
    {
        // std::cout<< "------>PROP AR/LG"  << '\n';
        int left = analyseexprinfor(expr->getLeft(),parent);
        if(left==1){
           expr->getLeft()->setreadexpr();
        }
        int right= analyseexprinfor(expr->getRight(), parent);
        if(right==1){
           expr->getRight()->setreadexpr();
        }
        if(left&right){
           expr->setreadexpr();
        }
        return left&right;
    }
    else if (expr->isProcCallExpr())
    {
        // std::cout << "------>PROP PRO CAL" << '\n';
       return 0;
    }
    else if (expr->isUnary())
    {
        // std::cout << "------>PROP UNARY" << '\n';
       return 0;
    }
    else
    {
        assert(false);
    }
}

void readExpressionAnalyser::analyseexprinfor_second(Expression* expr, forallStmt* parent,Type* type){
    cout << "   expression2" << " ";
    cout <<expr->isreadexprs()<<endl;
    if (!expr->isIdentifierExpr() && !expr->isLiteral() && expr->isreadexprs() && !expr->isInfinity())
    {
        Identifier *newid = (Identifier*)(Util::createIdentifierNode(getTempVar()));
        expr->setnewidassigned(newid);
        declaration* newdeclnode = (declaration*)(Util::createAssignedDeclNode(type,newid,expr));
        newdeclnode->setInGPU(true);
        parent->addreadexprdecls(newdeclnode);
        
    }
     if (expr->isArithmetic() || expr->isLogical() || expr->isRelational())
    { 
        analyseexprinfor_second(expr->getLeft(), parent,type);
        analyseexprinfor_second(expr->getRight(), parent,type);
    }
}
void readExpressionAnalyser::analyseassigninfor(assignment *stmt, forallStmt *parent)
{
    cout << "   assign" << endl;
    Expression* rhs = stmt->getExpr();
    Type* type;
    if(stmt->lhs_isIdentifier()){
        type = (stmt->getId())->getSymbolInfo()->getType();
    }
    else{
        type = ((stmt->getPropId())->getIdentifier2())->getSymbolInfo()->getType();
    }
    
    analyseexprinfor(rhs,parent);
    analyseexprinfor_second(rhs,parent,type);
}

void readExpressionAnalyser::analysedeclarationinfor(declaration *stmt, forallStmt *parent)
{
    cout << "   declaration" << endl;
    Expression *rhs = stmt->getExpressionAssigned();
    Type* type = stmt->getType();
    analyseexprinfor(rhs,parent);
    analyseexprinfor_second(rhs, parent,type);
}
void readExpressionAnalyser::analyseifinfor(ifStmt *ifstmt, forallStmt *parent)
{
    
    cout << "   if" << endl;
    blockStatement *ifbody = (blockStatement *)ifstmt->getIfBody();
    analyseStatementinForAll(ifstmt->getIfBody(),parent);
    
    
    blockStatement *elsebody = (blockStatement *)ifstmt->getElseBody();
    if(elsebody==NULL){
        return ;
    }
    cout << "   else" << endl;
    analyseStatementinForAll(ifstmt->getElseBody(),parent);
   
}

void readExpressionAnalyser::analysereductioninfor(reductionCallStmt *stmt, forallStmt *parent)
{
    cout << "   reduction" << endl;
    // list<ASTNode *> leftlist = stmt->getLeftList();
    // for(ASTNode* stmt : leftlist){
    //     if(stmt->getTypeofNode()==NODE_PROPACCESS){
    //          Identifier *affectedId = ((PropAccess*)stmt)->getIdentifier1();
           
    //     }
    // }
    // PropAccess* leftprop = stmt->getPropAccess();
    // if(leftprop!=NULL){
    //     Identifier *affectedId = leftprop->getIdentifier1();
       
    // }
}

void readExpressionAnalyser::analyseBlockinfor(blockStatement *bstmt, forallStmt *parent)
{
    cout << "   block" << endl;
    list<statement *> currStmts = bstmt->returnStatements();
    int ans = 1;
    for (statement *bstmt : currStmts)
    {
        analyseStatementinForAll(bstmt,parent);
    }
}

void readExpressionAnalyser::analyseStatementinForAll(statement *stmt, forallStmt *parent)
{
        switch (stmt->getTypeofNode())
        {
            case NODE_BLOCKSTMT:
            {
               analyseBlockinfor((blockStatement*)stmt,parent);
               break;
            }
            case NODE_FORALLSTMT:
            {
               analyseforinfor((forallStmt *)stmt, parent);
               break;
            }
            case NODE_ASSIGN:
            {
               analyseassigninfor((assignment *)stmt, parent);
               break;
            }
            case NODE_DECL:
            {
                analysedeclarationinfor((declaration *)stmt,parent);
                break;
            }
            case NODE_REDUCTIONCALLSTMT:
            {
               analysereductioninfor((reductionCallStmt *)stmt, parent);
               break;
            }
            case NODE_IFSTMT:
            {
               analyseifinfor((ifStmt *)stmt, parent);
               break;
            }
    }
}

void readExpressionAnalyser::analyseForAll(forallStmt *forstmt)
{
    blockStatement* forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    Identifier *ownvertex = forstmt->getIterator();
    int ans =1;
    
    for (statement *stmt : forbodystmts)
    {
        analyseStatementinForAll(stmt,forstmt);
    }
  
}


void readExpressionAnalyser::analyseStatement(statement *stmt)
{
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

void readExpressionAnalyser::analyseBlock(blockStatement *stmt)
{
    list<statement *> currStmts = stmt->returnStatements();
    for (statement *bstmt : currStmts)
    {
        analyseStatement(bstmt);
    }
    return;
}

void readExpressionAnalyser::analyseFunc(ASTNode *proc)
{
    Function *func = (Function *)proc;
    analyseStatement(func->getBlockStatement());
    return;
}

void readExpressionAnalyser::analyse(list<Function *> funcList)
{
    for (Function *func : funcList){
        cout<<endl<<"READ EXPRESSION ANALYSER"<<endl;
        analyseFunc(func);
    }
        
}