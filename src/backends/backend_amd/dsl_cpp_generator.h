#ifndef AMD_DSL_CPP_GENERATOR
#define AMD_DSL_CPP_GENERATOR

#include <cstdio>
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
#include "../dslCodePad.h"
#include "../../analyser/analyserUtil.cpp"

namespace spamd
{
class dsl_cpp_generator {
 private:
  dslCodePad header;
  dslCodePad main;
  dslCodePad kernel;
  FILE* headerFile;
  FILE* bodyFile;
  FILE* kernelFile;
  char* fileName;

  // added here
  bool genCSR;
  char* gName;
  int kernelCount;
  Function* currentFunc;


  bool isHeader;
  /*******************************amd specific**********************/
    int memObjectCount;
    vector<char*>memObjects;
    vector<char*>kernelObjects;
    vector<char*>hostMemObjects;
    vector<Identifier*> PremVarOfKernel; //premitive variables which are passed as kernel argument to any kernel
  /***************************************************************/
 public:
  dsl_cpp_generator() {
    // added here
    genCSR = false;
    isHeader = false;
    kernelCount = 0;
    memObjectCount = 0; // amd specific
    gName = new char[25];

    headerFile = NULL;
    bodyFile = NULL;
    kernelFile = NULL;
    fileName = new char[1024];
    currentFunc = NULL;

  }


  /**************************************************************amd*************************/
  void generateGetPlatforms();
  void generateGetDevices();
  void generateCreateContext();
  void generateCreateCommandQueue();
  dslCodePad& getTargetFile(int isMainFile);
  void checkForAllAndGenerate(blockStatement* blockStmt , int isMainFile);
  void addMemObject( char *str);
  void addKernelObject(char* str);
  bool isMemPresent(char* str);
  bool isKenelPresent(char* str);
  void releaseObjects(int isMainFile);
  void addProfilling(const char* eventName);
  const char* getAtomicFromType(Type *typeofVar, int atomicType);
  // void generateReverseBFS(iterateReverseBFS* stmt, int isMainFile);
  /********************************************End AMD****************************************/
  void generateParamList(list<formalParam*> paramList, dslCodePad& targetFile);
  void setCurrentFunc(Function* func);
  Function* getCurrentFunc();
  void setFileName(char* f);
  bool generate();
  void generation_begin();
  void generation_end();
  bool openFileforOutput();
  void closeOutputFile();
  const char* convertToCppType(Type* type);
  const char* getOperatorString(int operatorId);
  void generateFunc(ASTNode* proc);
  void generateFuncHeader(Function* proc, int isMainFile);
  void generateProcCall(proc_callStmt* procCall, int isMainFile);
  void generateVariableDecl(declaration* decl, int isMainFile);
  void generateStatement(statement* stmt, int isMainFile);
  //void generateAssignmentStmt(assignment* assignStmt);
  void generateAssignmentStmt(assignment* assignStmt, int isMainFile);
  void generateWhileStmt(whileStmt* whilestmt);
  void generateForAll(forallStmt* forAll, int isMainFile);
  void generateFixedPoint(fixedPointStmt* fixedPoint, int isMainFile);
  void generateIfStmt(ifStmt* ifstmt, int isMainFile);
  void generateDoWhileStmt(dowhileStmt* doWhile, int isMainFile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace = true,int isMainFile = 1);
  void generateReductionStmt(reductionCallStmt* reductnStmt, int isMainFile);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction, int isMainFile);
  void generateRevBFSAbstraction(iterateBFS* bfsAbstraction,
                                 int isMainFile);  // reverse


  void generateExpr(Expression* expr, int isMainFile, bool isAtomic = false);
  void generate_exprArL(Expression* expr, int isMainFile, bool isAtomic = false);

  void generate_exprRelational(Expression* expr, int isMainFile);
  void generate_exprInfinity(Expression* expr, int isMainFile);
  void generate_exprLiteral(Expression* expr, int isMainFile);
  void generate_exprIdentifier(Identifier* id, int isMainFile);
  void generate_exprPropId(PropAccess* propId, int isMainFile);
  void generate_exprProcCall(Expression* expr, int isMainFile);

  void generateForAll_header();
  void generateForAllSignature(forallStmt* forAll, int ismainFile);
  // void includeIfToBlock(forallStmt* forAll);
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  blockStatement* includeIfToBlock(forallStmt* forAll);

  void generateId();
  void generateOid();
  void addIncludeToFile(const char* includeName, dslCodePad& file,
                        bool isCPPLib);
  void generatePropertyDefination(Type* type, char* Id, int isMainFile);
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
  void generateAtomicBlock(int isMainFile);
  void generateVariableDeclForEdge(declaration* declStmt);
  void generateLocalInitForID();
  // new
  void castIfRequired(Type* type, Identifier* methodID, dslCodePad& main);
  void generateReductionCallStmt(reductionCallStmt* stmt, int isMainFile);
  void generateReductionOpStmt(reductionCallStmt* stmt, int isMainFile);
  void generate_exprUnary(Expression* expr, int isMainFile);
  void generateForAll_header(forallStmt* forAll, int isMainFile);
  void generatefixedpt_filter(Expression* filterExpr, int isMainFile);

  bool elementsIteration(char* extractId);
  //~ void generateCudaMalloc(Type* type, const char* identifier);
  // void generateCudaMalloc(Type* type, const char* identifier, bool
  // isMainFile);
  void generateCudaMemcpy(const char* dVar, const char* cVar,
                          const char* typeStr, const char* sizeOfType,
                          int isMainFile, const char* from);

  // for algorithm specific function implementation headers

  void generatePrintAnswer();
  void generateGPUTimerStop();
  void generateGPUTimerStart();
  void generateCudaMemcpyStr(const char*, const char*, const char*, const char*,
                             bool);
  void generateInitkernel1(assignment*, int);

  void generateInitkernelStr(const char* inVarType, const char* inVarName, const char* initVal, const char* sizeVE);

  void generateCSRArrays(const char*);
  void generateInitkernel(const char* name);
  void generateLaunchConfig(const char* name);
  void generateCudaDeviceSync();
  void generateForKernel();
  void generateForKernel(Identifier*);
  void generateCudaIndex(const char* idName);
  void generateCudaMemCpySymbol(char* var, const char* typeStr, bool direction);
  void generateCudaMalloc(Type* type, const char* identifier);
  void generateCudaMallocStr(const char* dVar, const char* type,
                             const char* sizeOfType);
  void generateThreadId(const char* str);
  void generateFuncBody(Function* proc, int isMainFile);

  void generateStartTimer();
  void generateStopTimer();

  void addCudaRevBFSIterationLoop(iterateBFS* bfsAbstraction);
  void addCudaBFSIterationLoop(iterateBFS* bfsAbstraction);
  void generateExtraDeviceVariable(const char* typeStr, const char* dVar, const char* sizeVal);
  void addCudaBFSIterKernel(iterateBFS* bfsAbstraction);
  void addCudaRevBFSIterKernel(iterateBFS* bfsAbstraction);
  void generateAtomicDeviceAssignmentStmt(assignment* asmt, int isMainFile);
  void generateDeviceAssignmentStmt(assignment* asmt, int isMainFile);
  void addCudaKernel(forallStmt* forAll);
  void generateCallList(list<formalParam*> paramList, dslCodePad& targetFile);
  void generateCudaMallocParams(list<formalParam*> paramList);
  void generateCudaMemCpyParams(list<formalParam*> paramList);
  void generateHeaderDeviceVariable(const char* typeStr, const char* dVar);
  void generateExtraDeviceVariableNoD(const char* typeStr, const char* dVar, const char* sizeVal);
  void generatePropParams(list<formalParam*> paramList, char *kernelName, bool isNeedType,int isMainFile,  int argnum);
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

}
#endif
