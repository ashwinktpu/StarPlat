#include "deviceVarsAnalyser.h"

void printTabs(int tabSpace)
{
  while (tabSpace--)
    cout << '\t';
}

void deviceVarsAnalyser::printBlock(blockStatement *stmt, int tabSpace)
{
  printTabs(tabSpace);
  cout << "Block Begin\n";
  tabSpace++;
  for (statement *bstmt : stmt->returnStatements())
  {
    printStatement(bstmt, tabSpace);
  }
  tabSpace--;
  printTabs(tabSpace);
  cout << "Block End\n";
}

void deviceVarsAnalyser::printDeclaration(declaration *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "Declaration: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printAssignment(assignment *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "Assignment: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printIfElse(ifStmt *stmt, int tabSpace)
{
  printTabs(tabSpace);
  cout << "If Statement\n";

  tabSpace++;
  printTabs(tabSpace);
  cout << "Condition: ";

  Expression *cond = stmt->getCondition();
  ASTNodeWrap *wrapNode = getWrapNode(cond);
  (wrapNode->outMap).print();

  printTabs(tabSpace);
  cout << "If Body\n";
  tabSpace++;
  printStatement(stmt->getIfBody(), tabSpace);
  tabSpace--;

  if (stmt->getElseBody() != nullptr)
  {
    printTabs(tabSpace);
    cout << "Else Body\n";
    tabSpace++;
    printStatement(stmt->getElseBody(), tabSpace);
    tabSpace--;
  }
  
  //printf("%p %d \n", stmt, stmt->getTypeofNode());
  ASTNodeWrap *stmtWrap = getWrapNode(stmt);

  printTabs(tabSpace);
  cout << "Merge point: ";
  (stmtWrap->outMap).print();
  tabSpace--;
}

void deviceVarsAnalyser::printUnary(unary_stmt *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "Unary statment: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printForAll(forallStmt *stmt, int tabSpace)
{
  ASTNodeWrap *wrapNode = getWrapNode(stmt);
  printTabs(tabSpace);
  cout << "For All stmt: ";
  (wrapNode->outMap).print();
}

void deviceVarsAnalyser::printStatement(statement *stmt, int tabSpace)
{
  switch (stmt->getTypeofNode())
  {
  case NODE_DECL:
    printDeclaration((declaration *)stmt, tabSpace);
    break;
  case NODE_ASSIGN:
    printAssignment((assignment *)stmt, tabSpace);
    break;
  case NODE_FORALLSTMT:
  {
    forallStmt *forStmt = (forallStmt *)stmt;
    if (forStmt->isForall())
      printForAll((forallStmt *)stmt, tabSpace);
    break;
  }
  case NODE_BLOCKSTMT:
    printBlock((blockStatement *)stmt, tabSpace);
    break;
  case NODE_UNARYSTMT:
    printUnary((unary_stmt *)stmt, tabSpace);
    break;
  case NODE_IFSTMT:
    printIfElse((ifStmt *)stmt, tabSpace);
    break;
  }
}