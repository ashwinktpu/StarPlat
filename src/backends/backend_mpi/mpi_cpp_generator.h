#ifndef MPI_CPP_GENERATOR
#define MPI_CPP_GENERATOR

#include <cstdio>
#include "../dslCodePad.h"
#include "../../ast/ASTNodeTypes.hpp"
#include "../../parser/includeHeader.hpp"
//#include "dslCodePad.h"


class mpi_cpp_generator
{
  private:

  dslCodePad header;
  dslCodePad main;
  FILE *headerFile;
  FILE *bodyFile;
  char* fileName;

  public:
  mpi_cpp_generator()
  {
    headerFile=NULL;
    bodyFile=NULL;
    fileName=new char[1024];

  }

  void setFileName(char* f);
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
  void generateFor(forallStmt* forStmt);
  void generateFixedPoint(fixedPointStmt* fixedPoint);
  void generateIfStmt(ifStmt* ifstmt);
  void generateDoWhileStmt(dowhileStmt* doWhile);
  void generateBFS();
  void generateBlock(blockStatement* blockStmt, bool includeBrace=true);
  void generateReductionStmt(reductionCallStmt* reductnStmt);
  void generateInnerReductionStmt(reductionCallStmt* stmt);
  void generateReductionStmtForSend(reductionCallStmt* stmt,bool send);
  void generateExprForSend(Expression* expr,int send, Identifier* remote);
  void generateAssignmentForSend(assignment* stmt,int send,Identifier* remote);
  void generatePropAccessForSend(PropAccess* lhs,int send,Identifier* remote);
  void generate_exprLiteralForSend(Expression* expr,int send,Identifier *remote);
  void generateArLForSend(Expression* stmt,int send, Identifier* remote);
  void generateBFSAbstraction(iterateBFS* bfsAbstraction);
  void generateExpr(Expression* expr);
  void generate_exprRelational(Expression* expr);
  void generate_exprInfinity(Expression* expr);
  void generate_exprLiteral(Expression* expr);
  void generate_exprIdentifier(Identifier* id);
  void generate_exprPropId(PropAccess* propId) ;
  void generate_exprPropIdReceive(PropAccess* propId) ;
  void generate_exprProcCall(Expression* expr);
  void generate_exprArL(Expression* expr);
  void generate_exprArLReceive(Expression* expr);
  void generateForAll_header();
  void generateForAllSignature(forallStmt* forAll);
  void generateForSignature(forallStmt* forAll);
  void generate_sendCall(statement* body);
  void generate_receiveCall(statement* body);
  void generate_receiveCallBFS(statement* body,int send,Identifier *id);
  void generate_addMessage(statement* body,int send,Identifier* id);
  void generateReceiveBlock(blockStatement* body);
  //void includeIfToBlock(forallStmt* forAll);
  bool neighbourIteration(char* methodId);
  bool allGraphIteration(char* methodId);
  bool elementsIteration(char* extractId);
  blockStatement* includeIfToBlock(forallStmt* forAll);
 // void generateIsFinished();
  void generateId();
  void generateOid();
  void addIncludeToFile(char* includeName,dslCodePad& file,bool isCPPLib);
  void generatePropertyDefination(Type* type,char* Id);
  void findTargetGraph(vector<Identifier*> graphTypes,Type* type);
  void getDefaultValueforTypes(int type);


};

static const char* INTALLOCATION = "new int";
static const char* BOOLALLOCATION = "new bool";
static const char* FLOATALLOCATION = "new float";
#endif
