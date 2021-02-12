/*#ifndef DSL_CPP_GENERATOR
#define DSL_CPP_GENERATOR

#include <cstdio>
#include "dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"

class dsl_cpp_generator
{
  private:

  dslCodePad header;
  dslCodePad main;
  FILE *headerFile;
  FILE *bodyFile;
  char* fileName;

  public:
  void generate();
  void generation_begin();
  void generation_end();
  bool openFileforOutput();
  void closeOutputFile();
  void generateFunc(ASTNode* proc);
  void generateFuncDecl();
  void generateProcCall(proc_callStmt* procCall);
  void generateVariableDecl(declaration* decl);
  void generateAssignmentStmt(assignment* assignStmt);
  void generateWhileStmt(whileStmt* while);
  void generateForAll(forallStmt*) forAll);
  void generateFixedPoint((fixedPointStmt*)fixedPoint);
  void generateIfStmt((ifStmt*)if);
  void generateDoWhileStmt((dowhileStmt*)doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);
  void generateId();
  void generateOid();
  void addIncludeToFile(char* includeName,dslCodePad file,bool isCPPLib);
  void generatePropertyDefination(Type* type,char* Id);
  void findTargetGraph(vector<Type*> graphTypes,Type* type);


};

static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
#endif
*/