#include "ForLoopFusionAnalyser.h"

#include<vector>
#include <string.h>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include "../analyserUtil.cpp"
#include "../../ast/ASTHelper.cpp"

#include <iostream>
using namespace std;

//Stores the statement pointer and the index in it's parent block
struct statementPos
{
  statement *stmt;
  int pos;
};

//Stores the used variables, minimum left index and maximum right index of a statement
struct statementRange
{
  statementPos stPos;
  int l;
  int r;

  usedVariables dependantVars;
};

//Checks whether there is RAW, WAW, WAR dependency b/w stmt and variables in usedVars
bool check_dependancy(statement *stmt, usedVariables &usedVars)
{
  auto checkExprDependancy = [&usedVars](Expression *expr) -> bool
  {
    usedVariables exprVars = analyserUtils::getVarsExpr(expr);
    for (Identifier *wVars : exprVars.getVariables(WRITE))
    {
      if (usedVars.isUsedVar(wVars))
        return true;
    }

    for (Identifier *rVars : exprVars.getVariables(READ))
    {
      if (usedVars.isUsedVar(rVars, WRITE))
        return true;
    }

    for (PropAccess *propId : exprVars.getPropAcess())
    {
      if ((usedVars.isUsedVar(propId->getIdentifier2())) || (usedVars.isUsedProp(propId)))
        return true;
    }

    return false;
  };
  auto node_type = stmt->getTypeofNode();
  if(node_type==NODE_DECL){
    declaration *declStmt = (declaration *)stmt;
    if (usedVars.isUsedVar(declStmt->getdeclId()))
      return true;

    if (declStmt->isInitialized() && checkExprDependancy(declStmt->getExpressionAssigned()))
      return true;
  }
  else if(node_type==NODE_ASSIGN){
    assignment *assgnStmt = (assignment *)stmt;
    if (assgnStmt->lhs_isIdentifier())
    {
      if (usedVars.isUsedVar(assgnStmt->getId()))
        return true;
    }

    else if (assgnStmt->lhs_isProp())
    {
      PropAccess *propId = assgnStmt->getPropId();
      if ((usedVars.isUsedVar(propId->getIdentifier2())) || (usedVars.isUsedProp(propId)))
        return true;
    }
    if (checkExprDependancy(assgnStmt->getExpr()))
      return true;
  }
  else if(node_type==NODE_PROCCALLSTMT){
    proc_callExpr *procedure = ((proc_callStmt *)stmt)->getProcCallExpr();
    string methodID(procedure->getMethodId()->getIdentifier());

    if (methodID == "attachNodeProperty")
    {
      for (argument *arg : procedure->getArgList())
      {

        if (arg->isAssignExpr())
        {
          assignment *asgn = arg->getAssignExpr();
          if(usedVars.isUsedProp(PropAccess::createPropAccessNode(nullptr, asgn->getId())))
            return true;

          if (checkExprDependancy(asgn->getExpr()))
            return true;

        }
      }
    }
  }
  else if(node_type==NODE_UNARYSTMT){
    unary_stmt *unaryStmt = (unary_stmt *)stmt;
    Expression *expr = unaryStmt->getUnaryExpr()->getUnaryExpr();
    if (expr->isPropIdExpr())
    {
      PropAccess *propId = expr->getPropId();
      if ((usedVars.isUsedVar(propId->getIdentifier2()))
          || (usedVars.isUsedProp(propId)))
        return true;
    }
    else if (expr->isIdentifierExpr())
    {
      if (usedVars.isUsedVar(expr->getId()))
        return true;
    }
  }
  else{
    return true;
  }
  return false;
}

bool check_block_dependancy(blockStatement* blockStmt,usedVariables& usedVars){
    for(auto stmt:blockStmt->returnStatements()){
        if(check_dependancy(stmt,usedVars)){
            return true;
        }
    }
    return false;
}

usedVariables get_used_vars(statement* stmt){
    usedVariables usedVars;
    switch (stmt->getTypeofNode())
    {
    case NODE_BLOCKSTMT:{
        for(auto itr:((blockStatement*)stmt)->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        break;
    }
    case NODE_WHILESTMT:{
        for(auto itr:((blockStatement*)(((whileStmt *)stmt)->getBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        break;
    }
    case NODE_DOWHILESTMT:{
        for(auto itr:((blockStatement*)(((dowhileStmt *)stmt)->getBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        break;
    }
    case NODE_FIXEDPTSTMT:{
        for(auto itr:((blockStatement*)(((fixedPointStmt *)stmt)->getBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        break;
    }
    case NODE_IFSTMT:{
        for(auto itr:((blockStatement*)(((ifStmt *)stmt)->getIfBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        if(((ifStmt *)stmt)->getElseBody()){
          for(auto itr:((blockStatement*)(((ifStmt *)stmt)->getElseBody()))->returnStatements()){
              usedVars.merge(get_used_vars(itr));
          }
        }
        break;
    }
    case NODE_ITRBFS:{
        for(auto itr:((blockStatement*)(((iterateBFS *)stmt)->getBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        for(auto itr:((blockStatement*)((((iterateBFS *)stmt)->getRBFS())->getBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        break;
    }
    case NODE_FORALLSTMT:{
        for(auto itr:((blockStatement*)(((forallStmt *)stmt)->getBody()))->returnStatements()){
            usedVars.merge(get_used_vars(itr));
        }
        break;
    }
    case NODE_ASSIGN:{
        assignment* asgn = (assignment*)stmt;
        Expression* expr = asgn->getExpr();
        usedVars.merge(analyserUtils::getVarsExpr(expr));
        if(asgn->lhs_isIdentifier()){
            usedVars.addVariable(asgn->getId(), WRITE);
        }
        else if(asgn->lhs_isProp()){
            PropAccess* propId = asgn->getPropId();
            usedVars.addVariable(propId->getIdentifier2(),WRITE);
            usedVars.addPropAccess(propId,WRITE);
        }
        break;
    }
    case NODE_DECL:{
        declaration* declStmt = (declaration*)stmt;
        if(declStmt->getType()->isPropType()){
            break;
        }
        Expression* expr = declStmt->getExpressionAssigned();
        usedVars.merge(analyserUtils::getVarsExpr(expr));
        break;
    }
    case NODE_UNARYSTMT:{
        unary_stmt *unaryStmt = (unary_stmt *)stmt;
        Expression *expr = unaryStmt->getUnaryExpr()->getUnaryExpr();

        if (expr->isPropIdExpr())
        {
            PropAccess *propId = expr->getPropId();
            usedVars.addVariable(propId->getIdentifier2(),WRITE);
            usedVars.addPropAccess(propId,WRITE);
        }
        else if (expr->isIdentifierExpr())
        {
            usedVars.addVariable(expr->getId(),WRITE);
        }
        break;
    }
    case NODE_PROCCALLSTMT:{
        proc_callExpr *procedure = ((proc_callStmt *)stmt)->getProcCallExpr();
        string methodID(procedure->getMethodId()->getIdentifier());

        if (methodID == "attachNodeProperty")
        {
        for (argument *arg : procedure->getArgList())
        {

            if (arg->isAssignExpr())
            {
                assignment *asgn = arg->getAssignExpr();
                PropAccess* propId = asgn->getPropId();
                usedVars.addVariable(propId->getIdentifier2(),WRITE);
                usedVars.addPropAccess(propId,WRITE);
                Expression* expr = asgn->getExpr();
                usedVars.merge(analyserUtils::getVarsExpr(expr));
            }
        }
        }
        break;
    }
    }
    return usedVars;
}

//Returns the minimum left index and maximum right index upto which a statement can be moved
statementRange get_range(statementPos stPos, vector<statement *> stmts)
{
    int l = stPos.pos;
    int r = stPos.pos + 1;
    usedVariables usedVars;
    forallStmt* for_all_stmt = (forallStmt*)stPos.stmt;
    usedVars = get_used_vars(for_all_stmt->getBody());
    while (l > 0 && !check_dependancy(stmts[l - 1], usedVars))
    {
        l--;
    }
    while ((unsigned)r <= stmts.size() && !check_dependancy(stmts[r - 1], usedVars))
    {
        r++;
    }
    return {stPos, l, r, usedVars};
}


void ForLoopFusionAnalyser::analyseBlock(blockStatement *blockStmt)
{
    list<statement*> stmtList = blockStmt->returnStatements();
    int pos = 0;
    vector<statement*> newStatements;
    list<statementPos> for_all_loops;
    for(auto itr = stmtList.begin();itr!=stmtList.end();itr++){
        auto stmt = *itr;
        switch (stmt->getTypeofNode())
        {
            case NODE_FORALLSTMT:{
                if(((forallStmt*)stmt)->isForall()){
                    for_all_loops.push_back({stmt,pos});
                }
                else{
                    newStatements.push_back(stmt);
                    pos++;
                }
                break;
            }
            default:{
                analyseStatement(stmt);
                newStatements.push_back(stmt);
                pos++;
                break;
            }
        }
    }
    list<statementRange> stmtRanges;
    for(auto itr:for_all_loops){
        stmtRanges.push_back(get_range(itr,newStatements));
    }
    list<statementPos> new_for_all_loops;
    list<statementRange>::iterator itrs, itre;
    return;
    for (itrs = stmtRanges.begin(); itrs != stmtRanges.end();)
    {
        int left = (itrs->stPos).pos;
        int right = itrs->r;

        usedVariables currMerge;

        itre = itrs;
        while (itre != stmtRanges.end())
        {
        blockStatement* block = (blockStatement*)((forallStmt*)(itre->stPos).stmt)->getBody();
        if (itre->l < right && !check_block_dependancy(block, currMerge))
        {
            left = max(left, itre->l);
            right = min(right, itre->r);

            currMerge.merge(itre->dependantVars);
            itre++;
        }
        else
            break;
        }
        forallStmt* for_all_stmt = (forallStmt*)((itrs->stPos).stmt);
        blockStatement* body = (blockStatement*)(for_all_stmt->getBody());
        ++itrs;
        while (itrs!=stmtRanges.end() && itrs != itre)
        {
            blockStatement* temp_body = (blockStatement*)(((forallStmt*)((itrs->stPos).stmt))->getBody());
            for(auto stmt:temp_body->returnStatements()){
                body->addStmtToBlock(stmt);
            }
            ++itrs;
        }
        new_for_all_loops.push_back({for_all_stmt,left});
    }
    for(auto itr:new_for_all_loops){
        newStatements.insert(newStatements.begin()+itr.pos,itr.stmt);
    }
    blockStmt->clearStatements();
    for(auto stmt:newStatements){
        blockStmt->addStmtToBlock(stmt);
    }
}


void ForLoopFusionAnalyser::analyseStatement(statement *stmt)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_BLOCKSTMT:
    analyseBlock((blockStatement *)stmt);
    break;

  case NODE_WHILESTMT:
    analyseStatement(((whileStmt *)stmt)->getBody());
    break;

  case NODE_DOWHILESTMT:
    analyseStatement(((dowhileStmt *)stmt)->getBody());
    break;

  case NODE_FIXEDPTSTMT:
    analyseStatement(((fixedPointStmt *)stmt)->getBody());
    break;

  case NODE_IFSTMT:
  {
    statement *ifBody = ((ifStmt *)stmt)->getIfBody();
    statement *elseBody = ((ifStmt *)stmt)->getElseBody();
    if (ifBody != NULL)
      analyseStatement(ifBody);
    if (elseBody != NULL)
      analyseStatement(elseBody);
    break;
  }

  case NODE_ITRBFS:
  {
    analyseStatement(((iterateBFS *)stmt)->getBody());
    iterateReverseBFS *revBFS = ((iterateBFS *)stmt)->getRBFS();
    if (revBFS != NULL)
      analyseStatement(revBFS->getBody());
    break;
  }

  case NODE_FORALLSTMT:
    analyseStatement(((forallStmt *)stmt)->getBody());
    break;
  }
}

void ForLoopFusionAnalyser::analyseFunc(ASTNode *proc)
{
  Function *func = (Function *)proc;
  analyseBlock(func->getBlockStatement());
  return;
}

void ForLoopFusionAnalyser::analyse(list<Function *> funcList)
{
  //list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
  {
    analyseFunc(func);
  }
}
