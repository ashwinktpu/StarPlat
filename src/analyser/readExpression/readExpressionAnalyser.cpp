#include "readExpressionAnalyser.h"
#include "../../backends/backend_cuda/getUsedVars.h"
#include "../deviceVars/deviceVarsAnalyser.h"
#include "../../backends/backend_cuda/dsl_cpp_generator.h"
#include <string.h>
#include <list>
#include <set>

map<Expression*,int> readonlyinparent ;

int tempVar = 0;
char *getTempVar()
{
    string var = "tempVar_" + to_string(tempVar++);
    int len = var.length();
    char *s = new char[len + 1];
    strcpy(s, var.c_str());
    return s;
}

void readExpressionAnalyser::analyseforBlock(forallStmt *forstmt, statement *parent)
{
    cout << "   for" << endl;
    blockStatement *forbody = (blockStatement *)forstmt->getBody();
    list<statement *> forbodystmts = forbody->returnStatements();
    for (statement *stmt : forbodystmts)
    {
        cout << " ";
        analyseStatementinBlock(stmt, parent);
    }
}



int readExpressionAnalyser::analyseexprinBlock(Expression *expr, statement *parent)
{
    cout << "   expression" << endl;
    if (expr == NULL)
    {
        return 0;
    }
    if (parent == NULL)
    {
        return 0;
    }
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
        usedVariables usedVars;
        if (parent->getTypeofNode() == NODE_FORALLSTMT)
        {
            usedVars = getVarsForAll((forallStmt *)parent);
        }
        if (parent->getTypeofNode() == NODE_DOWHILESTMT)
        {
            usedVars = getVarsDoWhile((dowhileStmt *)parent);
        }
        if (parent->getTypeofNode() == NODE_FIXEDPTSTMT)
        {
            usedVars = getVarsFixedPoint((fixedPointStmt *)parent);
        }

        list<Identifier *> vars = usedVars.getVariables(1);
        list<Identifier *> write_vars = usedVars.getVariables(2);
        for (Identifier *iden : vars)
        {
            if (usedVars.isUsedVar(iden, 2))
            {
                continue;
            }
            if (strcmp(iden->getIdentifier(), expr->getId()->getIdentifier()) == 0)
            {
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
    else if (expr->isArithmetic() || expr->isLogical() || expr->isRelational())
    {
        // std::cout<< "------>PROP AR/LG"  << '\n';

        int left = analyseexprinBlock(expr->getLeft(), parent);
        int right = analyseexprinBlock(expr->getRight(), parent);
        if (left && right)
        {
            expr->setreadexpr();
            expr->getLeft()->setwriteexpr();
            expr->getRight()->setwriteexpr();
        }
        return left && right;
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

void readExpressionAnalyser::analyseexprinBlock_second(Expression *expr, statement *parent, Type *type,statement* fatherparent)
{
    if (expr == NULL)
    {
        return;
    }
    if (parent == NULL)
    {
        return;
    }
    cout << "   expression2"<< " ";
    cout << expr->isreadexprs() << endl;
    if (!expr->isIdentifierExpr() && !expr->isLiteral() && expr->isreadexprs() && !expr->isInfinity())
    {
        if(readonlyinparent[expr]==1){
            return;
        }
        Identifier *newid = (Identifier *)(Util::createIdentifierNode(getTempVar()));
        expr->setnewidassigned(newid);
        declaration *newdeclnode = (declaration *)(Util::createAssignedDeclNode(type, newid, expr));
        readonlyinparent[expr]=1;
        if (parent->getTypeofNode() == NODE_FORALLSTMT)
        {
            ((forallStmt *)parent)->addreadexprdecls(newdeclnode);
        }
        if (parent->getTypeofNode() == NODE_DOWHILESTMT)
        {
            ((dowhileStmt *)parent)->addreadexprdecls(newdeclnode);
        }

        if (parent->getTypeofNode() == NODE_FIXEDPTSTMT)
        {
            ((fixedPointStmt *)parent)->addreadexprdecls(newdeclnode);
        }
        if(fatherparent->getTypeofNode()==NODE_FORALLSTMT){
            newdeclnode->setInGPU(true);
        }
    }
    else if (expr->isArithmetic() || expr->isLogical() || expr->isRelational())
    {
        analyseexprinBlock_second(expr->getLeft(), parent, type,fatherparent);
        analyseexprinBlock_second(expr->getRight(), parent, type,fatherparent);
    }
    else if (expr->isIdentifierExpr() || expr->isLiteral() || expr->isInfinity())
    {
        return;
    }
}
void readExpressionAnalyser::analyseassigninBlock(assignment *stmt, statement *parent)
{
    cout << "   assign" << endl;
    Expression *rhs = stmt->getExpr();
    Type *type;
    if (stmt->lhs_isIdentifier())
    {
        type = (stmt->getId())->getSymbolInfo()->getType();
    }
    else
    {
        type = ((stmt->getPropId())->getIdentifier2())->getSymbolInfo()->getType();
    }
    statement* fatherparent = (statement*)(stmt->getParent()->getParent());
    // cout<<endl;
    // cout<<fatherparent->getTypeofNode()<<endl;
    // cout << endl;
    analyseexprinBlock(rhs, parent);
    analyseexprinBlock_second(rhs, parent, type,fatherparent);
}

void readExpressionAnalyser::analysedeclarationinBlock(declaration *stmt, statement *parent)
{
    cout << "   declaration" << endl;
    Expression *rhs = stmt->getExpressionAssigned();
    Type *type = stmt->getType();
    statement *fatherparent = (statement *)(stmt->getParent()->getParent());
    analyseexprinBlock(rhs, parent);
    analyseexprinBlock_second(rhs, parent, type, fatherparent);
}
void readExpressionAnalyser::analyseifinBlock(ifStmt *ifstmt, statement *parent)
{

    cout << "   if" << endl;
    blockStatement *ifbody = (blockStatement *)ifstmt->getIfBody();
    analyseStatementinBlock(ifstmt->getIfBody(), parent);

    blockStatement *elsebody = (blockStatement *)ifstmt->getElseBody();
    if (elsebody == NULL)
    {
        return;
    }
    cout << "   else" << endl;
    analyseStatementinBlock(ifstmt->getElseBody(), parent);
}

void readExpressionAnalyser::analysereductioninBlock(reductionCallStmt *stmt, statement *parent)
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

void readExpressionAnalyser::analyseBlockfirst(blockStatement *stmt, statement *parent)
{
    list<statement *> currStmts = stmt->returnStatements();
    for (statement *bstmt : currStmts)
    {
        analyseStatementinBlockfirst(bstmt, parent);
    }
    return;
}

void readExpressionAnalyser::analyseStatementinBlockfirst(statement *stmt, statement *parent)
{
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
    {
        analyseBlock((blockStatement *)stmt, parent);
        break;
    }
    case NODE_FORALLSTMT:
    {
        cout << "forall" << endl;
        forallStmt *forAll = (forallStmt *)stmt;
        analyseBlockfirst((blockStatement *)forAll->getBody(), parent);
        break;
    }
    case NODE_ASSIGN:
    {
        analyseassigninBlock((assignment *)stmt, parent);
        break;
    }
    case NODE_DECL:
    {
        analysedeclarationinBlock((declaration *)stmt, parent);
        break;
    }
    case NODE_REDUCTIONCALLSTMT:
    {
        analysereductioninBlock((reductionCallStmt *)stmt, parent);
        break;
    }
    case NODE_IFSTMT:
    {
        analyseifinBlock((ifStmt *)stmt, parent);
        break;
    }
    case NODE_FIXEDPTSTMT:
    {
        cout << "fixedpoint" << endl;
        fixedPointStmt *fixedstmt = (fixedPointStmt *)stmt;
        analyseBlockfirst((blockStatement *)fixedstmt->getBody(), parent);
        break;
    }
    case NODE_DOWHILESTMT:
    {
        cout << "dowhile" << endl;
        dowhileStmt *dowhilestmt = (dowhileStmt *)stmt;
        analyseBlockfirst((blockStatement *)dowhilestmt->getBody(), parent);
        break;
    }
    }
}

void readExpressionAnalyser::analyseStatementinBlock(statement *stmt, statement *parent)
{
    if(parent==NULL){
        readonlyinparent.clear();
    }
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:
    {
        analyseBlock((blockStatement *)stmt, parent);
        break;
    }
    case NODE_FORALLSTMT:
    {
        cout << "forall" << endl;
        forallStmt* forAll = (forallStmt*)stmt;
        analyseBlockfirst((blockStatement *)forAll->getBody(), stmt);
        analyseBlock((blockStatement *)forAll->getBody(), stmt);
        break;
    }
    case NODE_ASSIGN:
    {
        analyseassigninBlock((assignment *)stmt, parent);
        break;
    }
    case NODE_DECL:
    {
        analysedeclarationinBlock((declaration *)stmt, parent);
        break;
    }
    case NODE_REDUCTIONCALLSTMT:
    {
        analysereductioninBlock((reductionCallStmt *)stmt, parent);
        break;
    }
    case NODE_IFSTMT:
    {
        analyseifinBlock((ifStmt *)stmt, parent);
        break;
    }
    case NODE_FIXEDPTSTMT:
    {
        cout << "fixedpoint" << endl;
        fixedPointStmt *fixedstmt = (fixedPointStmt *)stmt;
        analyseBlockfirst((blockStatement *)fixedstmt->getBody(), stmt);
        analyseBlock((blockStatement *)fixedstmt->getBody(), stmt);
        break;
    }
    case NODE_DOWHILESTMT:
    {
        cout << "dowhile" << endl;
        dowhileStmt *dowhilestmt = (dowhileStmt *)stmt;
        analyseBlockfirst((blockStatement *)dowhilestmt->getBody(), stmt);
        analyseBlock((blockStatement *)dowhilestmt->getBody(), stmt);
        break;
    }
    }
}

void readExpressionAnalyser::analyseBlock(blockStatement *stmt, statement *par)
{
    list<statement *> currStmts = stmt->returnStatements();
    for (statement *bstmt : currStmts)
    {
        analyseStatementinBlock(bstmt, par);
    }
    return;
}

void readExpressionAnalyser::analyseStatement(statement *stmt)
{
    switch (stmt->getTypeofNode())
    {
    case NODE_FIXEDPTSTMT:
    {
        cout << "fixedpoint" << endl;
        fixedPointStmt *fixedstmt = (fixedPointStmt *)stmt;
        analyseBlock((blockStatement *)fixedstmt->getBody(), stmt);
        break;
    }
    case NODE_DOWHILESTMT:
    {
        cout << "dowhile" << endl;
        dowhileStmt *dowhilestmt = (dowhileStmt *)stmt;
        analyseBlock((blockStatement *)dowhilestmt->getBody(), stmt);
        break;
    }
    case NODE_FORALLSTMT:
    {
        cout << "forall" << endl;
        forallStmt *forStmt = (forallStmt *)stmt;
        analyseBlock((blockStatement *)forStmt->getBody(), stmt);
        break;
    }
    case NODE_BLOCKSTMT:
    {
        cout << "block" << endl;
        analyseBlock((blockStatement *)stmt, NULL);
        break;
    }
    
    default:
        return;
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
    for (Function *func : funcList)
    {
        cout << endl
             << "READ EXPRESSION ANALYSER" << endl;
        analyseFunc(func);
    }
}