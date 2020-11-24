#include <string>
#include<stdio.h>
#include <map>
#include<Symbol.cpp>

using namespace std;

class SymbolTable
{
  SymbolTable();
  map<string,Symbol*> symbMap;

  Symbol* addSymbol(string symName,string Type,string enclosedType);
  Symbol* getSymbol(string symName);

};