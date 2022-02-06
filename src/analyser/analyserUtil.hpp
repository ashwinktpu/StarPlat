#ifndef ANALYSER_UTIL
#define ANALYSER_UTIL

#include "../ast/ASTNodeTypes.hpp"
#include <unordered_set>

enum variable_type
{
    READ = 1,
    WRITE,
    READ_WRITE
};

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
    unordered_set<IdentifierWrap, IdentifierWrap::HashFunction> readVars, writeVars;

public:
    bool checkEqual(Identifier *iden1, Identifier *iden2)
    {
        return (strcmp(iden1->getIdentifier(), iden2->getIdentifier()) == 0);
    }
    void addVariable(Identifier *iden, int type)
    {
        if (type & 1)
            readVars.insert(IdentifierWrap(iden));
        if (type & 2)
            writeVars.insert(IdentifierWrap(iden));
    }

    void merge(usedVariables usedVars1)
    {
        for (IdentifierWrap var : usedVars1.readVars)
            this->readVars.insert(var);

        for (IdentifierWrap var : usedVars1.writeVars)
            this->writeVars.insert(var);
    }

    void removeVariable(Identifier *iden, int type)
    {
        IdentifierWrap idenNew(iden);
        if (type & 1)
        {
            if (readVars.find(idenNew) != readVars.end())
                readVars.erase(idenNew);
        }

        if (type & 2)
        {
            if (writeVars.find(idenNew) != writeVars.end())
                writeVars.erase(idenNew);
        }
    }

    bool isRead(Identifier *iden)
    {
        return (readVars.find(IdentifierWrap(iden)) != readVars.end());
    }

    bool isWrite(Identifier *iden)
    {
        return (writeVars.find(IdentifierWrap(iden)) != writeVars.end());
    }

    bool isUsed(Identifier *iden)
    {
        IdentifierWrap idenNew(iden);
        return ((readVars.find(idenNew) != readVars.end()) || (writeVars.find(idenNew) != writeVars.end()));
    }

    list<Identifier *> getReadVariables()
    {
        list<Identifier *> result;
        for (IdentifierWrap iden : readVars)
            result.push_back(iden.iden);

        return result;
    }

    list<Identifier *> getWriteVariables()
    {
        list<Identifier *> result;
        for (IdentifierWrap iden : writeVars)
            result.push_back(iden.iden);

        return result;
    }
};

usedVariables getVarsExpr(Expression *expr)
{
    usedVariables result;

    if (expr->isIdentifierExpr())
    {
        Identifier *iden = expr->getId();
        result.addVariable(iden, 1);
    }
    else if (expr->isPropIdExpr())
    {
        PropAccess *propExpr = expr->getPropId();
        result.addVariable(propExpr->getIdentifier2(), 1);
    }
    else if (expr->isUnary())
    {
        if (expr->getOperatorType() == OPERATOR_NOT)
            result = getVarsExpr(expr->getUnaryExpr());
        else if ((expr->getOperatorType() == OPERATOR_INC) || (expr->getOperatorType() == OPERATOR_DEC))
        {
            Expression *uExpr = expr->getUnaryExpr();
            if (uExpr->isIdentifierExpr())
                result.addVariable(uExpr->getId(), 3);
            else if (uExpr->isPropIdExpr())
                result.addVariable(uExpr->getPropId()->getIdentifier2(), 3);
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