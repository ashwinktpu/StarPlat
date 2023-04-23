#include <cstdio>

#include "../../ast/ASTNodeTypes.hpp"

#ifndef DSL_CPP_GENERATOR
#define DSL_CPP_GENERATOR

#include "../dslCodePad.h"
//#include "../../ast/ASTHelper.cpp"

namespace spomp {

class dsl_cpp_generator {
 protected:
  dslCodePad header;
  dslCodePad main;
  dslCodePad cl;
  FILE *headerFile;
  FILE *bodyFile;
  FILE *clFile;
  char* fileName;
  int genFuncCount;
  int staticFuncCount;
  int inFuncCount;
  int decFuncCount;
  int dynFuncCount;
  int curFuncType;
  Function* currentFunc;
  vector<pair<Identifier*, proc_callExpr*>> forallStack;
  fixedPointStmt* fixedPointEnv;
  vector<vector<Identifier*>> freeIdStore;
  bool insidePreprocessEnv;
  bool insideBatchBlock ;
  vector<ASTNode*> parallelConstruct;
  bool isOptimized;

  
 
  public:
  dsl_cpp_generator()
  {
    headerFile=NULL;
    bodyFile=NULL;
    clFile = NULL;
    fixedPointEnv = NULL;
    fileName = new char[1024];
    genFuncCount = 0;
    staticFuncCount = 0;
    inFuncCount = 0;
    decFuncCount = 0;
    dynFuncCount = 0;
    insideBatchBlock = false;
    insidePreprocessEnv = false;
    isOptimized = false;
  }

  void setFileName(char* f);
  bool generate();
  void generation_begin();
  void generation_end();
  bool openFileforOutput();
  void closeOutputFile();
  const char* convertToCppType(Type* type);
  void castIfRequired(Type* type, Identifier* methodID, dslCodePad& main);
  const char* getOperatorString(int operatorId);
  void generateFunc(ASTNode* proc);
  void generateFuncHeader(Function* proc, bool isMainFile);
  void generateProcCall(proc_callStmt* procCall);
  void generateVariableDecl(declaration* decl);
  virtual void generateStatement(statement* stmt);
  void generateAssignmentStmt(assignment* assignStmt);
  void generateWhileStmt(whileStmt* whilestmt);
  virtual void generateForAll(forallStmt* forAll);
  void generateFixedPoint(fixedPointStmt* fixedPoint);
  void generateIfStmt(ifStmt* ifstmt);
  void generateDoWhileStmt(dowhileStmt* doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace = true);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateReductionCallStmt(reductionCallStmt* stmt);
  void generateReductionOpStmt(reductionCallStmt* stmt);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);
  void generateExpr(Expression* expr);
  void generate_exprRelational(Expression* expr);
  void generate_exprInfinity(Expression* expr);
  void generate_exprLiteral(Expression* expr);
  void generate_exprIdentifier(Identifier* id);
  virtual void generate_exprPropId(PropAccess* propId);
  void generate_exprPropIdReceive(PropAccess* propId);
  virtual void generate_exprProcCall(Expression* expr);
  void generate_exprArL(Expression* expr);
  void generate_exprUnary(Expression* expr);
  void generate_exprIndex(Expression* expr, bool islocal);
  void generateForAll_header(forallStmt* forAll);
  void getEdgeTranslation(Expression* expr);  //translation of edge assignment.
  virtual void generateForAllSignature(forallStmt* forAll);
  void generatefixedpt_filter(Expression* filterExpr);
  void generateParamList(list<formalParam*> paramList, dslCodePad& targetFile);
  //void includeIfToBlock(forallStmt* forAll);
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  bool elementsIteration(char* extractId);
  void generateArgList(list<argument*> argList, bool addBraces);    
  void generateNestedContainer(Type* type);
  void generateFreeInCurrentBatch();
  void generatePriorDeclarations(Function* proc);

  blockStatement* includeIfToBlock(forallStmt* forAll);
  Expression* associateIterNodeToProp(Expression* filterExpr, Identifier* iterNode);

  void generateId();
  void generateOid();
  void addIncludeToFile(const char* includeName, dslCodePad& file, bool isCPPLib);
  void generatePropertyDefination(Type* type, char* Id);
  void findTargetGraph(vector<Identifier*> graphTypes, Type* type);
  void incFuncCount(int funcType);
  int curFuncCount();
  void getDefaultValueforTypes(int type);
  void setCurrentFunc(Function* func);
  Function* getCurrentFunc();
  void generateFixedPointUpdate(PropAccess* propId);
  bool checkFixedPointAssociation(PropAccess* propId);
  void checkAndGenerateFixedPtFilter(forallStmt* forAll);
  void setOptimized() { isOptimized = true; }
  void generateForMergeContainer(Type* type, int& level);
  void generateInserts(Type* type,  Identifier* id);
  string  getProcName(proc_callExpr* proc);


};

static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
static const char* FLOATALLOCATION = "new float";
static const char* DOUBLEALLOCATION = "new double";
}  // namespace spomp
#endif
