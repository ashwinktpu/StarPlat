#include "attachPropAnalyser.h"

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
bool checkDependancy(statement *stmt, usedVariables &usedVars)
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

  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
  {
    declaration *declStmt = (declaration *)stmt;
    if (usedVars.isUsedVar(declStmt->getdeclId()))
      return true;

    if (declStmt->isInitialized() && checkExprDependancy(declStmt->getExpressionAssigned()))
      return true;
  }
  break;

  case NODE_ASSIGN:
  {
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
  break;

  case NODE_PROCCALLSTMT:
  {
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
  break;

  case NODE_UNARYSTMT:
  {
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
  break;
  //For all other statements return True
  default:
    return true;
  }
  return false;
}

//Returns the minimum left index and maximum right index upto which a statement can be moved
statementRange getRange(statementPos stPos, vector<statement *> stmts)
{
  int l = stPos.pos;
  int r = stPos.pos + 1;

  usedVariables usedVars;

  proc_callStmt *stmt = (proc_callStmt *)stPos.stmt;
  proc_callExpr *callStmt = stmt->getProcCallExpr();

  for (argument *arg : callStmt->getArgList())
  {
    if (arg->isAssignExpr())
    {
      assignment *asgn = arg->getAssignExpr();
      Expression *expr = asgn->getExpr();

      usedVars.merge(analyserUtils::getVarsExpr(expr));
      usedVars.addVariable(asgn->getId(), WRITE);
    }
  }

  while (l > 0 && !checkDependancy(stmts[l - 1], usedVars))
  {
    l--;
  }

  while ((unsigned)r <= stmts.size() && !checkDependancy(stmts[r - 1], usedVars))
  {
    r++;
  }

  return {stPos, l, r, usedVars};
}

/*
list<statementPos> findStmtPosition(list<statementRange> stmts)
{
  stmts.sort();
  list<statementRange>::iterator itr, itr1;
  list<statementPos> result;

  for(itr = stmts.begin(); itr!=stmts.end();)
  {
    int left = itr->l;
    int right = itr->r;

    itr1 = itr;
    while (itr1 != stmts.end())
    {
      if(itr1->l < right)
      {
        left = max(left, itr1->l);
        right = min(right, itr1->r);

        itr1++;
      }
      else
        break;
    }

    while(itr != itr1)
    {
      result.push_back({itr->stmt, left});
      ++itr;
    }
  }
  return result;
}*/

void attachPropAnalyser::analyseBlock(blockStatement *blockStmt)
{

  list<statement *> stmtList = blockStmt->returnStatements();
  list<statement *>::iterator itr;

  int numStmts = 0;
  list<statementPos> nodePropList, nodeAttachList;
  vector<statement *> newStatements;

  for (itr = stmtList.begin(); itr != stmtList.end(); itr++)
  {
    statement *stmt = *itr;

    switch (stmt->getTypeofNode())
    {
    case NODE_PROCCALLSTMT:
    {
      proc_callExpr *procedure = ((proc_callStmt *)stmt)->getProcCallExpr();
      string methodID(procedure->getMethodId()->getIdentifier());

      if (methodID == "attachNodeProperty")
        nodeAttachList.push_back({stmt, numStmts});
      else
      {
        newStatements.push_back(stmt);
        numStmts++;
      }
      break;
    }

    case NODE_DECL:
    {
      Type *type = ((declaration *)stmt)->getType();

      if (type->isPropType())
        nodePropList.push_back({stmt, numStmts});
      else
      {
        newStatements.push_back(stmt);
        numStmts++;
      }
    }
    break;

    default:
    {
      analyseStatement(stmt);

      newStatements.push_back(stmt);
      numStmts++;
    }
    }
  }

  //Getting the left min and right max of each attachNodeProp statement
  list<statementRange> stmtRanges;
  for (statementPos stPos : nodeAttachList)
  {
    statementRange validRange = getRange(stPos, newStatements);
    stmtRanges.push_back(validRange);
  }

  list<statementPos> newAttachNodeStmt;
  list<statementRange>::iterator itrs, itre;

  //Merging the attachProp statements based on dependancy
  for (itrs = stmtRanges.begin(); itrs != stmtRanges.end();)
  {

    int left = (itrs->stPos).pos;
    int right = itrs->r;

    usedVariables currMerge;

    itre = itrs;
    while (itre != stmtRanges.end())
    {
      if (itre->l < right && !checkDependancy((itre->stPos).stmt, currMerge))
      {
        left = max(left, itre->l);
        right = min(right, itre->r);

        currMerge.merge(itre->dependantVars);
        itre++;
      }
      else
        break;
    }

    proc_callStmt *proc_call = (proc_callStmt *)(itrs->stPos).stmt;
    proc_callExpr *procedure = proc_call->getProcCallExpr();

    ++itrs;
    while (itrs != itre)
    {
      proc_callExpr *nxtProcedure = ((proc_callStmt *)(itrs->stPos).stmt)->getProcCallExpr();
      for (argument *arg : nxtProcedure->getArgList())
        procedure->addToArgList(arg);

      ++itrs;
    }
    newAttachNodeStmt.push_back({proc_call, left});
  }

  for (list<statementPos>::reverse_iterator itr = newAttachNodeStmt.rbegin(); itr != newAttachNodeStmt.rend(); itr++){
    newStatements.insert(newStatements.begin() + itr->pos, itr->stmt);
  }
  if(backend==1){ // this is only valid for cuda backend for now , change this if the other cpp generators are changed in future
    vector<statement*> final_statements;  // merging the attachnode property with device variable assignments
    int last_attach_node_prop_pos = -1;
    int curr_pos = 0;
    for(auto stmt:newStatements){
      switch (stmt->getTypeofNode()){
        case NODE_PROCCALLSTMT:
        {
          proc_callExpr *procedure = ((proc_callStmt *)stmt)->getProcCallExpr();
          string methodID(procedure->getMethodId()->getIdentifier());
          final_statements.push_back(stmt);
          if (methodID == "attachNodeProperty"){
            last_attach_node_prop_pos = curr_pos;
          }
        break;
        }
        case NODE_ASSIGN:{
          if(((assignment*)stmt)->lhs_isProp() && ((assignment*)stmt)->isDeviceAssignment()){
            bool flag = true;
            usedVariables usedVars;
            usedVars.merge(analyserUtils::getVarsExpr(((assignment*)stmt)->getExpr()));
            PropAccess* propId = ((assignment*)stmt)->getPropId();
            usedVars.addVariable(propId->getIdentifier1(),WRITE);
            usedVars.addVariable(propId->getIdentifier2(),WRITE);
            for(int ii=last_attach_node_prop_pos+1;ii<final_statements.size();ii++){
              if(checkDependancy(final_statements[ii],usedVars)){
                flag = false;
                break;
              }
            }
            if(flag){
              if(last_attach_node_prop_pos<0 || last_attach_node_prop_pos>=final_statements.size()){
                final_statements.push_back(stmt);
              }
              else{
                proc_callExpr* expr = ((proc_callStmt*)final_statements[last_attach_node_prop_pos])->getProcCallExpr();
                argument* arg = new argument();
                arg->setAssign((assignment*)stmt,true);
                expr->addToArgList(arg);
                curr_pos--;
              }
            }
            else{
              final_statements.push_back(stmt);
            }
          }
          else{
            final_statements.push_back(stmt);
          }
          break;
        }
        default:
          final_statements.push_back(stmt);
          break;
      }
      curr_pos++;
    }
    newStatements = final_statements;
    final_statements.clear();
    curr_pos = 0;
    int last_device_assignment = -1;
    for(auto stmt:newStatements){
      switch(stmt->getTypeofNode()){
        case NODE_ASSIGN:{
          if(((assignment*)stmt)->lhs_isProp() && ((assignment*)stmt)->isDeviceAssignment()){
            if(last_device_assignment!=-1){
              bool flag = true;
              usedVariables usedVars;
              usedVars.merge(analyserUtils::getVarsExpr(((assignment*)stmt)->getExpr()));
              PropAccess* propId = ((assignment*)stmt)->getPropId();
              usedVars.addVariable(propId->getIdentifier1(),WRITE);
              usedVars.addVariable(propId->getIdentifier2(),WRITE);
              for(int ii=last_device_assignment+1;ii<final_statements.size();ii++){
                if(checkDependancy(final_statements[ii],usedVars)){
                  flag = false;
                  break;
                }
              }
              if(flag){
                ((assignment*)final_statements[last_device_assignment])->add_device_assignment((assignment*)stmt);
                curr_pos--;
              }
              else{
                final_statements.push_back(stmt);
                last_device_assignment = curr_pos;
              }
            }
            else{
              final_statements.push_back(stmt);
              last_device_assignment = curr_pos;
            }
          }
          else{
            final_statements.push_back(stmt);
          }
          break;
        }
        default:{
          final_statements.push_back(stmt);
        }
      }
      curr_pos++;
    }
    newStatements = final_statements;
  }
  //adding the merged statements to block
  blockStmt->clearStatements();
  for (statementPos stmt : nodePropList) //Adding property declaration to start of block
    blockStmt->addStmtToBlock(stmt.stmt);
  for (statement *stmt : newStatements){
    blockStmt->addStmtToBlock(stmt);
  }
}


void attachPropAnalyser::analyseStatement(statement *stmt)
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

void attachPropAnalyser::analyseFunc(ASTNode *proc)
{
  Function *func = (Function *)proc;
  analyseBlock(func->getBlockStatement());
  return;
}

void attachPropAnalyser::analyse(list<Function *> funcList)
{
  //list<Function *> funcList = frontEndContext.getFuncList();
  for (Function *func : funcList)
  {
    analyseFunc(func);
  }
}
