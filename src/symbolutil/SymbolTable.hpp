#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H



#include <string>
//#include<stdio.h>
#include <map>
#include "Symbol.hpp"
#include "../ast/ASTNodeTypes.hpp"

using namespace std;



class Scope
{ 
  protected:
  map<string,Symbol*> *HashInfo;
  
  public:
  Scope();
  void buildInfo();
  bool hasHashInfo();
  Symbol* LookUpinScope(Identifier* id);
  void insertInScope(Symbol* symbol);

};

class SymbolTable
{ 
  private:
  vector<Scope*> *scopeVector;
  vector<int> *activeScope;
  int current_Scope;
  int scope_count;

  public:

  SymbolTable();
  void createNewScope();
  void exitScope();
  void insertSymbol(string symName,string Type,string enclosedType);
  Symbol* LookUp(Identifier* id);


};

#endif