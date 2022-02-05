#ifndef ANALYSER_UTIL
#define ANALYSER_UTIL

#include "../ast/ASTNodeTypes.hpp"
#include <unordered_set>

enum variable_type {READ = 1, WRITE, READ_WRITE};

struct identifierHash
{
    size_t operator () (const Identifier* &iden) const{
        return hash<string>(string(iden->getIdentifier()));
    }
};

struct identifierEqual
{
    bool operator () (const Identifier* &id1, const Identifier* &id2) const{
        return (strcmp(id1->getIdentifier(), id2->getIdentifier()) == 0);
    }
};

class usedVariables
{
    private:
    unordered_set<Identifier*, identifierHash, identifierEqual> readVars;
    unordered_set<Identifier*, identifierHash, identifierEqual> writeVars;

    public:
    bool checkEqual(Identifier* iden1, Identifier* iden2)
    {
        return (strcmp(var->getIdentifier(), iden->getIdentifier()) == 0);
    }
    void addVariable(Identifier* iden, int type)
    {
        if(type & 1)
            readVars.insert(iden);
        if(type & 2)
            writeVars.insert(iden);
    }

    void merge(usedVariables &usedVars1)
    {
        for(Identifier* var: usedVars1.readVars)
            this->readVars.insert(var);

        for(Identifier* var: usedVars1.writeVars)
            this->writeVars.insert(var);
    }

    void removeVariable(Identifier* iden, int type)
    {
        if(type & 1)
        {
            if(readVars.find(iden) != readVars.end())
                readVars.erase(iden);
        }

        if(type & 2)
        {
            if(writeVars.find(iden) != writeVars.end())
                writeVars.erase(iden);
        }
    }

    bool isRead(Identifier* iden)
    {
        return (readVars.find(iden) != readVars.end());
    }

    bool isWrite(Identifier* iden)
    {
        return (writeVars.find(iden) != writeVars.end());
    }

    bool isUsed(Identifier* iden)
    {
        return ((readVars.find(iden) != readVars.end()) 
                    || (writeVars.find(iden) != writeVars.end()));
    }

    list<Identifier*> getReadVariables(){
        return list<Identifier*>(readVars.begin(), readVars.end());
    }

    list<Identifier*> getWriteVariables(){
        return list<Identifier*>(writeVars.begin(), writeVars.end());
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
    if(expr->getOperatorType() == OPERATOR_NOT)
      result = getVarsExpr(expr->getUnaryExpr());
    else if((expr->getOperatorType() == OPERATOR_INC) 
              || (expr->getOperatorType() == OPERATOR_DEC))
    {
      Expression* uExpr = expr->getUnaryExpr();
      if(uExpr->isIdentifierExpr())
        result.addVariable(uExpr->getId(), 3);
      else if(uExpr->isPropIdExpr())
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