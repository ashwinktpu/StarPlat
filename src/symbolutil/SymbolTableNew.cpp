 #include "SymbolTable.h"
#include "../ast/ASTNodeTypes.hpp"
#include<cassert>

TableEntry::TableEntry(Identifier* idSent,Type* typeSent)
 {
     id=idSent;
     type=typeSent;
     isGlobalVar = false;
     id->setSymbolInfo(this);
     assert(type!=NULL);
     assert(id->getIdentifier()!=NULL);
 }


 void SymbolTable::create_and_add_entry(Identifier* id, Type* type) {
        Identifier* idCopy = id->copy();
        Type* typeCopy = type->copy();
        TableEntry* e = new TableEntry(idCopy,typeCopy);
        id->setSymbolInfo(e);
        entries.push_back(e);

       
    }

TableEntry* SymbolTable::findEntryInST(Identifier* id)
{ 
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
       return parent->findEntryInST(id);
   }
   else
     return NULL;
}

bool SymbolTable::check_conflicts_and_add(SymbolTable* sTab,Identifier* id,Type* type)
 {
        assert(id->getSymbolInfo() == NULL);
        TableEntry* tabEntry=findEntryInST(id);
        if (tabEntry != NULL) 
        return false;
        create_and_add_entry(id, type);
        return true;


 }
 