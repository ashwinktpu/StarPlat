#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"

#ifndef MPI_DSL_CPP_GENERATOR
#define MPI_DSL_CPP_GENERATOR

namespace spmpi {

class dsl_cpp_generator
{
  protected:
    dslCodePad header;
    dslCodePad main;
    FILE *headerFile;
    FILE *bodyFile;
    char* fileName;
    Function* currentFunc;
    int genFuncCount;
    int staticFuncCount;
    int inFuncCount;
    int decFuncCount;
    int dynFuncCount;
    int curFuncType;
    vector<pair<Identifier*, proc_callExpr*>> forallStack;
    std::vector<statement*> insideParallelConstruct;
    fixedPointStmt* fixedPointEnv;
    //bool isOptimized;
  public:
  dsl_cpp_generator()
  {
    //std::cout << "***********************Inside construtor" << std::endl;
    headerFile=NULL;
    bodyFile=NULL;
    fileName=new char[1024];
    genFuncCount = 0;
    staticFuncCount = 0;
    inFuncCount = 0;
    decFuncCount = 0;
    dynFuncCount = 0;
    
    fixedPointEnv = NULL;
    printf("set to null\n");
    //isOptimized = false;
  }
  const char * getMPItype(Type * type);
  const char * getMPIreduction(int operatorId);
  const char * getReductionOperatorString(int reductionType);
  void setFileName(char* f);
  bool generate();
  virtual void generation_begin();
  void generation_end();
  virtual bool openFileforOutput();
  void closeOutputFile();
  const char* convertToCppType(Type* type, bool is_reference = false);
  void castIfRequired(Type* type, Identifier* methodID, dslCodePad& main);
  const char* getOperatorString(int operatorId);
  virtual void generateFunc(ASTNode* proc);
  void generateFuncHeader(Function* proc, bool isMainFile);
  void generateProcCall(proc_callStmt* procCall);
  void generateVariableDecl(declaration* decl);
  virtual void generateStatement(statement* stmt);
  void generateAssignmentStmt(assignment* assignStmt);
  void generateWhileStmt(whileStmt* whilestmt);
  void generateForAll(forallStmt* forAll);
  void generateFixedPoint(fixedPointStmt* fixedPoint);
  void generateIfStmt(ifStmt* ifstmt);
  void generateDoWhileStmt(dowhileStmt* doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace = true);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateReductionCallStmt(reductionCallStmt* stmt);
  void generateReductionOpStmt(reductionCallStmt* stmt);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);

  // functions related to expression generation
  void generateExpr(Expression* expr);
  void generate_exprRelational(Expression* expr);
  void generate_exprInfinity(Expression* expr);
  void generate_exprLiteral(Expression* expr);
  void generate_exprIdentifier(Identifier* id);
  void generate_exprPropId(PropAccess* propId);  //not yet implemented
  //void generate_exprPropIdReceive(PropAccess* propId);
  virtual void generate_exprProcCall(Expression* expr);
  void generate_exprArL(Expression* expr);
  void generate_exprUnary(Expression* expr);
  void generate_exprIndexExpr(Expression* expr);

  void generateForAll_header(forallStmt* forAll);
  void getEdgeTranslation(Expression* expr);  //translation of edge assignment.
  void generateForAllSignature(forallStmt* forAll);
  void generatefixedpt_filter(Expression* filterExpr, bool & filter_generated);
  void generateParamList(list<formalParam*> paramList, dslCodePad& targetFile);
  
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  bool elementsIteration(char* extractId);
  void generateArgList(list<argument*> argList);

  blockStatement* includeIfToBlock(forallStmt* forAll);

  void generateId();
  void generateOid();
  void addIncludeToFile(const char* includeName, dslCodePad& file, bool isCPPLib);
  void findTargetGraph(vector<Identifier*> graphTypes, Type* type);
  void incFuncCount(int funcType);
  int curFuncCount();
  void getDefaultValueforTypes(int type);
  void setCurrentFunc(Function* func);
  Function* getCurrentFunc();
  void generateFixedPointUpdate(PropAccess* propId);
  bool checkFixedPointAssociation(PropAccess* propId);
  void checkAndGenerateFixedPtFilter(forallStmt* forAll);
  //void setOptimized() { isOptimized = true; }

};
static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
static const char* FLOATALLOCATION = "new float";
static const char* DOUBLEALLOCATION = "new double";

}   //spmpi

#endif
