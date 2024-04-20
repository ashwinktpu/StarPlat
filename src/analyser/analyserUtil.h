#ifndef ANALYSER_UTILITY
#define ANALYSER_UTILITY

#include <unordered_map>
#include "../ast/ASTNodeTypes.hpp"
//#include "../symbolutil/SymbolTable.h"

#define propKey pair<TableEntry *, TableEntry *>

enum variable_type
{
    READ = 1,
    WRITE,
    READ_WRITE,
    READ_ONLY, 
    WRITE_ONLY,
    READ_AND_WRITE
};

/*
enum depeandancy
{
    READ_READ,
    READ_WRITE,
    WRTIE_READ,
    WRITE_WRTIE,
    ALL_READ,
    ALL_WRITE,
    READ_ALL,
    WRITE_ALL
}

struct IdentifierWrap
{
    string id;
    Identifier *iden;

    IdentifierWrap() {}
    IdentifierWrap(Identifier *iden)
    {
        this->id = string(iden->getIdentifier());
        this->iden = iden;
    }
    bool operator==(const IdentifierWrap &iden) const{
        return (this->id == iden.id);
    }

    struct HashFunction
    {
        size_t operator()(const IdentifierWrap &iden) const{
            return hash<string>()(iden.id);
        }
    };
};*/

class usedVariables
{
private:
    struct hash_pair
    {
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2> &p) const
        {
            auto hash1 = hash<T1>{}(p.first);
            auto hash2 = hash<T2>{}(p.second);
            return hash1 ^ hash2;
        }
    };

    unordered_map<TableEntry *, Identifier *> readVars, writeVars;
    unordered_map<propKey, PropAccess *, hash_pair> readProp, writeProp;

public:
    usedVariables() {}
    void addVariable(Identifier *iden, int type);
    void addPropAccess(PropAccess *prop, int type);
    void merge(usedVariables usedVars1);
    void removeVariable(Identifier *iden, int type);
    bool isUsedVar(Identifier *iden, int type = READ_WRITE);
    list<Identifier *> getVariables(int type = READ_WRITE);
    bool hasVariables(int type = READ_WRITE);
    bool isUsedPropAcess(PropAccess *propId, int type = READ_WRITE);
    bool isUsedProp(PropAccess *propId, int type = READ_WRITE);
    list<PropAccess *> getPropAcess(int type = READ_WRITE);
    void clear();
};

class analyserUtils
{
public:
    static usedVariables getVarsExpr(Expression *expr);
};

class printAST
{
    void printTabs();

public:
    int tabSpace = 0;

    void printFunction(Function *func);
    void printBlock(blockStatement *stmt);
    void printStatement(statement *stmt);
};


#endif