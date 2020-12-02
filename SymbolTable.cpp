#include<SymbolTable.h>

using namespace std;




SymbolTable::SymbolTable()
{ scopeVector=new vector<Scope*>;
  scopeVector.push_back(new Scope);
  activeScope = new vector<int>;
  activeScope.push_back(0);
  current_Scope=0;
  scope_count=0;
}

void SymbolTable::createNewScope()
{
  scopeVector.push_back(new Scope());
  scope_count++;
  current_Scope=scope_count;
  activeScope.push_back(scope_count);
}


void SymbolTable::exitScope()
{
  activeScope.pop_back();
  current_Scope=activeScope.back();
  }

 void SymbolTable::insertSymbol(string symName,string Type="NONE",string enclosedType="NONE")
{
    Symbol* symbol=new Symbol(symName,Type,enclosedType);
    Scope* scope=scopeVector.at(current_Scope);
    if(scope->hasHashInfo())
    {
        scope->buildInfo();
    }

    scope->insertInScope(symbol);

    return symbol;
}

Symbol* SymbolTable::LookUp(Identifier* id)
{   Symbol* symbol=NULL;
    for(int i=activeScope.size()-1;i>=0;i--)
    {
        Scope* scope=scopeVector.at(i);

        if(scope->hasHashInfo())
        {
             symbol=scope->LookUpinScope(id);
        }
         if(symbol!=NULL)
            break;

        
    }

    return symbol;
}

Scope::Scope()
{

 HashInfo=NULL;

}

Scope::void buildInfo()
{

    HashInfo=new map<string,Symbol*>;

}

Scope::bool hasHashInfo()
{
    if(HashInfo!=NULL)
     return true;
     else 
     return false;
}

Scope::void insertInScope(Symbol* symbol)
{
   HashInfo[symbol->name]=symbol;

}
Scope::Symbol* LookUpinScope(Identifier* id)
{

    Symbol* symbol=NULL;
    map<string,Symbol*>::iterator itr;
    itr=HashInfo.find(string);
    if(itr!=HashInfo.end())
    {
        symbol=HashInfo[string];
    }
    return symbol;
}

