#ifndef SYMBOLTABLEBUILDER_H
#define SYMBOLTABLEBUILDER_H



#include "../ast/ASTNodeTypes.hpp"
#include "../maincontext/MainContext.hpp"
#include <cassert>

class SymbolTableBuilder
{
 private:
 list<SymbolTable*> variableSymbolTables;
 list<SymbolTable*> propSymbolTables;
 SymbolTable* currVarSymbT;
 SymbolTable* currPropSymbT;

 public:
 SymbolTableBuilder()
 {
     currVarSymbT=NULL;
     currPropSymbT=NULL;
 }

 void init_curr_SymbolTable(ASTNode* node);
 /*{
   node->getVarSymbT()->setParent(currVarSymbT);
   node->getPropSymbT()->setParent(currPropSymbT);
   variableSymbolTables.push_back(currVarSymbT);
   propSymbolTables.push_back(currPropSymbT);
   currVarSymbT=node->getVarSymbT();
   currPropSymbT=node->getPropSymbT();

 }*/

 void delete_curr_SymbolTable();
 /*{ 
     currVarSymbT=variableSymbolTables.back();
     currPropSymbT=propSymbolTables.back();

     variableSymbolTables.pop_back();
     propSymbolTables.pop_back();
 }*/

void buildST(list<Function*> funcList);
/*{
    //list<Function*> funcList=frontEndContext.getFuncList();
    list<Function*>::iterator itr;
    for(itr=funcList.begin();itr!=funcList.end();itr++)
       buildForProc((*itr));
} */
  
void buildForProc(Function* func);
void buildForStatements(statement* stmt);
void checkReductionExpr(reductionCall* reduce);
void checkForExpressions(Expression* expr);
bool checkHeaderSymbols(Identifier* src,PropAccess* propId,forallStmt* forall);
bool findSymbolId(Identifier* id);
bool findSymbolPropId(PropAccess* propId);


};

#endif
