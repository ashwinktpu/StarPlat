#ifndef DEVICE_VARS_ANALYSER
#define DEVICE_VARS_ANALYSER

#include <unordered_map>
//#include "../../ast/ASTNodeTypes.hpp"
//#include "../../maincontext/MainContext.hpp"
#include "../analyserUtil.cpp"
#include "../../ast/ASTHelper.cpp"
//#include "../../symbolutil/SymbolTable.h"

class lattice
{ 
public:
  //Location of a variable
  enum PointType{
    NOT_INITIALIZED,
    CPU_GPU_SHARED,
    GPU_ONLY,
    CPU_ONLY,
  };

  //Variable access location and type
  enum AccessType{
    CPU_READ,
    GPU_READ,
    CPU_WRITE,
    GPU_WRITE,
  };

  enum LatticeType{
    CPU_Preferenced,
    GPU_Preferenced,
  };

private:
  unordered_map<TableEntry*, PointType> typeMap;

public:
  lattice(){}

  lattice(list<Identifier*> &initVars){
    for(Identifier* iden: initVars)
      addVariable(iden);
  }

  lattice(const lattice &l1){
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
      typeMap[pr.first] = pr.second;
  }

  PointType meet(PointType p1, PointType p2, LatticeType type){
    if(type == CPU_Preferenced)
    {
      switch (p1)
      {
      case NOT_INITIALIZED:
      case CPU_GPU_SHARED:
        return p2;
      case GPU_ONLY:
        return (p2 == CPU_ONLY) ? CPU_ONLY : GPU_ONLY;
      case CPU_ONLY:
        return CPU_ONLY;
      }
    }
    else
    {
      switch (p1)
      {
      case NOT_INITIALIZED:
      case CPU_GPU_SHARED:
        return p2;
      case CPU_ONLY:
        return (p2 == GPU_ONLY) ? GPU_ONLY : CPU_ONLY;
      case GPU_ONLY:
        return GPU_ONLY;
      }
    }
    //~ std::cout<< "SHOULD NEVER BE REACHED" << '\n';
    return (PointType());
  }

  //CPU preferenced meet
  void operator ^= (lattice &l1)
  {
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
    {
      if(typeMap.find(pr.first) != typeMap.end())
        typeMap[pr.first] = meet(typeMap[pr.first], pr.second, CPU_Preferenced);
    }
  }

  /*
  void operator &= (lattice &l1)
  {
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
    {
      if(typeMap.find(pr.first) != typeMap.end())
        typeMap[pr.first] = meet(typeMap[pr.first], pr.second, GPU_Preferenced);
    }
  }*/

  //CPU preferenced meet
  lattice operator^ (const lattice &l1)
  {
    lattice out;
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
    {
      if(typeMap.find(pr.first) != typeMap.end())
        out.typeMap[pr.first] = meet(typeMap[pr.first], pr.second, CPU_Preferenced);
    }
    return out;
  }

  //GPU preferenced meet
  lattice operator& (const lattice &l1)
  {
    lattice out;
    for(pair<TableEntry*, PointType> pr: l1.typeMap)
    {
      if(typeMap.find(pr.first) != typeMap.end())
        out.typeMap[pr.first] = meet(typeMap[pr.first], pr.second, GPU_Preferenced);
    }
    return out;
  }

  bool operator== (const lattice &l1)
  {
    for(pair<TableEntry*, PointType> pr: typeMap)
    {
      if(l1.typeMap.at(pr.first) != pr.second)
        return false;
    }
    return true;
  }

  bool operator!= (const lattice &l1)
  {
    for(pair<TableEntry*, PointType> pr: typeMap)
    {
      if(l1.typeMap.at(pr.first) != pr.second)
        return true;
    }
    return false;
  }

  void meet(Identifier* iden, AccessType acType)
  {
    TableEntry* symbInfo = iden->getSymbolInfo();
    if(typeMap.find(symbInfo) != typeMap.end())
    {
      PointType cType = typeMap.at(symbInfo);
      switch (cType)
      {
      case CPU_GPU_SHARED:
        switch (acType)
        {
        case CPU_READ:
          typeMap[symbInfo] = CPU_GPU_SHARED;
          break;
        case CPU_WRITE:
          typeMap[symbInfo] = CPU_ONLY;
          break;
        case GPU_READ:
          typeMap[symbInfo] = CPU_GPU_SHARED;
          break;
        case GPU_WRITE:
          typeMap[symbInfo] = GPU_ONLY;
          break;
        }
        break;
      
      case GPU_ONLY:
        switch (acType)
        {
        case CPU_READ:
          typeMap[symbInfo] = CPU_GPU_SHARED;
          break;
        case CPU_WRITE:
          typeMap[symbInfo] = CPU_ONLY;
          break;
        case GPU_READ:
          typeMap[symbInfo] = GPU_ONLY;
          break;
        case GPU_WRITE:
          typeMap[symbInfo] = GPU_ONLY;
          break;
        }
        break;
      
      case CPU_ONLY:
        switch (acType)
        {
        case CPU_READ:
          typeMap[symbInfo] = CPU_ONLY;
          break;
        case CPU_WRITE:
          typeMap[symbInfo] = CPU_ONLY;
          break;
        case GPU_READ:
          typeMap[symbInfo] = CPU_GPU_SHARED;
          break;
        case GPU_WRITE:
          typeMap[symbInfo] = GPU_ONLY;
          break;
        }
        break;
      }
    }
  }

  void addVariable(Identifier* iden, PointType type = NOT_INITIALIZED)
  {
    TableEntry* symbInfo = iden->getSymbolInfo();
    typeMap.insert({symbInfo, type});
  }

  void removeVariable(Identifier* iden)
  {
    TableEntry* symbInfo = iden->getSymbolInfo();
    typeMap.erase(symbInfo);
  }

  void print()
  {
    for(pair<TableEntry*, PointType> pr: typeMap)
    {
      string type;
      if(pr.second == CPU_GPU_SHARED)
        type = "CPU_GPU_SHARED";
      else if(pr.second == GPU_ONLY)
        type = "GPU_ONLY";
      else if(pr.second == CPU_ONLY)
        type = "CPU_ONLY";
      else
        type = "NOT INIT";

      cout<<string(pr.first->getId()->getIdentifier())<<": "<<type<<' ';
    }

    cout<<endl;
  }

  void setVarType(Identifier* iden, PointType type){
    TableEntry* symbInfo = iden->getSymbolInfo();
    this->typeMap[symbInfo] = type;
  }

  unordered_map<TableEntry*, PointType> getLattice(){
    return typeMap;
  }
};

class ASTNodeWrap
{
  public:
  usedVariables usedVars;
  lattice inMap, outMap;
  ASTNode* currStmt;
  bool hasForAll;
};

class deviceVarsAnalyser
{
  private:
  unordered_map<ASTNode*, ASTNodeWrap*> latticeMap;
  usedVariables gpuUsedVars;
  int tempVar = 0;

  ASTNodeWrap* initWrapNode(ASTNode* node, list<Identifier*> &vars)
  {
    ASTNodeWrap* wrapNode = new ASTNodeWrap();
    wrapNode->inMap = lattice(vars);
    wrapNode->outMap = lattice(vars);
    wrapNode->currStmt = node;

    latticeMap.insert(make_pair(node, wrapNode));
    return wrapNode;
  }

  ASTNodeWrap* getWrapNode(ASTNode* node){
    return latticeMap.at(node);
  }

  char* getTempVar()
  {
    string var = "tempVar_" + to_string(tempVar++);
    char* temp = new char[var.length() + 1];
    strcpy(temp, var.c_str());
    return temp;
  }

  public:
  deviceVarsAnalyser(){
  }

  void analyse(list<Function*> funcList);
  void analyseFunc(ASTNode* proc);

  //Forward datapropogation analysis on each ASTNodeWrap
  lattice analyseStatement(statement* stmt, lattice&);
  lattice analyseBlock(blockStatement* blockStmt, lattice&);
  lattice analyseUnary(unary_stmt* blockStmt, lattice&);
  lattice analyseIfElse(ifStmt* stmt, lattice&);
  lattice analyseAssignment(assignment* stmt, lattice&);
  lattice analyseDeclaration(declaration*, lattice&);
  lattice analyseForAll(forallStmt*, lattice&);
  lattice analyseWhile(whileStmt*, lattice&);
  lattice analyseDoWhile(dowhileStmt*, lattice&);
  lattice analyseFor(forallStmt*, lattice&);
  lattice analyseFixedPoint(fixedPointStmt*, lattice&);
  lattice analyseProcCall(proc_callStmt*, lattice&);
  lattice analyseReduction(reductionCallStmt*, lattice&);
  lattice analyseItrBFS(iterateBFS*, lattice&);

  //Initializes ASTNodeWrap for each statment and collects GPU used variables
  bool initBlock(blockStatement* blockStmt, list<Identifier*> &);
  bool initStatement(statement* stmt, list<Identifier*> &);
  bool initUnary(unary_stmt* blockStmt, list<Identifier*> &);
  bool initIfElse(ifStmt* stmt, list<Identifier*> &);
  bool initAssignment(assignment* stmt, list<Identifier*> &);
  bool initDeclaration(declaration*, list<Identifier*> &);
  bool initForAll(forallStmt*, list<Identifier*> &);
  bool initWhile(whileStmt*, list<Identifier*> &);
  bool initDoWhile(dowhileStmt*, list<Identifier*> &);
  bool initFor(forallStmt*, list<Identifier*> &);
  bool initFixedPoint(fixedPointStmt*, list<Identifier*> &);
  bool initProcCall(proc_callStmt*, list<Identifier*> &);
  bool initReduction(reductionCallStmt*, list<Identifier*> &);
  bool initItrBFS(iterateBFS*, list<Identifier*> &);

  void printStatement(statement* , int);
  void printBlock(blockStatement* , int);
  void printUnary(unary_stmt* , int);
  void printIfElse(ifStmt* , int);
  void printAssignment(assignment* , int);
  void printDeclaration(declaration*, int);
  void printForAll(forallStmt*, int);

  //Returns the used variables in each statment
  usedVariables getVarsStatement(statement* stmt);
  usedVariables getVarsBlock(blockStatement* stmt);
  usedVariables getVarsForAll(forallStmt* stmt);
  usedVariables getVarsUnary(unary_stmt* stmt);
  usedVariables getVarsDeclaration(declaration* stmt);
  usedVariables getVarsWhile(whileStmt* stmt);
  usedVariables getVarsDoWhile(dowhileStmt* stmt);
  usedVariables getVarsAssignment(assignment* stmt);
  usedVariables getVarsIf(ifStmt* stmt);
  usedVariables getVarsExpr(Expression* stmt);
  usedVariables getVarsReduction(reductionCallStmt *stmt);

  /*
  Adds variable transfers at required position
  Flags the GPU used variables
  Initialized the parameters required to transfer to GPU
  */
  statement* transferVarsStatement(statement* stmt,blockStatement* parBlock);
  statement* transferVarsForAll(forallStmt* stmt,blockStatement* parBlock);
  statement* transferVarsFor(forallStmt* stmt,blockStatement* parBlock);
  statement* transferVarsUnary(unary_stmt* stmt,blockStatement* parBlock);
  statement* transferVarsBlock(blockStatement* stmt,blockStatement* parBlock);
  statement* transferVarsDeclaration(declaration* stmt,blockStatement* parBlock);
  statement* transferVarsWhile(whileStmt* stmt,blockStatement* parBlock);
  statement* transferVarsDoWhile(dowhileStmt* stmt,blockStatement* parBlock);
  statement* transferVarsAssignment(assignment* stmt,blockStatement* parBlock);
  statement* transferVarsIfElse(ifStmt* stmt,blockStatement* parBlock);
  statement* transferVarsFixedPoint(fixedPointStmt* stmt,blockStatement* parBlock);
  statement* transferVarsProcCall(proc_callStmt* stmt, blockStatement* parBlock);
  statement* transferVarsReduction(reductionCallStmt* stmt, blockStatement* parBlock);
  statement* transferVarsItrBFS(iterateBFS* stmt, blockStatement* parBlock);
};

#endif
