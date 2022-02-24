#ifndef DSL_CPP_GENERATOR
#define DSL_CPP_GENERATOR

#include <cstdio>
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
#include "../dslCodePad.h"
//#include "dslCodePad.h"
//~ struct vars {
//~ string varType;
//~ string varName;
//~ bool result;
//~ vars(){}
//~ vars(string vType,string vName,bool res) {
//~ varType=vType;
//~ varName=vName;
//~ result=res;
//~ }
//~ };

class dsl_cpp_generator {
 private:
  dslCodePad header;
  dslCodePad main;
  FILE* headerFile;
  FILE* bodyFile;
  char* fileName;

  // added here
  bool genCSR;
  char* gName;
  int kernelCount;

  //~ vector<vars> varList;
  //~ std::vector <string> vvList;
  //~ vector<string> varList;
  std::vector<std::pair<string, string>> variableList;
  bool isHeader;

 public:
  dsl_cpp_generator() {
    // added here
    genCSR = false;
    isHeader = false;
    kernelCount = 0;
    gName = new char[25];

    headerFile = NULL;
    bodyFile = NULL;
    fileName = new char[1024];
    //~ varList.clear();
  }

  //~ ~dsl_cpp_generator(){
  //~ vList.clear();
  //~ vvList.clear();
  //~ }

  void setFileName(char* f);
  bool generate();
  void generation_begin();
  void generation_end();
  bool openFileforOutput();
  void closeOutputFile();
  const char* convertToCppType(Type* type);
  const char* getOperatorString(int operatorId);
  void generateFunc(ASTNode* proc);
  void generateFuncHeader(Function* proc, bool isMainFile);
  void generateProcCall(proc_callStmt* procCall, bool isMainFile);
  void generateVariableDecl(declaration* decl, bool isMainFile);
  void generateStatement(statement* stmt, bool isMainFile);
  // void generateAssignmentStmt(assignment* assignStmt);
  void generateAssignmentStmt(assignment* assignStmt, bool isMainFile);
  void generateWhileStmt(whileStmt* whilestmt);
  void generateForAll(forallStmt* forAll, bool isMainFile);
  void generateFixedPoint(fixedPointStmt* fixedPoint, bool isMainFile);
  void generateIfStmt(ifStmt* ifstmt, bool isMainFile);
  void generateDoWhileStmt(dowhileStmt* doWhile, bool isMainFile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace = true,
                     bool isMainFile = true);
  void generateReductionStmt(reductionCallStmt* reductnStmt, bool isMainFile);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction, bool isMainFile);
  void generateRevBFSAbstraction(iterateBFS* bfsAbstraction,
                                 bool isMainFile);  // reverse

  void generateExpr(Expression* expr, bool isMainFile, bool isAtomic = false);
  void generate_exprArL(Expression* expr, bool isMainFile, bool isAtomic = false);

  void generate_exprRelational(Expression* expr, bool isMainFile);
  void generate_exprInfinity(Expression* expr, bool isMainFile);
  void generate_exprLiteral(Expression* expr, bool isMainFile);
  void generate_exprIdentifier(Identifier* id, bool isMainFile);
  void generate_exprPropId(PropAccess* propId, bool isMainFile);
  void generate_exprProcCall(Expression* expr, bool isMainFile);

  void generateForAll_header();
  void generateForAllSignature(forallStmt* forAll, bool isKernel);
  // void includeIfToBlock(forallStmt* forAll);
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  blockStatement* includeIfToBlock(forallStmt* forAll);

  void generateId();
  void generateOid();
  void addIncludeToFile(const char* includeName, dslCodePad& file,
                        bool isCPPLib);
  void generatePropertyDefination(Type* type, char* Id, bool isMainFile);
  void findTargetGraph(vector<Identifier*> graphTypes, Type* type);
  void getDefaultValueforTypes(int type);

  void generateInitialization();
  void generateFuncSSSPBody();
  void generateFuncCUDASizeAllocation();
  void generateFuncCudaMalloc();
  void generateBlockForSSSPBody(blockStatement* blockStmt, bool includeBrace);
  void generateStatementForSSSPBody(statement* stmt);
  void generateCudaMemCpyForSSSPBody();
  void generateFuncPrintingSSSPOutput();
  void generateFuncVariableINITForSSSP();
  void generateFuncTerminatingConditionForSSSP();
  // newly added for cuda speific handlings index
  void generateCudaIndex();
  void generateAtomicBlock(bool isMainFile);
  void generateVariableDeclForEdge(declaration* declStmt);
  void generateLocalInitForID();
  // new
  void castIfRequired(Type* type, Identifier* methodID, dslCodePad& main);
  void generateReductionCallStmt(reductionCallStmt* stmt, bool isMainFile);
  void generateReductionOpStmt(reductionCallStmt* stmt, bool isMainFile);
  void generate_exprUnary(Expression* expr, bool isMainFile);
  void generateForAll_header(forallStmt* forAll, bool isMainFile);
  void generatefixedpt_filter(Expression* filterExpr, bool isMainFile);

  bool elementsIteration(char* extractId);
  //~ void generateCudaMalloc(Type* type, const char* identifier);
  // void generateCudaMalloc(Type* type, const char* identifier, bool
  // isMainFile);
  void generateCudaMemcpy(const char* dVar, const char* cVar,
                          const char* typeStr, const char* sizeOfType,
                          bool isMainFile, const char* from);

  // for algorithm specific function implementation headers

  void generatePrintAnswer();
  void generateGPUTimerStop();
  void generateGPUTimerStart();
  void generateCudaMemCpyStr(const char*, const char*, const char*, const char*,
                             bool);
  void generateInitkernel1(assignment*, bool);

  void generateInitkernelStr(const char* inVarType, const char* inVarName, const char* initVal);

  void generateCSRArrays(const char*);
  void generateInitkernel(const char* name);
  void generateLaunchConfig(const char* name);
  void generateCudaDeviceSync();
  void generateForKernel();
  void generateForKernel(Identifier*);
  void generateCudaIndex(const char* idName);
  void generateCudaMalloc(Type* type, const char* identifier);
  void generateCudaMallocStr(const char* dVar, const char* type,
                             const char* sizeOfType);
  void generateThreadId(const char* str);
  void generateFuncBody(Function* proc, bool isMainFile);

  void generateStartTimer();
  void generateStopTimer();

  void addCudaRevBFSIterationLoop(iterateBFS* bfsAbstraction);
  void addCudaBFSIterationLoop(iterateBFS* bfsAbstraction);
  void generateExtraDeviceVariable(const char* typeStr, const char* dVar, const char* sizeVal);
  void addCudaBFSIterKernel(iterateBFS* bfsAbstraction);
  void addCudaRevBFSIterKernel(list<statement*>& revStmtList);
  void generateAtomicDeviceAssignmentStmt(assignment* asmt, bool isMainFile);
  void generateDeviceAssignmentStmt(assignment* asmt, bool isMainFile);

  //~ void setGenCSR(bool yes = true) { genCSR = yes; }
  //~ bool isGenCSR() { return genCSR; }
  //~ void setGName(const char* str) {
  //~ strlen(str);
  //~ strcpy(gName, str);
  //~ }
  //~ char* getGName() { return gName; }
  void IncrementKCount() { kernelCount++; }
  int getKCount() { return kernelCount; }
};

static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
static const char* FLOATALLOCATION = "new float";
static const char* DOUBLEALLOCATION = "new double";
#endif
