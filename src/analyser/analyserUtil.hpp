#ifndef ANALYSER_UTIL
#define ANALYSER_UTIL

#include "../ast/ASTNodeTypes.hpp"
#include <unordered_map>
#include "../symbolutil/SymbolTable.h"

typedef pair<TableEntry*, TableEntry*> propKey

enum variable_type
{
    READ = 1,
    WRITE,
    READ_WRITE
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
}*/

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
};

class usedVariables
{
private:
    unordered_map<TableEntry*, Identifier*> readVars, writeVars;
    unordered_map<propKey, PropAccess*> readProp, writeProp;

public:
    void addVariable(Identifier *iden, int type)
    {
        if (type & 1)
            readVars.insert({iden->getSymbolInfo(), iden});
        if (type & 2)
            writeVars.insert({iden->getSymbolInfo(), iden});
    }

    void addPropAccess(PropAccess *prop, int type)
    {
        Identifier* iden1 = prop->getIdentifier1();
        Identifier* iden2 = prop->getIdentifier2();
        propKey prop_key = make_pair(iden1->getSymbolInfo(), iden2->getSymbolInfo());

        if (type & 1)
            readProp.insert({prop_key, prop});
        if (type & 2)
            writeProp.insert({prop_key, prop});
    }

    void merge(usedVariables usedVars1)
    {
        for (pair<TableEntry*, Identifier*> iden: usedVars1.readVars)
            this->readVars.insert({iden.first, iden.second});

        for (pair<TableEntry*, Identifier*> iden : usedVars1.writeVars)
            this->writeVars.insert({iden.first, iden.second});

        for (pair<propKey, Identifier*> iden: usedVars1.readProp)
            this->readProp.insert({iden.first, iden.second});

        for (pair<propKey, Identifier*> iden : usedVars1.writeProp)
            this->writeProp.insert({iden.first, iden.second});
    }

    void removeVariable(Identifier *iden, int type)
    {
        if (type & 1)
        {
            if (readVars.find(iden->getSymbolInfo()) != readVars.end())
                readVars.erase(iden->getSymbolInfo());
        }

        if (type & 2)
        {
            if (writeVars.find(iden->getSymbolInfo()) != writeVars.end())
                writeVars.erase(iden->getSymbolInfo());
        }
    }

    bool isRead(Identifier *iden)
    {
        return (readVars.find(iden->getSymbolInfo()) != readVars.end());
    }

    bool isWrite(Identifier *iden)
    {
        return (writeVars.find(iden->getSymbolInfo()) != writeVars.end());
    }

    bool isUsed(Identifier *iden)
    {
        TableEntry* symbInfo = iden->getSymbolInfo();
        return ((readVars.find(symbInfo) != readVars.end()) || (writeVars.find(symbInfo) != writeVars.end()));
    }

    list<Identifier *> getReadVariables()
    {
        list<Identifier *> result;
        for (pair<TableEntry*, Identifier*> iden : readVars)
            result.push_back(iden.second);

        return result;
    }

    list<Identifier *> getWriteVariables()
    {
        list<Identifier *> result;
        for (pair<TableEntry*, Identifier*> iden : writeVars)
            result.push_back(iden.second);

        return result;
    }

    bool hasReadVariables(){
        return (readVars.size() > 0);
    }

    bool hasWriteVariables(){
        return (writeVars.size() > 0);
    }
};

usedVariables getVarsExpr(Expression *expr)
{
    usedVariables result;

    if (expr->isIdentifierExpr())
    {
        Identifier *iden = expr->getId();
        result.addVariable(iden, READ);
    }
    else if (expr->isPropIdExpr())
    {
        PropAccess *propExpr = expr->getPropId();
        result.addVariable(propExpr->getIdentifier2(), READ);
    }
    else if (expr->isUnary())
    {
        if (expr->getOperatorType() == OPERATOR_NOT)
            result = getVarsExpr(expr->getUnaryExpr());
        else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC))
        {
            Expression *uExpr = expr->getUnaryExpr();
            if (uExpr->isIdentifierExpr())
                result.addVariable(uExpr->getId(), READ_WRITE);
            else if (uExpr->isPropIdExpr())
                result.addVariable(uExpr->getPropId()->getIdentifier2(), READ_WRITE);
        }
    }
    else if (expr->isLogical() || expr->isArithmetic() || expr->isRelational())
    {
        result = getVarsExpr(expr->getLeft());
        result.merge(getVarsExpr(expr->getRight()));
    }
    return result;
}

#endif