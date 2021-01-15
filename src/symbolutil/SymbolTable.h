#include <string>
#include <stdio.h>
#include <map>
<<<<<<< HEAD
#include"Symbol.h"
=======
#include "Symbol.h"

>>>>>>> 43c2a08e026f040d5049c3bf784dfbb42fa84e08

using namespace std;


class Scope
{ 
  protected:
  map<string,Symbol*> *HashInfo;
  
  public:
  Scope();
  void buildInfo();
  bool hasHashInfo();
  void insertInScope(Symbol* symbol);
  Symbol* LookUpinScope(Identifier* id);

};

class SymbolTable
{
  vector<Scope*> *scopeVector;
  vector<int> *activeScope;
  int current_Scope;
  int scope_count;

  SymbolTable();

  void createNewScope();
  void exitScope();
  
  void insertSymbol(string symName,string Type,string enclosedType);
   Symbol* LookUp(Identifier* id);


};