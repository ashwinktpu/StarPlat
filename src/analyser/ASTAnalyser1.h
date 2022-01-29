#ifndef AST_ANALYSER1
#define AST_ANALYSER1

#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
//#include "dslCodePad.h"

class usedVariables
{
    private:
    list<Identifier*> varName;

    public:
    bool checkEqual(Identifier* iden1, Identifier* iden2)
    {
        return (strcmp(var->getIdentifier(), iden->getIdentifier()) == 0);
    }
    void addVariable(Identifier* iden)
    {
        for(Identifier* var: varName)
            if(checkEqual(iden, var))
                return;
        
        varName.push_back(iden);
    }

    void merge(usedVariables &usedVars1)
    {
        for(Identifier *var: usedVars1.gerVariables())
            this->addVariable(var);
    }

    void removeVariable(list<Identifier*> toRemove)
    {
        list<Identifier*> newList;
        for(Identifier* var: this->varName)
        {
            newList.push_back(var);
            for (Identifier *remIden : toRemove)
            {
                if (checkEqual(remIden, var))
                {
                    newList.pop_back();
                    break;
                }
            }
        }
        this->varName = newList;
    }

    void gerVariables(){
        return this->varName;
    }
};

class ASTAnalyser1
{
  public:
  ASTAnalyser1(){
  }

  void analyse();
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  void analyseBlock(blockStatement* blockStmt);
  usedVariables getVarsStatement(statement* stmt);
  usedVariables getVarsBlock(blockStatement* stmt);
  usedVariables getVarsWhile(whileStmt* stmt);
  usedVariables getVarsDoWhile(dowhileStmt* stmt);
  usedVariables getVarsAssignment(assignment* stmt);
  usedVariables getVarsIf(ifStmt* stmt);
  usedVariables getVarsFixedPoint(fixedPointStmt* stmt);
  usedVariables getVarsReduction(reductionCallStmt* stmt);
  usedVariables getVarsBFS(iterateBFS* stmt);
  usedVariables getVarsExpr(Expression* stmt);
  //usedVariables getVarsProcCall(proc_callStmt* stmt);
};

#endif
