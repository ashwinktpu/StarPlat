 #ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <cstddef> //for NULL in gcc 6
#include<list>
#include <cstddef>

using namespace std;

class Identifier;
class Type;
class ASTNode;

class TableEntry
{
  private:
  Identifier* id;
  Type* type;
  bool isArgument;

  TableEntry():id(nullptr), type(nullptr),isArgument(false){

  }
 
public:
 TableEntry(Identifier* idSent,Type* typeSent);
 Identifier* getId()
 {
     return id;
 }

 Type* getType()
 {
     return type;
 }
 
 void setArgument(bool ifArg)
 {
    isArgument=ifArg;
 }

 bool isArg()
 {
     return isArgument;
 }

};

class SymbolTable
{

private:
ASTNode* symbTabOwner;
int sTtype;
SymbolTable* parent;
list<TableEntry*> entries;

public:

SymbolTable(ASTNode* ownerNode,int symbTabType)
{
    parent=nullptr;
    symbTabOwner=ownerNode;
    sTtype=symbTabType;
    
}

int getSymbolTabType()
{
    return sTtype;
}

SymbolTable* getParent()
{
    return parent;
}

ASTNode* getOwnerNode()
{
    return symbTabOwner;
}

list<TableEntry*> getEntries()
{
    return entries;
}

  void addEntry(TableEntry* entry)
    {
    entries.push_back(entry);
    }

   void setParent(SymbolTable* sTParent)
   {
       parent=sTParent;
   } 

void create_and_add_entry(Identifier* id, Type* type);

TableEntry* findEntryInST(Identifier* id);
/*{
   list<TableEntry*>::iterator itr;

   for(itr=entries.begin();itr!=entries.end();itr++)
   {
       char* idName=id->getIdentifier();
       char* entryIdName=(*itr)->getId()->getIdentifier();
       if(!strcmp(idName,entryIdName))
       {
           return (*itr);
       }
   }
   if(parent!=NULL)
   {
       return findEntryInST(parent);
   }
   else
     return NULL;
}*/

bool check_conflicts_and_add(SymbolTable* sTab,Identifier* id,Type* type);
/* {
        assert(id->getSymbolInfo() == NULL);
        TableEntry* tabEntry=findEntryInST(id);
        if (tabEntry != NULL) 
        return false;
        create_and_add_entry(id, type);
        return true;


 }*/






};

#endif

 
