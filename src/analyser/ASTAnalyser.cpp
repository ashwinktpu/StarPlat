#include "ASTAnalyser.h"

#include <string.h>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include "../ast/ASTHelper.cpp"

struct statementRange
{
  statement *stmt;
  int l;
  int r;

  bool operator < (statementRange &stmt1)
  {
      if(l < stmt1.l)
        return true;

      return (r < stmt1.r);
  }
};

struct statementPos
{
  statement *stmt;
  int pos;

  bool operator < (statementPos &stmt1)
  {
      return (pos < stmt1.p)
  }
};

void ASTAnalyser::analyseStatement(statement *stmt)
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

list<Identifier *> getVarsExpr(Expression *expr)
{
  list<Identifier *> result;

  if (expr->isIdentifierExpr())
  {
    Identifier *iden = expr->getId();
    result.push_back(iden);
  }
  else if (expr->isPropIdExpr())
  {
    PropAccess *propExpr = expr->getPropId();
    result.push_back(propExpr->getIdentifier2());
  }
  else if (expr->isUnary())
    result = getVarsExpr(expr->getUnaryExpr());
  else if (expr->isLogical() || expr->isArithmetic() || expr->isLogical())
  {
    result = getVarsExpr(expr->getLeft());
    result.merge(getVarsExpr(expr->getRight()));
  }
  return result;
}

bool checkDependancy(statement *stmt, unordered_set<string> &usedVars)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
  {
    declaration *declStmt = (declaration *)stmt;
    string idenAssigned(declStmt->getdeclId()->getIdentifier());

    if (usedVars.find(idenAssigned) != usedVars.end())
      return true;
  }
  break;

  case NODE_ASSIGN:
  {
    assignment *assgnStmt = (assignment *)stmt;
    if (assgnStmt->lhs_isIdentifier())
    {
      string idenAssigned(assgnStmt->getId()->getIdentifier());
      if (usedVars.find(idenAssigned) != usedVars.end())
        return true;
    }
    else if (assgnStmt->lhs_isProp())
    {
      PropAccess *propId = assgnStmt->getPropId();
      string propAssigned(propId->getIdentifier2()->getIdentifier());
      if (usedVars.find(propAssigned) != usedVars.end())
        return true;
    }
  }
  break;

  case NODE_PROCCALLSTMT:
    break;

  case NODE_UNARYSTMT:
  {
    unary_stmt *unaryStmt = (unary_stmt *)stmt;
    Expression *expr = unaryStmt->getUnaryExpr();

    if (expr->isPropIdExpr())
    {
      PropAccess *propId = expr->getPropId();
      string propAssigned(propId->getIdentifier2()->getIdentifier());
      if (usedVars.find(propAssigned) != usedVars.end())
        return true;
    }
    else if (expr->isIdentifierExpr())
    {
      string idenAssigned(expr->getId()->getIdentifier());
      if (usedVars.find(idenAssigned) != usedVars.end())
        return true;
    }
  }
  break;

  default:
    return true;
  }

  return false;
}

pair<int, int> getRange(proc_callStmt *stmt, int currPos, vector<statement *> stmts)
{
  int l = currPos;
  int r = currPos + 1;

  unordered_set<string> usedVars;
  proc_callExpr *callStmt = stmt->getProcCallExpr();
  for (argument *arg : callStmt->getArgList())
  {
    if (arg->isAssignExpr())
    {
      assignment *asgn = arg->getAssignExpr();
      Expression *expr = asgn->getExpr();

      for (Identifier *iden : getVarsExpr(expr))
        usedVars.insert(string(iden->getIdentifier()));
    }
  }

  while (l > 0 && !checkDependancy(stmts[l - 1], usedVars))
  {
    l--;
  }

  while (r < stmts.size() && !checkDependancy(stmts[r], usedVars))
  {
    r++;
  }

  return make_pair(l, r);
}

list<statementPos> findStmtPosition(list<statementRange> stmts)
{
  stmts.sort();
  list<statementRange>::iterator itr, itr1;
  list<statementPos> result;

  for(itr = stmts.begin(); itr!=stmts.end())
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
}

void ASTAnalyser::analyseBlock(blockStatement *blockStmt)
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

  unordered_map<string, list<statementRange>> uMap;
  for (statementPos stPos : nodeAttachList)
  {
    pair<int, int> validRange = getRange(stPos.stmt, stPos.pos, newStatements);

    proc_callExpr *callExpr = ((proc_callStmt *)stPos.stmt)->getProcCallExpr();
    Identifier *graphIden = callExpr->getId1();

    uMap[string(graphIden->getIdentifier())].push_back({stPos.stmt, validRange.first, validRange.second});
  }

  list<statementPos> newAttachNodeStmt;
  for (pair<string, list<statementRange>> attachList : uMap)
  {
    list<statementPos> currAttachList = findStmtPosition(attachList.second);

    for (list<statementPos>::iterator itr = currAttachList.begin(); itr != currAttachList.end();)
    {
      proc_callStmt *proc_call = (proc_callStmt *)(itr->stmt);
      proc_callExpr *procedure = proc_call->getProcCallExpr();

      list<statementPos>::iterator itrNxt = itr;
      itrNxt++;

      while (itrNxt != currAttachList.end() && (itrNxt->pos) == itr->pos)
      {
        proc_callExpr *nxtProcedure = ((proc_callStmt *)(itrNxt->stmt))->getProcCallExpr();
        for (argument *arg : nxtProcedure->getArgList())
          procedure->addToArgList(arg);

        itrNxt++;
      }

      newAttachNodeStmt.push_back(*itr);
      itr = itrNxt;
    }
  }
  
  newAttachNodeStmt.sort();
  for(list<statementPos>::iterator itr = newAttachNodeStmt.rbegin(); itr != newAttachNodeStmt.rbegin(); itr++)
    newStatements.insert(newStatements.begin() + itr->pos, itr->stmt);

  blockStmt->clearStatements();
  for(statementPos stmt: nodePropList)
    blockStmt->addStmtToBlock(stmt.stmt);
  for(statement* stmt: newStatements)
    blockStmt->addStmtToBlock(stmt);
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
  for (Function *func : funcList)
  {
    analyseFunc(func);
  }
}
