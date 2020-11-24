#include<SymbolTable.h>

using namespace std;

Symbol* SymbolTable::addSymbol(string symName,string Type="NONE",string enclosedType="NONE")
{
    Symbol* symbol=new Symbol(symName,Type,enclosedType);
    symbMap[symName]=symbol;

    return symbol;
}

Symbol* SymbolTable::getSymbol(string symName)
{
    Symbol* symbol=NULL;
    map<string,Symbol*>::iterator itr;
    itr=symbMap.find(symName);
    if(itr!=symbMap.end())
    {
       symbol=symbMap[symName];
    }
   
    return symbol;
}


