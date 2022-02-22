#ifndef AST_ANALYSER1
#define AST_ANALYSER1

#include "../../ast/ASTNodeTypes.hpp"
#include "analyserUtil.hpp"
#include "unordered_map"

class lattice
{  
public:
  enum PointType{
    CPU_ONLY,
    GPU_ONLY,
    CPU_GPU_SHARED,
  };

private:
  unordered_map<TableEntry*, PointType> typeMap;

public:
  lattice(){}

  lattice(list<Identifier*> &initVars){
    for(Identifier* iden: initVars)
      addVariable(iden);
  }

  lattice(lattice &l1){
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
      typeMap[pr.first] = pr.second;
  }

  //TODO : Needs to be changed
  PointType meet(PointType p1, PointType p2)
  {
    if(p1 == p2) return p1;
    else if((p1 == CPU_ONLY) && (p2 == GPU_ONLY)) return CPU_GPU_SHARED;
    else if((p1 == GPU_ONLY) && (p2 == CPU_ONLY)) return CPU_GPU_SHARED;
    else if(p1 == CPU_GPU_SHARED) return p2;
    else return p1;
  }

  void operator ^= (lattice &l1)
  {
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
    {
      if(typeMap.find(pr.first) != typeMap.end())
        typeMap[pr.first] = meet(typeMap[pr.first], pr.second);
    }
  }

  void meet(Identifier* iden, PointType type)
  {
    TableEntry* symbInfo = iden->getSymbolInfo();
    if(typeMap.find(symbInfo) != typeMap.end()){
      // TODO : Add code for meet of symbol info
    }
  }

  void addVariable(Identifier* iden, PointType type = CPU_ONLY)
  {
    TableEntry* symbInfo = iden->getSymbolInfo();
    typeMap.insert({symbInfo, type});
  }

  void removeVariable(Identifier* iden)
  {
    TableEntry* symbInfo = iden->getSymbolInfo();
    typeMap.erase(symbInfo);
  }
};


class deviceVarsAnalyser
{
  private:
  class ASTNodeWrap
  {
    public:
    list<ASTNode*> predecessor, successor;
    lattice inMap, outMap;
    ASTNode* currStmt;
  };

  unordered_map<ASTNode*, ASTNodeWrap*> latticeMap;

  ASTNodeWrap* getWrapNode(ASTNode* node, lattice &inLattice)
  {
    if(latticeMap.find(node) == latticeMap.end())
    {
      ASTNodeWrap* wrapNode = new ASTNodeWrap();
      wrapNode->currStmt = node;
      wrapNode->inMap = inLattice;

      latticeMap.insert({node, wrapNode});
    }

    return latticeMap.at(node);
  }

  public:
  deviceVarsAnalyser(){
  }

  void analyse();
  void analyseFunc(ASTNode* proc);
  void analyseStatement(statement* stmt);
  void analyseBlock(blockStatement* blockStmt);
  void analyseUnary(unary_stmt* blockStmt);
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
