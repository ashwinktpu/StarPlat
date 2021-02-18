#ifndef DSL_CPP_GENERATOR
#define DSL_CPP_GENERATOR

#include <cstdio>
#include "dslCodePad.h"
#include "../ast/ASTNodeTypes.hpp"
#include "../parser/includeHeader.hpp"

class dsl_cpp_generator
{
  private:

  dslCodePad header;
  dslCodePad main;
  FILE *headerFile;
  FILE *bodyFile;
  char* fileName;

  public:
  dsl_cpp_generator()
  {
    headerFile=NULL;
    bodyFile=NULL;
    fileName=new char[1024];

  }


  bool generate();
  void generation_begin();
  void generation_end();
  bool openFileforOutput();
  void closeOutputFile();
  const char* convertToCppType(Type* type);
  const char* getOperatorString(int operatorId);
  void generateFunc(ASTNode* proc);
  void generateFuncHeader(Function* proc,bool isMainFile);
  void generateProcCall(proc_callStmt* procCall);
  void generateVariableDecl(declaration* decl);
  void generateStatement(statement* stmt);
  void generateAssignmentStmt(assignment* assignStmt);
  void generateWhileStmt(whileStmt* whilestmt);
  void generateForAll(forallStmt* forAll);
  void generateFixedPoint(fixedPointStmt* fixedPoint);
  void generateIfStmt(ifStmt* ifstmt);
  void generateDoWhileStmt(dowhileStmt* doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);
  void generateExpr(Expression* expr);
  void generate_exprRelational(Expression* expr);
  void generate_exprInfinity(Expression* expr);
  void generate_exprLiteral(Expression* expr);
  void generate_exprIdentifier(Expression* expr);

  void generateId();
  void generateOid();
  void addIncludeToFile(char* includeName,dslCodePad file,bool isCPPLib);
  void generatePropertyDefination(Type* type,char* Id);
  void findTargetGraph(vector<Identifier*> graphTypes,Type* type);
  void getDefaultValueforTypes(int type);


};

static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
#endif
